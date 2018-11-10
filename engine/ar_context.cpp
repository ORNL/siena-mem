#include <iostream>
#include "ar_execblock.h"
#include "ar_context.h"
#include "ar_dnode.h"
#include "ar_core.h"
#include "ar_memsim.h"
#include "ar_cacheobj.h"
#include "ar_memobj.h"
#include "ar_engine.h"



void Context::init(size_t _tid, size_t _st, size_t _end, CoreGroup &core_group, ExecBlock& blk)
{
    assert(!is_done);
    
    tid = _tid;
    t_cpu = 0;
    fetched_global_step = _st;
    global_control_max  = _end;
    
    datanode_list  = blk.get_datanodes();
    nodegroup_list = blk.get_nodegroups();
    curr_group_id  = 0;
    watermark      = blk.get_window_size();
    
    //setup core parameters
    rob_depth  = core_group.get_rob();
    ib_depth   = rob_depth*4;
    lfb_depth = core_group.get_lfb();
    alu = core_group.get_alu();
    ld  = core_group.get_ld();
    st  = core_group.get_st();
    
    //stats
    //retired_ops    = 0;
    //ops_step       = blk.get_ops_step();
    dispatch_util  = 0.;
    lfb_util       = 0.;
    pendreq_cycles = 0.;

    has_complete_op = false;
    has_ready_ops   = true;
    
}


void Context::fini(){
    
    uint64_t elapsed_cycles = t_cpu;
    
    INFO("[%d]::cycles=%lu",tid, t_cpu);

    l1->print_stats();
}


Context::Context(){
    
    //l1cache = nullptr;
    //window  = nullptr;
    //nodegroup_list = nullptr;
    //datanode_list  = nullptr;
    
    is_done=false;
    
}

Context::~Context(){
    
    //if(l1cache) delete(l1cache);
    //if(window)  delete []window;
    //if(nodegroup_list) delete []nodegroup_list;
    //if(datanode_list)  delete []datanode_list;
}


uint64_t Context::get_next_event(bool mode)const{
  
  uint64_t t0 = (window[win_head].status==pending ?window[win_head].dispatch : window[win_head].complete);
  uint64_t t  = (mode ? time_next_event : t0);

  return t;
}


void Context::init_ports(){
    
    t_cpu = 0;
    
    t_alu = 1;
    avail_alu = alu;
    
    t_ld  = 1;
    avail_ld = ld;
    
    t_st  = 1;
    avail_st = st;
}


void Context::connect_core(Core& core){

    l1=core.get_l1_ptr();
}


size_t Context::fetch_global_step(size_t pos){
    
    assert( fetched_global_step<global_control_max );
    assert( curr_group_id==0 );
    
    bool done = false;
    size_t nodegroup_size = nodegroup_list.size();
    
    while( !done )
    {
        GRPCTRL_T  &group = nodegroup_list[curr_group_id];
        size_t local_step = fetched_global_step*group.group_steps + group.curr_group_step;
        
        for (size_t node_id = group.st_node; node_id <= group.end_node; node_id++)
        {
            DataNode &node  = datanode_list[node_id];
            vector<ADDR_T> addr_list = node.gen_addr(local_step);
            
            const OpType type = node.get_op();
            const size_t ops  = node.get_comp();
            const size_t prev = node.get_num_prev();
            MemObj* memobj_ptr= node.get_mem_ptr();

            std::vector<OFFSET_T> next;
            for (auto next_node_id : node.next){
                DataNode  &next_node = datanode_list[next_node_id];
                size_t next_group_id = next_node.get_group_id();
                size_t offset        = next_node.offset-node.offset;
                if(next_group_id==curr_group_id){
                    OFFSET_T o ={offset, next_node.ops_step};
                    next.emplace_back(o);
                }else{
                    offset = offset - group.num_ops*group.curr_group_step;
                    GRPCTRL_T  &next_group = nodegroup_list[next_group_id];
                    for (size_t i=0; i<next_group.group_steps;) {
                        size_t adj_offset = offset + next_group.num_ops*i; i++;
                        OFFSET_T o = {adj_offset, next_node.ops_step};
                        next.emplace_back(o);
                        //INFO("node[%d].offset=%d, next_node[%d].offset=%d, adj_offset=%d",node_id,node.offset,next_node_id,next_node.offset, adj_offset);
                    }
                    
                }
            }
            
            size_t i = 0;
            for (auto addr : addr_list)
            {
                if(pos>=ib_depth) pos = 0;
                //assert(pos!=win_head || win_load==0);
                //if(pos==win_head && win_load>0)
                    //PANIC("pos=%u, win_head=%u, win_load=%u, ib_depth=%u, fetched_global_step=%u",pos,win_head,win_load,ib_depth,fetched_global_step);
                
                OP_T op = {issued, pos, prev, type, ops, addr,0,0,memobj_ptr};
                for( auto o : next ){
                    for(size_t p=0;p<o.num;p++)
                        op.next.push_back( o.offset - i + p);
                }
                i++;
                window[pos++] = op;
                win_load ++;
            }
        }
        
        //Start stepping forward to the next node
        group.curr_group_step ++;
        if(  group.curr_group_step == group.group_steps ){
            
            group.curr_group_step = 0;
            curr_group_id ++;
            
            if( curr_group_id == nodegroup_size ){
                curr_group_id = 0;
                fetched_global_step ++;
                done = true;
            }
        }
        //End stepping forward to the next node
    }
    
    //retired_ops += ops_step;
    assert( curr_group_id==0 );
    
    return pos;
    
}


