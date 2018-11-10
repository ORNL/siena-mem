#include "ar_memsim.h"
#include "ar_cacheobj.h"
#include "ar_memobj.h"
#include "ar_core.h"
#include "ar_execblock.h"
#include "ar_dnode.h"
#include "ar_context.h"
#include "ar_engine.h"

CacheObj::CacheObj(string _name, uint64_t _capacity,
                   uint64_t _granularity, uint64_t _associativity, Engine *_engine)
:
BaseObj(false, _name, _capacity, _granularity){
    
    cache_associativity  = _associativity;
    engine = _engine;
    cache_array = new CACHE_T(_name, _capacity, _granularity, _associativity);
    assert( lineshift == cache_array->_lineShift );
    assert( access_granularity == (1<<lineshift) );
    
    //_loadfunc_ptr  = &CacheObj::simple_load;
    //_storefunc_ptr = &CacheObj::simple_store;
}


void CacheObj::print()const{
    
    INFO("\tcapacity=%lu,\n\tcacheline=%lu,\n\tassociativity=%lu,\n\tlatency_read=%u,\n\tlatency_write=%u,\n\tuse_simulator=%u",
         capacity,
         access_granularity,
         cache_associativity,
         access_latency_load,
         access_latency_store,
         use_simulator()
         );
    
    cache_array->print();
    
    if( simulator )
        simulator->print();
    
}


void CacheObj::print_stats()const{
    
    cout << HLINE<<endl;
    cout << cache_array->StatsLong();
    
    if( simulator )
        simulator->print_stats();
    
}


void CacheObj::set_simulator(MemSim* _simulator){

    simulator     = _simulator;
    
    //_loadfunc_ptr  = &CacheObj::simulate_load;
    //_storefunc_ptr = &CacheObj::simulate_store;
    //_advancefunc_ptr = &CacheObj::simulate_advance;
    
    simulator->set_read_latency(  access_latency_load );
    simulator->set_write_latency( access_latency_store );
    simulator->set_capacity(capacity);
    simulator->set_mem_frequency( frequency );
    
    assert(access_granularity==simulator->get_access_granularity());
    assert(lineshift==simulator->get_lineshift());
}


void CacheObj::advance(uint64_t next_cpu_cycle){
    
    if( simulator ) return;
    
    for (auto it=pendingCl_delay_map.begin(); it!=pendingCl_delay_map.end();) {
        if( 0 < it->second && it->second < next_cpu_cycle){
            pendingCl_req_map.erase(it->first);
            it=pendingCl_delay_map.erase(it);
        }
    }
}


uint64_t CacheObj::load(REQ_T req){
    
    ADDR_T addr = req.addr;
    addr = addr>>lineshift;
    addr = addr<<lineshift;
    
    uint64_t complete = 0UL;
    
    //pending for the next level to return
    if(pendingCl_delay_map.find(addr)!=pendingCl_delay_map.end())
    {
        cache_array->upd_cache_stats(CACHE_BASE::ACCESS_TYPE_LOAD, true);
        
        complete = pendingCl_delay_map[addr];
        if(  complete == 0UL)
        {
            pendingCl_req_map[addr].push_back(req);
        }else
        {
            complete  = (req.dispatch>complete ?req.dispatch : complete);
            complete += access_latency_load;
        }
#ifdef DEBUG_CACHESIM
        DB("%s Pending::[addr=0x%lx, complete = %lu]", name.c_str(), addr, req.complete);
#endif
        return complete;
    }
    
    CACHE_SET::CACHEBLOCK evict_block=cache_array->lookup_evict_update(addr,CACHE_BASE::ACCESS_TYPE_LOAD);
    
    //Hit
    if( evict_block.tag==1 ){
        if(simulator){
            simulator->load(req.dispatch, addr);
            load_queue[addr].push_back(req);
        }
        else{
            complete = req.dispatch + access_latency_load;
        }
        
#ifdef DEBUG_CACHESIM
        DB("%s HIT::[addr=0x%lx]", name.c_str(),addr);
#endif
        
        return complete;
    }
    //Miss
    else{
        
#ifdef DEBUG_CACHESIM
        DB("%s MISS::[addr=0x%lx]", name.c_str(),addr);
#endif
        REQ_T req_next = req;
        req_next.dispatch += (simulator ? 0 : access_latency_load);
        req_next.prev_ptr = this;
        req_next.addr = addr;
        
        if(next){
            complete = next->load(req_next);
        }else{
            complete = (req.mem_ptr)->load(req_next);
        }
        pendingCl_delay_map[addr] = complete;
        if(complete == 0UL){
            pendingCl_req_map[addr].push_back(req);
        }
        
        //wb
        if( evict_block.dirty==1 && evict_block.valid==1 ){
            if(next){
                next->store(req);
            }else{
	      //MemSim* mem_ptr = engine->find_memsim(evict_block.addr);
	      MemObj* mem_ptr = engine->find_memobj(evict_block.addr);
#ifdef DEBUG_CACHESIM
	      assert(mem_ptr);
	      DB("Evict addr=0x%lx to memory %s",mem_ptr->get_name());
#endif
	      REQ_T wb_req = {evict_block.addr};
	      wb_req.dispatch = req.dispatch;
	      wb_req.dispatch += (simulator ? 0 : access_latency_store);
	      wb_req.ctx_ptr = req.ctx_ptr;
	      mem_ptr->store(wb_req);
            }
        }
        
        return complete;
    }
}


