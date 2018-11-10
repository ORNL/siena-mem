#include <sstream>
#include <cstdio>
#include <cfloat>
#include <memory>

#include "ar_memalloc.h"
#include "ar_cacheobj.h"
#include "ar_memobj.h"
#include "ar_dnode.h"
#include "ar_onode.h"
#include "ar_execblock.h"
#include "ar_memsim.h"
#include "ar_context.h"
#include "ar_core.h"
#include "ar_engine.h"


//constructor
Engine::Engine(ASTAppModel *app, ASTMachModel *mach, ASTMapModel *mapping)
    : AspenTool(app,mach,mapping)
{
    //init parallelism
    PushPar(1);

    init();
}


//destructor
Engine::~Engine()
{
    fini();
}

Context& Engine::get_ctx(size_t tid){return t_ctx[tid];}

void Engine::dump_state(){
  for (size_t tid=thread_st; tid<=thread_end; tid++)
    {
      t_ctx[tid].dump_state();
    }
  for(auto memsim : memsim_ptr_list)
    memsim->dump_state();

  for(auto kv : machine_caches)
    for(auto cache : kv.second)
      cache.dump_state();
}


void Engine::init_thread_ctx(CoreGroup &cores, ExecBlock& blk){
    
    //t_ctx = new Context[num_threads];
    t_ctx.resize(num_threads);
    
    thread_st = 0;
    thread_end= num_threads -1;
    global_control  = blk.get_global_control();
    size_t workload = global_control/num_threads;
    if( global_control%num_threads !=0 )
        workload++;
    
    for (size_t tid = thread_st; tid<=thread_end; tid++){
        
        size_t st = tid* workload;
        size_t end= st + workload;
        
        if(st>=global_control){
            
            for(size_t tt=tid;tt<=thread_end;tt++){
                num_finished_threads ++;
                t_ctx[tid].mark_done();
                INFO("tid=%u completed [%u/%u]", tid, num_finished_threads, num_threads);
            }
            //remember the first completed thread and also break the loop
            thread_end = tid-1;
                
        }else{
            if( end>global_control )
                end = global_control;
        
            t_ctx[tid].init(tid,st,end,cores,blk);
#ifdef DUMP_STATE
            INFO("tid=%u workshare [%u - %u]", tid, st, end);
#endif
        }
    }
    
}

void Engine::fini_thread_ctx(){

    for (size_t tid=thread_st; tid<=thread_end; tid++)
        t_ctx[tid].fini();

    t_ctx.clear();
    
    for (unordered_map<string, vector<CacheObj>>::iterator it = machine_caches.begin(); it != machine_caches.end(); ++it)
    {
        vector<CacheObj>& cache_list = it->second;
        for (auto &cache : cache_list){
            if(!cache.is_FLC())
                cache.print_stats();
        }
    }
    
    for (unordered_map<string, MemObj>::iterator it=machine_memories.begin(); it!=machine_memories.end(); it++)
    {
        (it->second).print_stats();
    }

}