inline bool Context::try_dispatch_ld(REQ_T req){

    //calibrate
    t_ld = (t_ld>t_cpu ?t_ld : t_cpu);
    
    //only dispatch those available earlier than the port
    if( req.dispatch<=t_ld && l1->get_num_pending_cl()<lfb_depth )
    {
        req.dispatch = t_ld;
        avail_ld --;
        if (avail_ld == 0){
            avail_ld = ld;
            t_ld ++;
        }
        uint64_t complete = l1->load(req);
        window[req.pos].status   = dispatched;
        window[req.pos].complete = complete;
        return true;
    }
    return false;
}

inline bool Context::try_dispatch_st(REQ_T req){
    
    //calibrate
    t_st = (t_st>t_cpu ?t_st : t_cpu);
    
    //only dispatch those available earlier than the port
    if( req.dispatch<=t_st )
    {
        req.dispatch = t_st;
        avail_st --;
        if (avail_st ==0) {
            avail_st = st;
            t_st ++;
        }
        l1->store(req);
        window[req.pos].status   = dispatched;
        window[req.pos].complete = t_st;
        return true;
    }
    return false;
}

inline void Context::dispatch(OP_T &op){
    
  if( op.prev>0 ) {op.status = pending;op.dispatch=0; return;}
    
    //add delays
    op.dispatch = t_cpu;
    if (op.comp > 0) {
        
        //calibrate
        t_alu = (t_alu>t_cpu ?t_alu : t_cpu);
        
        //cap at dispatch rate
        size_t min = (avail_alu<op.comp ?avail_alu : op.comp);
        op.comp   -= min;
        avail_alu -= min;
        if (avail_alu ==0) {
            avail_alu = alu;
            t_alu ++;
            
            size_t dt = op.comp/alu;
            size_t dc = op.comp - dt*alu;
            avail_alu = alu - dc;
            t_alu    += dt;
            op.comp   = 0;
        }
        assert(op.comp==0);
        
        op.dispatch = t_alu;
    }

    switch (op.type) {
        case LOAD:
        {
            REQ_T req = {op.addr, op.dispatch, this, op.pos, op.mem_ptr, nullptr, 0};
            //if cannot dispatch, pend it
            if( !try_dispatch_ld(req) ){
                pending_ld_list.push_back(req);
                op.status = pending;
            }
            break;
        }
        case STORE:
        {
            REQ_T req = {op.addr, op.dispatch, this, op.pos, op.mem_ptr, nullptr, 0};
            if( !try_dispatch_st(req)){
                pending_st_list.push_back(req);
                op.status = pending;
            }
            break;
        }
        default:
        {
            PANIC("Invalid op type %d", op.type);
            break;
        }
    }
    
#ifdef DEBUG_CORE
    DB("[%u][%u]::status=%d, prev=%d, dispatch=%lu, complete=%lu, [t_alu=%lu, t_ld=%lu, t_st=%lu]",
         tid,op.pos,op.status,op.prev,op.dispatch,op.complete, t_alu, t_ld, t_st);
#endif
    
}