void CacheObj::dump_state(){
#ifdef DUMP_STATE
    for (auto p : pendingCl_delay_map) {
        DB("%s pendingCl_delay_map[0x%lx]",name.c_str(),p.first);
    }
    for (auto p : pendingCl_req_map) {
        DB("%s pendingCl_req_map[0x%lx].size=%d",name.c_str(),p.first,p.second.size());
    }
    for (auto p : load_queue) {
        DB("%s load_queue[0x%lx].size=%d",name.c_str(),p.first,p.second.size());
    }
#endif
}


uint64_t CacheObj::store(REQ_T req){
    
    uint64_t complete=0;
    
    //return (this->*_storefunc_ptr)(req);
    
    CACHE_SET::CACHEBLOCK evict_block = cache_array->lookup_evict_update(req.addr,CACHE_BASE::ACCESS_TYPE_STORE);
    
    if(simulator){
        ADDR_T addr = req.addr;
        addr = addr>>lineshift;
        addr = addr<<lineshift;
        simulator->store(req.dispatch, addr);
    }
    else{
        complete = req.dispatch + access_latency_store;
    }
    
    if( evict_block.dirty==1 && evict_block.valid==1 ){
        if(next){
            next->store(req);
        }else{
	      //MemSim* mem_ptr = engine->find_memsim(evict_block.addr);
	      MemObj* mem_ptr = engine->find_memobj(evict_block.addr);
#ifdef DEBUG_CACHESIM
	      assert(mem_ptr);
	      DB("Evict addr=0x%lx to memory %s",mem_ptr->get_name());
#endif
	      REQ_T wb_req = {evict_block.addr};
	      wb_req.dispatch = req.dispatch;
	      wb_req.dispatch += (simulator ? 0 : access_latency_store);
	      wb_req.ctx_ptr = req.ctx_ptr;
	      mem_ptr->store(wb_req);
        }
    }
    return complete;
}

/*
void CacheObj::prop_complete(uint64_t t, uint64_t addr_st, uint64_t addr_end){
    
    unordered_map<uint64_t,uint64_t>::iterator it = pendingCl_delay_map.begin();
    
    for (;it!=pendingCl_delay_map.end();) {
        ADDR_T addr = it->first;
        if( addr_st<=addr && addr<addr_end){
            assert(pendingCl_req_map.find(addr)!=pendingCl_req_map.end());
            if(isFLC){
                vector<REQ_T>& rlist = pendingCl_req_map[addr];
                for (auto req : rlist) {
#ifdef DEBUG_CACHESIM
                    DB("%s complete [%d][%d]", name.c_str(), req.thread_id, req.pos);
#endif
                    req.complete = t;
                    req.ctx_ptr->complete_op(req);
                }
            }
            pendingCl_req_map.erase(addr);
            it = pendingCl_delay_map.erase(it);
        }else
            ++it;
    }
    
#ifdef DEBUG_CACHESIM
    for (auto p : pendingCl_delay_map) {
        DB("%s pendingCl_delay_map[0x%lx]",name.c_str(),p.first);
    }
    for (auto p : pendingCl_req_map) {
        DB("%s pendingCl_req_map[0x%lx].size=%d",name.c_str(),p.first,p.second.size());
    }
#endif
    
    for (auto &prev : prev_list) {
        prev->prop_complete(t, addr_st, addr_end);
    }    
}
*/

void CacheObj::prop_complete(REQ_T req){
    
    auto it = pendingCl_delay_map.find(req.addr);
    assert( it!=pendingCl_delay_map.end() );
    pendingCl_delay_map.erase(it);
    
    vector<REQ_T>& rlist= pendingCl_req_map[req.addr];
    assert( rlist.size()>0 );
    
    for (auto r : rlist) {
        r.complete = req.complete;
        
        if(isFLC){
#ifdef DEBUG_CACHESIM
            DB("%s complete [%d][%d]", name.c_str(), (r.ctx_ptr)->get_tid(), r.pos);
#endif
            r.ctx_ptr->complete_op(r);
        }else{
            (r.prev_ptr)->prop_complete(r);
        }
    }
    pendingCl_req_map.erase(req.addr);
    
}


void CacheObj::start_complete(uint64_t t, uint64_t addr_st){

    vector<REQ_T>& rlist = load_queue[addr_st];
    
#ifdef DEBUG_CACHESIM
    DB("%s [0x%lx - 0x%lx]", name.c_str(), addr_st);
    assert(!next);
    assert(rlist.size()>0);
#endif
    
    REQ_T req = rlist[0];
    req.complete = t;
    if(isFLC){
        
#ifdef DEBUG_CACHESIM
        DB("%s complete [%d][%d]", name.c_str(), (req.ctx_ptr)->get_tid(), req.pos);
#endif
        req.ctx_ptr->complete_op(req);
    }else{
        (req.prev_ptr)->prop_complete(req);
    }
    
    if(rlist.size()==1){
        load_queue.erase(addr_st);
    }else{
        rlist.erase(rlist.begin());
    }
    
}


void CacheObj::lookup(ADDR_T addr, OpType op){
    
    CACHE_SET::CACHEBLOCK evict_block;
    switch (op) {
        case LOAD:
            evict_block=cache_array->lookup_evict_update(addr,CACHE_BASE::ACCESS_TYPE_LOAD);
            break;
        case STORE:
            evict_block=cache_array->lookup_evict_update(addr,CACHE_BASE::ACCESS_TYPE_STORE);
            break;
        default:
            PANIC("Invalid op type %d",op);
            break;
    }
    if( evict_block.tag!=1 && next)
        ((CacheObj*)next)->lookup(addr, op);
}