void Engine::exec_node_longstep(string core_type, string node_id, size_t nThreads)
{
    //core parameters
    std::unordered_map<string, CoreGroup>::iterator it = machine_cores.find(core_type);
    assert( it != machine_cores.end() );
    CoreGroup &cores = it->second;
    
    //mem calibrate
    sync_ticks(cores);
    
    //context
    ExecBlock& blk = executeblock_map.at(node_id);
    
    //setup execution
    num_threads = nThreads;
    num_finished_threads=0;
    init_thread_ctx(cores, blk);
    
    //connect core and ctx
    cores.link_core_ctx(t_ctx);
    
    // warm up
    if(is_warmup){
        //exec_warmup();
        while( num_finished_threads < num_threads )
        for (size_t tid=thread_st; tid<=thread_end; tid++)
        {
            bool done = t_ctx[tid].warmup();
            num_finished_threads += done ?1 :0;
        }
    }else{
        
        uint64_t next_step = UINT64_MAX;
	const bool mode = (memsim_ptr_list.size()>0);
        //weave
        for (size_t tid=thread_st; tid<=thread_end; tid++)
        {
 	    t_ctx[tid].init_window();
            uint64_t t = t_ctx[tid].get_next_event(mode); 
            next_step  = (0<t && t<next_step) ?t : next_step;
        }
        //Bound
        for (auto memsim : memsim_ptr_list) {
            uint64_t t = memsim->get_next_event();
            next_step  = (0<t && t<next_step) ?t : next_step;
        }
        
        //record progress
        cores.record_start();
        
        //steping until all threads are done
        size_t step_cnt = 0;
        while( num_finished_threads < num_threads )
        //for(;step_cnt<100;)
        {            
            step_cnt++;
            uint64_t next_step_new = UINT64_MAX;
            for (auto memsim : memsim_ptr_list) {
                memsim->advance(next_step);
                uint64_t t = memsim->get_next_event();
                next_step_new  = (0<t && t<next_step_new) ?t : next_step_new;
            }
            for (size_t tid=thread_st; tid<=thread_end; tid++)
            {
                bool done = t_ctx[tid].advance(next_step);
                num_finished_threads += done ?1 :0;
		if(num_finished_threads<num_threads){
		  uint64_t t = t_ctx[tid].get_next_event(mode);
		  next_step_new  = (0<t && t<next_step_new) ?t : next_step_new;
		}else
		  next_step_new = next_step;
            }
            next_step = next_step_new;
            if(step_cnt%50000==0) INFO("progress [%d::%lu]", step_cnt, next_step);
            if(step_cnt%5000000==0) dump_state();
        }
    
    /* steping until all threads are done
    while( num_finished_threads < num_threads ){
        
        //Start of All threads
        for (size_t tid=thread_st; tid<=thread_end; tid++){
            
            Context& ctx = t_ctx[tid];
            
            //Step I: Fetch ops into the instruction buffer
            if(ctx.ib_load < ROB){
                ctx.fetch_ops();
            }
            //End of Step I Fetch OPS
            
            
#ifdef MEMSIM
            //Try to re-send memory requests if they are not accepted previously
            ctx.resend_pending_req()();
#endif
            
            //Step II: Dispatch OPS that has dependency resolved
            int remaining_AIPC= core_alu;
            int remaining_LD  = core_ld;
            int remaining_ST  = core_st;
            bool hasUnit      = true;
            
            if(cpu_cycles%10000000==0)
                INFO("Tid%i[cycle%lu]::win_head=%u,win_load=%u,LFB_occupancy=%u,fetched_global_step=%u",
                     tid, cpu_cycles, ctx.win_head, ctx.win_load, ctx.LFB_occupancy, ctx.fetched_global_step);
            
            
            //dispatch only if we have any updates
            if(ctx.has_ready_ops){
                
                size_t rob_load = (ctx.win_load<rob_depth ? ctx.win_load : rob_depth);
                size_t s_head   = ctx.win_head;
                
                while( rob_load>0 && hasUnit){
                    
                    OP_Top = t_ctx[tid].window[s_head];
                    
                    //Solve dependency
                    if( op.status == pending){
                        //Fastforwarding here that only wait for data dependency to complete if this op has a LLC miss ?
                        DataNode node  = t_ctx[tid].datanode_list[op.node_id];
                        GRPCTRL_T group= t_ctx[tid].nodegroup_list[node.group_id];
                        bool isReady   = true;
                        for(auto dep_node_id : node.prev){
                            DataNode dep_node= t_ctx[tid].datanode_list[dep_node_id];
                            bool isSameGroup = (dep_node_id>=group.st_node && dep_node_id<=group.end_node);
                            bool hasGlobalDep= (dep_node.retired_global_step >= op.global_step);
                            hasGlobalDep = hasGlobalDep && (dep_node.retired_global_step>=0);
                            bool hasGroupDep = (dep_node.retired_step >= op.step);
                            hasGroupDep = hasGroupDep && (dep_node.retired_step>=0);
                            isReady = (isSameGroup ?hasGroupDep : hasGlobalDep);
                            
                            if(!isReady) break;
                        }
                        op.status = (isReady ? ready : pending);
                        //if(isReady) cout << " Tid "<< tid <<": wondow["<< s_head<<"] change status:pending->ready"<<endl;
                    }
                    
                    //Dispatch ready ops
                    if( op.status == ready ){
#ifdef DEBUG_CORE
                        cout<<"Dispatch Tid"<< tid <<":: wondow["<< s_head<<"] node_id=" << op.node_id
                        <<", addr["<< op.addr<<"],comp="<<op.comp<< ", status="
                        <<op.status << ", step="<< op.step<<endl;
#endif
                        if(op.comp>remaining_AIPC){
                            t_ctx[tid].window[s_head].comp -= remaining_AIPC;
                            remaining_AIPC = 0;
                        }
                        else{
                            remaining_AIPC -= op.comp;
                            t_ctx[tid].window[s_head].comp = 0;
                            
                            if(op.type==LOAD && remaining_LD>0 && t_ctx[tid].LFB_occupancy<lfb_depth )
                            {
                                remaining_LD --;
                                t_ctx[tid].window[s_head].status = dispatched;
                                t_ctx[tid].has_complete_op=true;
                                
                                CACHE_SET::CACHEBLOCK evict_block;
                                evict_block = t_ctx[tid].l1cache->lookup_evict_update(op.addr,CACHE_BASE::ACCESS_TYPE_LOAD);
                                if(evict_block.tag==0){
                                    cache_lookup_load( s_head, tid );
                                }else{
                                    t_ctx[tid].window[s_head].status = completed;
                                    t_ctx[tid].has_complete_op=true;
                                    t_ctx[tid].has_ready_ops=true;
                                }
                            }//End of LOAD
                            else if(op.type==STORE && remaining_ST>0)
                            {
                                remaining_ST --;
                                t_ctx[tid].window[s_head].status = completed;
                                t_ctx[tid].has_complete_op = true;
                                
                                CACHE_SET::CACHEBLOCK evict_block;
                                evict_block = t_ctx[tid].l1cache->lookup_evict_update(op.addr,CACHE_BASE::ACCESS_TYPE_STORE);
                                if(evict_block.tag==0)
                                    cache_lookup_store( op.addr, tid );
                            }//End of STORE
                        }//End of op.comp<remaining_AIPC
                    }//End of ready ops
                    
                    int remaining_unit = (remaining_AIPC+remaining_LD+remaining_ST);
                    hasUnit = (remaining_unit>0) ?true : false;
                    s_head ++;
                    s_head = (s_head==ib_depth ? 0 : s_head);
                    rob_load--;
                }//Finish searching rob for ready ops
            }
            //End of dispatch ctx with updates
            
            //Stats
            int num_dispatched = core_ipc-(remaining_AIPC+remaining_LD+remaining_ST);
            t_ctx[tid].dispatch_util += ((double)num_dispatched)/core_ipc;
            t_ctx[tid].lfb_util += t_ctx[tid].LFB_occupancy/(double)lfb_depth;
            t_ctx[tid].pendreq_cycles += (double)t_ctx[tid].pendingreq_arr.size();
            t_ctx[tid].has_ready_ops = (num_dispatched>0);
            //End of Step II:  dispatching
            
            
            //Step III: Retire ops in-order, constrainted by IPC
            //Only search up to ROB window depth
            if(t_ctx[tid].has_complete_op)
            {
                size_t rob_load = (t_ctx[tid].win_load<rob_depth ? t_ctx[tid].win_load : rob_depth);
                size_t s_head =   t_ctx[tid].win_head;
                int retired   = 0;
                while( rob_load>0 && retired < core_ipc)
                {
                    OP_Top = t_ctx[tid].window[s_head];
                    if(op.status != completed)
                        break;
                    
                    t_ctx[tid].retired_ops ++;
#ifdef DEBUG_CORE
                    cout<<"Core::Tid"<< tid << "::Retire window["<<s_head<<"], addr="<<op.addr<<", node="<< op.node_id
                    << ", global_step="<<op.global_step <<", step=" << op.step << endl;
#endif
                    
                    if(op.mark){
                        t_ctx[tid].datanode_list[op.node_id].retired_step = op.step;
                        t_ctx[tid].datanode_list[op.node_id].retired_global_step = op.global_step;
#ifdef DEBUG_CORE
                        cout<<"Core::Tid"<< tid << "::Mark Node "<< op.node_id << ", global_step="
                        <<op.global_step <<", step=" << op.step<<endl;
#endif
                        //if(op.node_id==0 && tid==0)
                        //cout <<"Tid0:cycle:"<<cpu_cycles<<":globalstep:"<<op.global_step<<endl;
                        
                    }
                    retired++;
                    rob_load--;
                    s_head ++;
                    s_head = (s_head==ib_depth ? 0 : s_head);//circular buffer
                }
#ifdef DEBUG_CORE
                cout<<"Core::Tid"<< tid << "::retired="<<retired << endl;
#endif
                if(t_ctx[tid].win_load>0 && rob_load==0 &&
                   t_ctx[tid].fetched_global_step==t_ctx[tid].global_control_max)
                {
#pragma omp atomic update
                    num_finished_threads ++;
#ifdef VERBOSE
                    cout << "tid " << tid << " completed ("<< num_finished_threads<<" / "<<num_threads<<")"<<endl;
#endif
                }
                t_ctx[tid].win_head = s_head;
                t_ctx[tid].win_load -= retired;
                t_ctx[tid].has_complete_op = (retired==core_ipc);
                t_ctx[tid].has_ready_ops = (retired>0  ?true : t_ctx[tid].has_ready_ops);
            }
            //End of has_complete_ops
            //End of Step III Retire
            
        }
        //End of All threads
        
        //make sure all cores has updated to MC
        cpu_cycles ++;
        
#ifdef MEMSIM
        //Memory-side simulators
        for (unordered_map<string, MemSim*>::iterator it=machine_memories.begin(); it!=machine_memories.end(); it++)
        {
            MemSim* mem = (it->second);
            uint64_t cpu_tick = mem->cpu_tick;
            uint64_t mem_tick = mem->mem_tick;
            
            if(cpu_cycles%cpu_tick==(cpu_tick-1)){
                for (uint64_t j = 0; j < mem_tick; j++)
                    mem->clock();
            }
        }
        //End of all memories
        
        //Cache-side simulators
        for (size_t i=0;i<machine_caches.size();i++)
        {
            if(machine_caches[i].has_simulator()){
                MemSim* cache = machine_caches[i].get_simulator();
                uint64_t cpu_tick = cache->cpu_tick;
                uint64_t mem_tick = cache->mem_tick;
                
                if(cpu_cycles%cpu_tick==(cpu_tick-1)){
                    for (uint64_t j = 0; j < mem_tick; j++)
                        cache->clock();
                }
            }
        }
        //End of all caches
#endif
        
        
        //Do IO while memthread is updating
#ifdef SAVE_TRACE
        for (size_t tid=thread_st; tid<=thread_end; tid++){
            if(t_ctx[tid].trace_string.size() > MAX_TRACE_BUFFER){
                fprintf(t_ctx[tid].trace_file,"%s",t_ctx[tid].trace_string.c_str());
                t_ctx[tid].trace_string.clear();
            }
        }
#endif
        
    }
    //End of stepping until all threads are done*/
    

        INFO("total_ops = %lu, cycles=%lu", blk.get_total_ops(), next_step);
            
        //core record duration
        cores.record_end(next_step);
    }
    
    //Free Context
    fini_thread_ctx();

}