void Context::init_window(){

    init_ports();
        
    size_t num_steps = ib_depth/watermark;
    num_steps = (num_steps<2) ? 2 : num_steps;
    size_t max_steps = global_control_max-fetched_global_step;
    num_steps = (num_steps<max_steps) ?num_steps : max_steps;
    ib_depth  = num_steps*watermark;

    window.resize(ib_depth);
    size_t pos = 0;
    while( num_steps>0 ){
        pos = fetch_global_step(pos);
        num_steps --;
    }
#ifdef DUMP_STATE
    INFO("[%u]::fetched_global_step=%d, global_control_max=%u, ib_depth=%u, watermark=%u, pos=%u",
            tid, fetched_global_step, global_control_max, ib_depth, watermark, pos);
#endif
    assert(pos==ib_depth);
    
    win_load = ib_depth;
    win_head = 0;
    size_t rob_load = (rob_depth<win_load ? rob_depth : win_load);
    size_t rob_tail = win_head + rob_load;
    time_next_event = UINT64_MAX;
    
    for (size_t pos=win_head; pos<rob_tail; pos++) {
        OP_T &op = window[pos];
        dispatch(op);
        uint64_t t_next = (op.status == pending ?op.dispatch : op.complete);
        time_next_event = (t_next<time_next_event ?t_next : time_next_event);
    }
#ifdef DUMP_STATE    
    INFO("[%u]::win_head=%u, win_load=%u, rob_depth=%u, time_next_event=%lu",
       tid,win_head,win_load,rob_depth, time_next_event);
#endif    
#ifdef DEBUG_CORE
    size_t id=0;
    for(auto op : window){
        stringstream ss;
        ss<<"{";
        for(auto n : op.next) ss<<n<<" ";
        ss<<"}";
        string next_str = ss.str();
        DB("[%u][%u]=[%u, %u, %u, 0x%lx, %s]",tid, id++, op.prev, op.type, op.comp, op.addr, next_str.c_str());
    }
#endif
    
    //return time_next_event;
}


void Context::dump_state(){
#ifdef DUMP_STATE    
    DB("[%u] [t_cpu=%lu, time_next_event=%lu, win_head=%u, win_load=%u, has_complete_op=%d, global_step=%d, t_alu=%lu, t_ld=%lu, t_st=%lu]",
       tid, t_cpu,time_next_event, win_head, win_load,has_complete_op,fetched_global_step, t_alu, t_ld, t_st);

    size_t rob_load = (rob_depth<win_load ? rob_depth : win_load);
    size_t pos = win_head;
    for (;rob_load>0;rob_load--) {
        pos = (pos>=ib_depth ? 0 : pos);
        OP_T &op = window[pos];
        pos++;
        DB("[%u][%u] status=%d, prev=%d, dispatch=%lu, complete=%lu",
            tid, op.pos, op.status, op.prev, op.dispatch, op.complete);
    }
#endif
}


bool Context::advance(uint64_t t){
    
    if(is_done) return false;

#ifdef DEBUG_CORE
    DB("[%u] step from %lu to %lu [time_next_event=%lu, win_head=%u, win_load=%u,has_complete_op=%d,global_step=%d]",
       tid, t_cpu, t,time_next_event, win_head, win_load,has_complete_op,fetched_global_step);
#endif

    t_cpu = t;
    
    vector<REQ_T>::iterator it = pending_ld_list.begin();
    for (; it!=pending_ld_list.end(); ) {
        if( try_dispatch_ld(*it) ){
            it = pending_ld_list.erase(it);
            has_complete_op = true;
        }else{
            ++it;
        }
    }
    
    it = pending_st_list.begin();
    for (; it!=pending_st_list.end(); ) {
        if( try_dispatch_st(*it) ){
            it = pending_st_list.erase(it);
            has_complete_op = true;
        }else{
            ++it;
        }
    }
    
    //bool special = (tid==5 && t_cpu>=3050);
    if(!has_complete_op && time_next_event>t_cpu ) return false;

        
    has_complete_op = false;
    size_t rob_load = (rob_depth<win_load ? rob_depth : win_load);
    size_t pos = win_head;
    for (;rob_load>0;rob_load--) {
        pos = (pos>=ib_depth ? 0 : pos);
        OP_T &op = window[pos];
        
#ifdef DEBUG_CORE
        DB("[%u][%u]before::status=%d, prev=%d, dispatch=%lu, complete=%lu, [t_alu=%lu, t_ld=%lu, t_st=%lu]",
             tid,op.pos,op.status,op.prev,op.dispatch,op.complete, t_alu, t_ld, t_st);
#endif
        switch (op.status) {
            case dispatched:
            {
                if( 0<op.complete && op.complete<=t_cpu ){
                    op.status = completed;
                    if(op.pos==win_head){
                        retire_head();
                    }
                    if( !is_done )
                        for(auto offset : op.next){
                        size_t child_pos = (op.pos + offset)%ib_depth;
                        OP_T&  child_op  = window[child_pos];
                            if(child_op.prev==0) PANIC("after window[%u] next_offset=%u, child_pos=%u, ib_depth=%u,child_op.prev=%u", op.pos, offset, child_pos,ib_depth,child_op.prev);
                        child_op.prev--;
                        if(child_op.prev==0 && child_op.status==pending)
                            dispatch(child_op);
                    }
                }
                break;
            }
            case completed:
            {
                if(op.pos==win_head)
                    retire_head();
                break;
            }
            default:
                break;
        }
#ifdef DEBUG_CORE
        DB("[%u][%u]after::status=%d, prev=%d, dispatch=%lu, complete=%lu, [t_alu=%lu, t_ld=%lu, t_st=%lu]",
             tid,op.pos,op.status,op.prev,op.dispatch,op.complete, t_alu, t_ld, t_st);
#endif
        
        pos++;
    }
    
    time_next_event = UINT64_MAX;
    //may have been updated
    rob_load = (rob_depth<win_load ? rob_depth : win_load);
    pos = win_head;
    for (;rob_load>0;rob_load--) {
        pos = (pos>=ib_depth ? 0 : pos);
        OP_T &op = window[pos];
        uint64_t t_next = (op.status == pending ?op.dispatch : op.complete);
        time_next_event = (t_cpu<t_next && t_next<time_next_event) ?t_next : time_next_event;
        pos++;
    }
    
#ifdef DEBUG_CORE
    //if(special) PANIC("special %d",special);
    DB("[%u][t_cpu=%lu, time_next_event=%lu, win_head=%u, win_load=%u, global_step=%lu]", 
       tid,t_cpu, time_next_event,win_head,win_load,fetched_global_step);
#endif
    
    return is_done;
}


bool Context::advance_bk(uint64_t t){
    
#ifdef DEBUG_CORE
    assert(t>t_cpu);
    DB("[%u] step from %lu to %lu [win_head=%u, win_load=%u]", tid, t_cpu, t, win_head, win_load);
#endif
    
    t_cpu = t;
    
    if(is_done ||  time_next_event>t_cpu) return false;
    
    vector<REQ_T>::iterator it = pending_ld_list.begin();
    for (; it!=pending_ld_list.end(); ) {
        if( try_dispatch_ld(*it) ){
            it = pending_ld_list.erase(it);
        }else{
            ++it;
        }
    }
    
    it = pending_st_list.begin();
    for (; it!=pending_st_list.end(); ) {
        if( try_dispatch_st(*it) ){
            it = pending_st_list.erase(it);
        }else{
            ++it;
        }
    }

    size_t rob_load = (rob_depth<win_load ? rob_depth : win_load);
    size_t pos = win_head;
    for (;rob_load>0;rob_load--) {
        pos = (pos>=ib_depth ? 0 : pos);
        OP_T &op = window[pos];
        
#ifdef DEBUG_CORE
        DB("[%u][%u]before::status=%d, prev=%d, dispatch=%lu, complete=%lu, [t_alu=%lu, t_ld=%lu, t_st=%lu]",
           tid,op.pos,op.status,op.prev,op.dispatch,op.complete, t_alu, t_ld, t_st);
#endif
        switch (op.status) {
            case dispatched:
            {
                if( 0<op.complete && op.complete<=t_cpu ){
                    op.status = completed;
                    if(op.pos==win_head){
                        retire_head();
                    }
                    if( !is_done )
                        for(auto offset : op.next){
                            size_t child_pos = (op.pos + offset)%ib_depth;
                            OP_T&  child_op  = window[child_pos];
                            child_op.prev--;
                            if(child_op.prev==0 && child_op.status==pending)
                                dispatch(child_op);
                        }
                }
                break;
            }
            case completed:
            {
                if(op.pos==win_head)
                    retire_head();
                break;
            }
            default:
                break;
        }
#ifdef DEBUG_CORE
        DB("[%u][%u]after::status=%d, prev=%d, dispatch=%lu, complete=%lu, [t_alu=%lu, t_ld=%lu, t_st=%lu]",
           tid,op.pos,op.status,op.prev,op.dispatch,op.complete, t_alu, t_ld, t_st);
#endif
        
        pos++;
    }
    
    time_next_event = UINT64_MAX;
    //may have been updated
    rob_load = (rob_depth<win_load ? rob_depth : win_load);
    pos = win_head;
    for (;rob_load>0;rob_load--) {
        pos = (pos>=ib_depth ? 0 : pos);
        OP_T &op = window[pos];
        uint64_t t_next = (op.status == pending ?op.dispatch : op.complete);
        time_next_event = (t_next==0 || (t_cpu<t_next && t_next<time_next_event) ) ?t_next : time_next_event;
        pos++;
    }
    
#ifdef DEBUG_CORE
    DB("[%u][t_cpu=%lu, time_next_event=%lu, win_head=%u, win_load=%u, global_step=%lu]", tid,t_cpu, time_next_event,win_head,win_load,fetched_global_step);
#endif
    
    return is_done;
}