/*
void Engine::process_read_resp(ADDR_T addr_st, ADDR_T addr_end, size_t mem_level){

    for (int i=(int)mem_level-1; i>=0; i--) {
        
        for(unordered_map<ADDR_T,vector<REQ_T>>::iterator it = machine_caches[i].fill_buffer.begin();it!=machine_caches[i].fill_buffer.end();){
            
            ADDR_T addr = it->first;
#ifdef DEBUG_MEMSIM
            INFO("Level%u: addr=%lu",i, addr);
#endif
            if (  addr_st<=addr && addr<addr_end ) {
                for (auto req : it->second) {
                    size_t tid = req.thread_id;
                    t_ctx[tid].LFB_occupancy --;
#ifdef DEBUG_MEMSIM
                    INFO("process_read_resp::Level%u: t_ctx[%u].LFB_occupancy=%u",i,tid,t_ctx[tid].LFB_occupancy);
#endif
                    if(i==0){
                        size_t s_head = req.rob_id;
                        assert(t_ctx[tid].window[s_head].addr == req.addr );
                        t_ctx[tid].window[s_head].status = completed;
                        t_ctx[tid].has_complete_op=true;
                        t_ctx[tid].has_ready_ops=true;
#ifdef DEBUG_MEMSIM
                        INFO("Level0: complete t_ctx[%u].window[%u]",tid,s_head);
                        INFO("t_ctx[%u].LFB_occupancy=%u",tid,t_ctx[tid].LFB_occupancy);
#endif
                    }
                }
                it = machine_caches[i].fill_buffer.erase(it);
            }else
                ++it;
        }
    }
}

void Engine::process_write_resp(ADDR_T addr_st, ADDR_T addr_end, size_t mem_level){}


 void Engine::exec_warmup(){
 
 while( num_finished_threads < num_threads ){
 
 for (size_t tid=thread_st; tid<=thread_end;tid++){
 
 //Step I: Fetch OPS to warmup cache
 if(t_ctx[tid].fetched_global_step < t_ctx[tid].global_control_max)
 {
 //resume from the group
 size_t group_id= t_ctx[tid].curr_group_id;
 GRPCTRL_T group= t_ctx[tid].nodegroup_list[group_id];
 
 for(size_t node_id = group.st_node;node_id <= group.end_node; node_id++) {
 DataNode node = t_ctx[tid].datanode_list[node_id];
 size_t global_step= t_ctx[tid].fetched_global_step;
 size_t local_step = global_step*group.group_steps + group.curr_group_step;
 
 vector<ADDR_T> addr_list = node.gen_addr(local_step);
 for (size_t j=0; j<addr_list.size(); j++)
 {
 cache_warmup(addr_list[j],node.isWrite);
 }//end of all addresses from get_addr
 }
 
 //Start stepping forward to the next node
 t_ctx[tid].nodegroup_list[group_id].curr_group_step ++;
 if( t_ctx[tid].nodegroup_list[group_id].curr_group_step == group.group_steps ){
 t_ctx[tid].curr_group_id ++;
 if( t_ctx[tid].curr_group_id == t_ctx[tid].nodegroup_size ){
 t_ctx[tid].curr_group_id = 0;
 t_ctx[tid].fetched_global_step ++;
 }
 t_ctx[tid].nodegroup_list[t_ctx[tid].curr_group_id].curr_group_step = 0;
 if(t_ctx[tid].fetched_global_step==t_ctx[tid].global_control_max)
 num_finished_threads++;
 }//End stepping forward to the next node
 
 }//End of Step I Fetch OPS
 }//End of all simulated threasds
 }//End of warmup
 
 
 cout << HLINE << endl;
 cout << "Finish warming up caches:"<<endl;
 for (auto const& cache : machine_caches) {
 cache.print_stats();
 }
 cout << HLINE << endl;
 }
 
 void Engine::exec_node_cyclestep(string core_type, string node_id, size_t nThreads){
 
 //retrieve core parameters
 assert(machine_cores.find(core_type) != machine_cores.end() );
 CoreGroup cores = (machine_cores.find(core_type))->second;
 size_t core_frequency = cores.get_frequency();
 size_t rob_depth = cores.get_rob();
 size_t ib_depth  = cores.get_ib();
 size_t lfb_depth = cores.get_lfb();
 size_t core_alu = cores.get_alu();
 size_t core_ld  = cores.get_ld();
 size_t core_st  = cores.get_st();
 size_t core_ipc = cores.get_ipc();
 
 
 //calibrate core and mem freq
 sync_ticks(core_frequency);
 
 
 //keep history
 uint64_t cpu_cycles_prev = cpu_cycles;
 
 //setup global execution
 num_threads = nThreads;
 num_finished_threads=0;
 global_control = executeblock_map[node_id].global_control;
 
 
 //Alloc Thread Context
 init_thread_ctx(ib_depth, lfb_depth, node_id);
 
 
	//Fast warmup
	if(is_warmup){
 exec_warmup();
 return;
	}//End of warmup
 
 
 for (size_t tid=thread_st; tid<=thread_end; tid++)
 {
 t_ctx[tid].init_window();
 }
 
 //steping until all threads are done
 while( num_finished_threads < num_threads ){
 
 //Start of All threads
 for (size_t tid=thread_st; tid<=thread_end; tid++){
 
 Context& ctx = t_ctx[tid];
 
 //Step I: Fetch ops into the instruction buffer
 if(ctx.ib_load < ROB){
 ctx.fetch_ops();
 }
 //End of Step I Fetch OPS
 
 
 #ifdef MEMSIM
 //Try to re-send memory requests if they are not accepted previously
 ctx.resend_pending_req()();
 #endif
 
 //Step II: Dispatch OPS that has dependency resolved
 int remaining_AIPC= core_alu;
 int remaining_LD  = core_ld;
 int remaining_ST  = core_st;
 bool hasUnit      = true;
 
 if(cpu_cycles%10000000==0)
 INFO("Tid%i[cycle%lu]::win_head=%u,win_load=%u,LFB_occupancy=%u,fetched_global_step=%u",
 tid, cpu_cycles, ctx.win_head, ctx.win_load, ctx.LFB_occupancy, ctx.fetched_global_step);
 
 
 //dispatch only if we have any updates
 if(ctx.has_ready_ops){
 
 size_t rob_load = (ctx.win_load<rob_depth ? ctx.win_load : rob_depth);
 size_t s_head   = ctx.win_head;
 
 while( rob_load>0 && hasUnit){
 
 OP_Top = t_ctx[tid].window[s_head];
 
 //Solve dependency
 if( op.status == pending){
 //Fastforwarding here that only wait for data dependency to complete if this op has a LLC miss ?
 DataNode node  = t_ctx[tid].datanode_list[op.node_id];
 GRPCTRL_T group= t_ctx[tid].nodegroup_list[node.group_id];
 bool isReady   = true;
 for(auto dep_node_id : node.prev){
 DataNode dep_node= t_ctx[tid].datanode_list[dep_node_id];
 bool isSameGroup = (dep_node_id>=group.st_node && dep_node_id<=group.end_node);
 bool hasGlobalDep= (dep_node.retired_global_step >= op.global_step);
 hasGlobalDep = hasGlobalDep && (dep_node.retired_global_step>=0);
 bool hasGroupDep = (dep_node.retired_step >= op.step);
 hasGroupDep = hasGroupDep && (dep_node.retired_step>=0);
 isReady = (isSameGroup ?hasGroupDep : hasGlobalDep);
 
 if(!isReady) break;
 }
 op.status = (isReady ? ready : pending);
 //if(isReady) cout << " Tid "<< tid <<": wondow["<< s_head<<"] change status:pending->ready"<<endl;
 }
 
 //Dispatch ready ops
 if( op.status == ready ){
 #ifdef DEBUG_CORE
 cout<<"Dispatch Tid"<< tid <<":: wondow["<< s_head<<"] node_id=" << op.node_id
 <<", addr["<< op.addr<<"],comp="<<op.comp<< ", status="
 <<op.status << ", step="<< op.step<<endl;
 #endif
 if(op.comp>remaining_AIPC){
 t_ctx[tid].window[s_head].comp -= remaining_AIPC;
 remaining_AIPC = 0;
 }
 else{
 remaining_AIPC -= op.comp;
 t_ctx[tid].window[s_head].comp = 0;
 
 if(op.type==LOAD && remaining_LD>0 && t_ctx[tid].LFB_occupancy<lfb_depth )
 {
 remaining_LD --;
 t_ctx[tid].window[s_head].status = dispatched;
 t_ctx[tid].has_complete_op=true;
 
 CACHE_SET::CACHEBLOCK evict_block;
 evict_block = t_ctx[tid].l1cache->lookup_evict_update(op.addr,CACHE_BASE::ACCESS_TYPE_LOAD);
 if(evict_block.tag==0){
 cache_lookup_load( s_head, tid );
 }else{
 t_ctx[tid].window[s_head].status = completed;
 t_ctx[tid].has_complete_op=true;
 t_ctx[tid].has_ready_ops=true;
 }
 }//End of LOAD
 else if(op.type==STORE && remaining_ST>0)
 {
 remaining_ST --;
 t_ctx[tid].window[s_head].status = completed;
 t_ctx[tid].has_complete_op = true;
 
 CACHE_SET::CACHEBLOCK evict_block;
 evict_block = t_ctx[tid].l1cache->lookup_evict_update(op.addr,CACHE_BASE::ACCESS_TYPE_STORE);
 if(evict_block.tag==0)
 cache_lookup_store( op.addr, tid );
 }//End of STORE
 }//End of op.comp<remaining_AIPC
 }//End of ready ops
 
 int remaining_unit = (remaining_AIPC+remaining_LD+remaining_ST);
 hasUnit = (remaining_unit>0) ?true : false;
 s_head ++;
 s_head = (s_head==ib_depth ? 0 : s_head);
 rob_load--;
 }//Finish searching rob for ready ops
 }
 //End of dispatch ctx with updates
 
 //Stats
 int num_dispatched = core_ipc-(remaining_AIPC+remaining_LD+remaining_ST);
 t_ctx[tid].dispatch_util += ((double)num_dispatched)/core_ipc;
 t_ctx[tid].lfb_util += t_ctx[tid].LFB_occupancy/(double)lfb_depth;
 t_ctx[tid].pendreq_cycles += (double)t_ctx[tid].pendingreq_arr.size();
 t_ctx[tid].has_ready_ops = (num_dispatched>0);
 //End of Step II:  dispatching
 
 
 //Step III: Retire ops in-order, constrainted by IPC
 //Only search up to ROB window depth
 if(t_ctx[tid].has_complete_op)
 {
 size_t rob_load = (t_ctx[tid].win_load<rob_depth ? t_ctx[tid].win_load : rob_depth);
 size_t s_head =   t_ctx[tid].win_head;
 int retired   = 0;
 while( rob_load>0 && retired < core_ipc)
 {
 OP_Top = t_ctx[tid].window[s_head];
 if(op.status != completed)
 break;
 
 t_ctx[tid].retired_ops ++;
 #ifdef DEBUG_CORE
 cout<<"Core::Tid"<< tid << "::Retire window["<<s_head<<"], addr="<<op.addr<<", node="<< op.node_id
 << ", global_step="<<op.global_step <<", step=" << op.step << endl;
 #endif
 
 if(op.mark){
 t_ctx[tid].datanode_list[op.node_id].retired_step = op.step;
 t_ctx[tid].datanode_list[op.node_id].retired_global_step = op.global_step;
 #ifdef DEBUG_CORE
 cout<<"Core::Tid"<< tid << "::Mark Node "<< op.node_id << ", global_step="
 <<op.global_step <<", step=" << op.step<<endl;
 #endif
 //if(op.node_id==0 && tid==0)
 //cout <<"Tid0:cycle:"<<cpu_cycles<<":globalstep:"<<op.global_step<<endl;
 
 }
 retired++;
 rob_load--;
 s_head ++;
 s_head = (s_head==ib_depth ? 0 : s_head);//circular buffer
 }
 #ifdef DEBUG_CORE
 cout<<"Core::Tid"<< tid << "::retired="<<retired << endl;
 #endif
 if(t_ctx[tid].win_load>0 && rob_load==0 &&
 t_ctx[tid].fetched_global_step==t_ctx[tid].global_control_max)
 {
 #pragma omp atomic update
 num_finished_threads ++;
 #ifdef VERBOSE
 cout << "tid " << tid << " completed ("<< num_finished_threads<<" / "<<num_threads<<")"<<endl;
 #endif
 }
 t_ctx[tid].win_head = s_head;
 t_ctx[tid].win_load -= retired;
 t_ctx[tid].has_complete_op = (retired==core_ipc);
 t_ctx[tid].has_ready_ops = (retired>0  ?true : t_ctx[tid].has_ready_ops);
 }
 //End of has_complete_ops
 //End of Step III Retire
 
 }
 //End of All threads
 
 //make sure all cores has updated to MC
 cpu_cycles ++;
 
 #ifdef MEMSIM
 //Memory-side simulators
 for (unordered_map<string, MemSim*>::iterator it=machine_memories.begin(); it!=machine_memories.end(); it++)
 {
 MemSim* mem = (it->second);
 uint64_t cpu_tick = mem->cpu_tick;
 uint64_t mem_tick = mem->mem_tick;
 
 if(cpu_cycles%cpu_tick==(cpu_tick-1)){
 for (uint64_t j = 0; j < mem_tick; j++)
 mem->clock();
 }
 }
 //End of all memories
 
 //Cache-side simulators
 for (size_t i=0;i<machine_caches.size();i++)
 {
 if(machine_caches[i].has_simulator()){
 MemSim* cache = machine_caches[i].get_simulator();
 uint64_t cpu_tick = cache->cpu_tick;
 uint64_t mem_tick = cache->mem_tick;
 
 if(cpu_cycles%cpu_tick==(cpu_tick-1)){
 for (uint64_t j = 0; j < mem_tick; j++)
 cache->clock();
 }
 }
 }
 //End of all caches
 #endif
 
 
 //Do IO while memthread is updating
 #ifdef SAVE_TRACE
 for (size_t tid=thread_st; tid<=thread_end; tid++){
 if(t_ctx[tid].trace_string.size() > MAX_TRACE_BUFFER){
 fprintf(t_ctx[tid].trace_file,"%s",t_ctx[tid].trace_string.c_str());
 t_ctx[tid].trace_string.clear();
 }
 }
 #endif
 
 }
 //End of stepping until all threads are done
 
 
 
 uint64_t elapsed_cycles = cpu_cycles-cpu_cycles_prev;
 cores.inc_cycles(elapsed_cycles);
 
 //Free Thread Context
 fini_thread_ctx(elapsed_cycles);
 
 }
 
 
 void Engine::cache_warmup(ADDR_T addr, bool isWrite){
 
 for(auto &cache : machine_caches) {
 
 CACHE_BASE::ACCESS_TYPE type=(isWrite ?CACHE_BASE::ACCESS_TYPE_STORE : CACHE_BASE::ACCESS_TYPE_LOAD);
 
 CACHE_SET::CACHEBLOCK evict_block = cache.lookup_evict_update(addr,type);
 
 if(evict_block.tag==1) break;
 }
 }
 
 bool Engine::cache_lookup_load(size_t s_head, size_t tid){
 
 OP_Top = t_ctx[tid].window[s_head];
 bool isMiss = true;
 ADDR_T wb_addr_arr[8];
 size_t num_wb = 0;
 
 if(t_ctx[tid].is_pending_buffer_full()) return;
 
 REQ_T req = {op.addr, LOAD, tid, nullptr, s_head, cpu_cycles, 0};
 
 t_ctx[tid].load(req);
 
 
 #ifdef CACHESIM
 for (auto &cache : machine_caches) {
 
 ADDR_T tag = (op.addr>>(cache.lineshift));
 tag <<= cache.lineshift;
 
 if(cache.fill_buffer.find(tag)!=cache.fill_buffer.end()){
 REQ_T req;
 req.thread_id = tid;
 req.rob_id = s_head;
 req.addr = op.addr;
 machine_caches[i].fill_buffer[tag].push_back(req);
 t_ctx[tid].LFB_occupancy ++;
 
 isMiss=false;
 (cache.cache_map)->upd_cache_stats(CACHE_BASE::ACCESS_TYPE_LOAD, true);//Count hit in LFB as cache hit
 #ifdef DEBUG_CACHESIM
 INFO("CACHE::Level%u::Tid%u HIT in LFB, fill_buffer[%lu].size=%u",i,tid,tag,machine_caches[i].fill_buffer[tag].size());
 #endif
 break;
 }
 #ifdef DEBUG_CACHESIM
 cout<<"CACHE::Level"<< i <<"::Tid"<<tid<<" MISS LFB: tag="<< tag <<endl;
 #endif
 CACHE_SET::CACHEBLOCK   evict_block;
 #pragma omp critical
 {evict_block = (cache.cache_map)->lookup_evict_update(op.addr,CACHE_BASE::ACCESS_TYPE_LOAD);}
 
 //Hit: assume Valid
 if( evict_block.tag==1 ){//&& evict_block.valid==1
 
 if( cache.has_simulator() ){
 t_ctx[tid].LFB_occupancy ++;
 
 //Use simulator
 REQ_T req;
 req.addr = op.addr;
 req.type = LOAD;
 req.mem_ptr   = cache.cache_sim;
 req.thread_id = tid;
 req.rob_id = s_head;
 bool accepted = false;
 #pragma omp critical
 {accepted = (cache.cache_sim)->insert(req);}
 if(!accepted){//need re-send if not accepted
 t_ctx[tid].pendingreq_arr.push_back(req);
 }
 #ifdef DEBUG_CACHESIM
 cout<<"CACHE::Level"<< i <<"::Tid"<<tid<<" HIT insert READ "<< req.addr
 << " to cache "<< (cache.cache_sim)->mem_name<<", accepted="<< accepted <<endl;
 #endif
 }
 else{
 t_ctx[tid].window[s_head].status = completed;
 t_ctx[tid].has_complete_op=true;
 t_ctx[tid].has_ready_ops=true;
 }
 
 isMiss = false;
 break;
 }
 else{//Miss,Dirty-Evict
 
 vector<REQ_T> list;
 machine_caches[i].fill_buffer[tag]=list;//init an empty list
 
 #ifdef DEBUG_CACHESIM
 cout<<"CACHE::Level"<< i <<"::Tid"<<tid<<" MISS, add fill_buffer: tag="<<tag<<endl;
 #endif
 if(evict_block.valid==1 && evict_block.dirty==1)
 wb_addr_arr[num_wb++]= evict_block.addr;
 
 isMiss = true;
 }
 }
 //End of all levels of caches
 #endif
 
 
 //if has LLC miss
 if(isMiss){
 t_ctx[tid].LFB_occupancy ++;
 MemSim* mem  = op.mem_ptr;
 
 #ifdef MEMSIM
 REQ_T req;
 req.addr = op.addr;
 req.type = LOAD;
 req.mem_ptr   = mem;
 req.thread_id = tid;
 req.rob_id = s_head;
 bool accepted = false;
 #pragma omp critical
 {accepted=mem->insert(req);}
 if(!accepted){//need re-send if not accepted
 t_ctx[tid].pendingreq_arr.push_back(req);
 }
 #ifdef DEBUG_CACHESIM
 cout<<"Tid"<<tid<<" insert READ "<< req.addr<< " to memory "
 <<mem->mem_name<<", accepted="<< accepted <<endl;
 #endif
 //Writeback requests
 for (int i=0; i<num_wb; i++) {
 ADDR_T wb_addr = wb_addr_arr[i];
 MemSim* wb_mem_ptr = find_memory(wb_addr);
 REQ_T req2;
 req2.addr = wb_addr;
 req2.type = STORE;
 req2.mem_ptr = wb_mem_ptr;
 accepted = false;
 #pragma omp critical
 {accepted  = (req2.mem_ptr)->insert(req2);}
 if(!accepted){  //need re-send if not accepted
 t_ctx[tid].pendingreq_arr.push_back(req2);
 }
 #ifdef DEBUG_MEMSIM
 cout<<"Tid "<<tid<<" insert WRITE "<< req2.addr<< " to memory "
 << (wb_mem_ptr)->mem_name<<", accepted="<< accepted <<endl;
 #endif
 }
 
 #endif
 
 #ifdef SAVE_TRACE
 //buffer traces
 std::stringstream ss;
 ss << "0x"<< std::hex<< addr <<std::dec<<" READ "<<cpu_cycles<<"\n";
 t_ctx[tid].trace_string.append(ss.str());
 
 //Writeback request
 for (int i=0; i<num_wb; i++) {
 ADDR_T wb_addr = wb_addr_arr[i];
 std::stringstream ss;
 ss << "0x"<< std::hex<< wb_addr <<std::dec<<" WRITE "<<cpu_cycles<<"\n";
 t_ctx[tid].trace_string.append(ss.str());
 }
 #endif
 }
 //End of LLC miss
 
 //t_ctx[tid].window[s_head].retire_cycle = 0;
 
 return isMiss;
 }
 
 void Engine::cache_lookup_store(ADDR_T addr, size_t tid){
 
 for (size_t i=0; i<machine_caches.size(); i++) {
 CacheObj cache = machine_caches[i];
 CACHE_SET::CACHEBLOCK   evict_block;
 #pragma omp critical
 {evict_block = (cache.cache_map)->lookup_evict_update(addr,CACHE_BASE::ACCESS_TYPE_STORE);}
 
 if(evict_block.tag==1 ) break; //Hit
 else{
 
 //send mem request if dirty cacheline is evicted
 if( evict_block.valid==1 && evict_block.dirty==1 ){
 #ifdef MEMSIM
	MemSim* wb_mem_ptr = find_memory(evict_block.addr);
	REQ_T req;
	req.addr = evict_block.addr;
	req.type = STORE;
	req.mem_ptr   = wb_mem_ptr;
	bool accepted = false;
 #pragma omp critical
	{accepted=wb_mem_ptr->insert(req);}
	if(!accepted){//need re-send if not accepted
 t_ctx[tid].pendingreq_arr.push_back(req);
	}
 #ifdef DEBUG_MEMSIM
	cout<<"Tid "<<tid<<" insert WRITE "<< req.addr
 << " to memory "<< wb_mem_ptr->mem_name<<", accepted="<< accepted <<endl;
 #endif
 #endif
 
 #ifdef SAVE_TRACE
	//buffer traces
	std::stringstream ss;
	ss << "0x"<< std::hex<< evict_block.addr <<std::dec<<" WRITE "<<cpu_cycles<<"\n";
	t_ctx[tid].trace_string.append(ss.str());
 #endif
 }//End of writeback dirty cacheline
 
 break;
 
 }//End of MISS
 }//End of all cache levels
 }
 */