void Context::complete_op(REQ_T req){
    
    size_t pos = req.pos;
    OP_T& op = window[pos];
    
#ifdef DEBUG_CORE
    assert(op.addr == req.addr );
    assert(op.status == dispatched );
    assert(op.complete == 0 );
#endif
    
    op.complete = req.complete;
    has_complete_op = true;
}


void Context::retire_head(){
    win_head ++;
    win_head = (win_head==ib_depth ? 0: win_head);
    win_load --;
    if(win_load<rob_depth){
        size_t start = (win_head+win_load)%ib_depth;
        while(win_load<=(ib_depth-watermark) && fetched_global_step<global_control_max ){
            start = fetch_global_step(start);
        }
    }
    
#ifdef DEBUG_CORE
    DB("[%u]::win_head=%d, win_load=%d",tid, win_head, win_load);
#endif
    
    if(win_load>=rob_depth){
        dispatch(window[(win_head+rob_depth-1)%ib_depth]);
    }
    
    if(win_load==0 && fetched_global_step==global_control_max){
        is_done = true;
#ifdef DEBUG_CORE
	DB("[%u]::Done!",tid);
#endif
    }
}


void Context::fetch_ops(){

    const size_t max_load = ib_depth-rob_depth;
    const size_t nodegroup_size = nodegroup_list.size();
    while( (fetched_global_step<global_control_max) && (win_load<max_load) )
    {
        //resume from the group
        size_t group_id = curr_group_id;
        GRPCTRL_T group = nodegroup_list[group_id];
        
        for (size_t node_id = group.st_node; node_id <= group.end_node; node_id++) {
            DataNode node     = datanode_list[node_id];
            size_t global_step= fetched_global_step;
            size_t local_step = global_step*group.group_steps + group.curr_group_step;
            
            vector<ADDR_T> addr_list = node.gen_addr(local_step);
            
            //expand
            if( (addr_list.size() + win_load)>ib_depth ){
                ib_depth += ib_depth;
                window.resize(ib_depth);
            }
            
            const OP_STATUS_T status = (node.has_dependency() ? pending : ready);
            const OpType type = node.get_op();
            MemObj* mem_ptr   = node.mem_ptr;
            const size_t flops= node.num_flops_per_access;
            
            size_t s_tail  = ( win_head + win_load )%ib_depth;
            for (size_t j=0; j<addr_list.size(); j++)
            {
                OP_T& op  = window[s_tail];
                op.status = status;
                op.type = type;
                op.comp = flops;
                op.addr = addr_list[j];
                op.mem_ptr   = mem_ptr;
                op.node_id   = node_id;
                op.step      = local_step;
                op.global_step=global_step;
                op.mark      = false;
                
                s_tail ++;
                win_load ++;
                s_tail = (s_tail==ib_depth ? 0 : s_tail);
                
            }//end of all addresses from get_addr
            
            s_tail  = ( win_head+ win_load-1)%ib_depth;
            window[s_tail].mark = true;

#ifdef DEBUG_CORE
            DB("Tid %d: group_id=%d, Node Id=%d, fetched_global_step=%d, local_step=%d, ib_load= ",
                  tid, group_id, node_id, fetched_global_step,local_step, win_load);
#endif
            
        }
        
        //Start stepping forward to the next node
        nodegroup_list[group_id].curr_group_step ++;
        if(  nodegroup_list[group_id].curr_group_step == group.group_steps ){
            
             curr_group_id ++;
            
            if(  curr_group_id ==  nodegroup_size ){
                 curr_group_id = 0;
                 fetched_global_step ++;
            }
            
             nodegroup_list[curr_group_id].curr_group_step = 0;
        }
        //End stepping forward to the next node
    }
}


void Context::fetch_global_step_warmup(){
    
    assert( fetched_global_step<global_control_max );
    assert( curr_group_id==0 );
    
    bool done = false;
    size_t nodegroup_size = nodegroup_list.size();
    
    while( !done )
    {
        GRPCTRL_T  &group = nodegroup_list[curr_group_id];
        size_t local_step = fetched_global_step*group.group_steps + group.curr_group_step;
        
        for (size_t node_id = group.st_node; node_id <= group.end_node; node_id++)
        {
            DataNode &node  = datanode_list[node_id];
            vector<ADDR_T> addr_list = node.gen_addr(local_step);
            const OpType type = node.get_op();
            for (auto addr : addr_list)
                l1->lookup(addr, type);
        }
        
        //Start stepping forward to the next node
        group.curr_group_step ++;
        if(  group.curr_group_step == group.group_steps ){
            
            group.curr_group_step = 0;
            curr_group_id ++;
            
            if( curr_group_id == nodegroup_size ){
                curr_group_id = 0;
                fetched_global_step ++;
                done = true;
            }
        }
        //End stepping forward to the next node
    }
    
    assert( curr_group_id==0 );
    
#ifdef false
    DB("Tid %d::fetched_global_step=%d", tid, fetched_global_step);
#endif
}


bool Context::warmup(){
    
    if(fetched_global_step<global_control_max){
        fetch_global_step_warmup();
        if(fetched_global_step==global_control_max) return true;
    }
    return false;
}


void Context::update_dependency(OP_T &op){
    
    DataNode node   = datanode_list[op.node_id];
    GRPCTRL_T group = nodegroup_list[node.group_id];
    bool isReady   = true;
    for(auto dep_node_id : node.prev){
        DataNode dep_node=  datanode_list[dep_node_id];
        bool isSameGroup = (dep_node_id>=group.st_node && dep_node_id<=group.end_node);
        bool hasGlobalDep= (dep_node.retired_global_step >= op.global_step);
        hasGlobalDep = hasGlobalDep && (dep_node.retired_global_step>=0);
        bool hasGroupDep = (dep_node.retired_step >= op.step);
        hasGroupDep = hasGroupDep && (dep_node.retired_step>=0);
        isReady = (isSameGroup ?hasGroupDep : hasGlobalDep);
        
        if(!isReady) break;
    }
    op.status = (isReady ? ready : pending);
    
}


void Context::update_loop(){
    
    size_t rob_load = (win_load<rob_depth ? win_load : rob_depth);
    size_t s_head   =  win_head;
    
    while( rob_load>0 ){
        
        OP_T &op = window[s_head];
        
        //Check dependency
        if( op.status == pending){
            update_dependency( op );
        }
        
        //Dispatch ready ops
        if( op.status == ready ){
            dispatch( window[s_head] );
            
#ifdef DEBUG_CORE
            DB("Dispatch Tid%d:: wondow[%d] node_id=%d, addr[0x%lx], comp=%d, status=%d, step=",
                 tid, s_head, op.node_id, op.addr, op.comp, op.status, op.step);
#endif
            
        }
        //End of ready ops
        
        int remaining_unit = (avail_alu + avail_ld + avail_st);
        bool hasUnit = (remaining_unit>0) ?true : false;
        
        s_head ++;
        s_head = (s_head==ib_depth ? 0 : s_head);
        rob_load--;
    }
    //End of all ops
    
}






