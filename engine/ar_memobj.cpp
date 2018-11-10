
#include "ar_memalloc.h"
#include "ar_memsim.h"
#include "ar_cacheobj.h"
#include "ar_memobj.h"
#if SAVE_TRACE
#include <fstream>
#include "ar_context.h"
#endif


MemObj::MemObj(std::string _name, uint64_t _capacity, uint64_t _access_granularity)
:
BaseObj(true, _name, _capacity, _access_granularity){
    
    avail_capacity = _capacity;
    /*
    _loadfunc_ptr  = &MemObj::simple_load;
    _storefunc_ptr = &MemObj::simple_store;
    _advancefunc_ptr = &MemObj::simple_advance;
    */
}


size_t MemObj::get_num_memobj()const{ return memobj_map.size(); }


void MemObj::print()const{
    
    INFO("Memory %s::\n\tcapacity=%lu,\n\tgranularity=%lu,\n\tlatency_read=%u,\n\tlatency_write=%u,\n\tuse_simulator=%u",
         name.c_str(),
         capacity,
         access_granularity,
         access_latency_load,
         access_latency_store,
         use_simulator()
         );
 
    if( simulator )
        simulator->print();
}


void MemObj::print_stats()const{
    std::cout << HLINE<<std::endl;
    std::cout << name <<std::endl;
    if( simulator )
        simulator->print_stats();
}


void MemObj::set_simulator(MemSim* _simulator){

    simulator = _simulator;
    
    //_loadfunc_ptr  = &MemObj::simulate_load;
    //_storefunc_ptr = &MemObj::simulate_store;
    //_advancefunc_ptr = &MemObj::simulate_advance;
    
    simulator->set_read_latency(  access_latency_load );
    simulator->set_write_latency( access_latency_store );
    simulator->set_capacity(capacity);
    simulator->set_mem_frequency( frequency );
    
    assert(access_granularity==simulator->get_access_granularity());
    assert(lineshift==simulator->get_lineshift());
}


inline uint64_t MemObj::load(REQ_T req){
    
    ADDR_T addr = req.addr;
    addr = addr>>lineshift;
    addr = addr<<lineshift;
    
    //return (this->*_loadfunc_ptr)(req);
    
    uint64_t complete = 0UL;
    if(simulator){
        //aggregate transactions
        if(load_queue.find(addr)==load_queue.end())
            simulator->load(req.dispatch, addr);
        
        load_queue[addr].push_back(req);
    }
    else
    {
        complete = req.dispatch + access_latency_load;
    }
    
#if SAVE_TRACE
    std::stringstream ss;
    ss <<"t"<<(req.ctx_ptr)->get_tid()<<".trace";
    const char* fname=ss.str().c_str();
    std::ofstream memtrace(fname, std::ios_base::app);
    const char* type = "R";
    memtrace << req.dispatch << " " << type << " 0x" << std::hex << addr << std::dec << " " << complete << std::endl;
#endif

#ifdef DEBUG_MEMSIM
    DB("%s::[req.addr=0x%lx, load addr=0x%lx, req.complete=%lu]",
       name.c_str(), req.addr, addr, complete);
#endif
    
    return complete;
}


inline uint64_t MemObj::store(REQ_T req){
    
    ADDR_T addr = req.addr;
    addr = addr>>lineshift;
    addr = addr<<lineshift;
    
    uint64_t complete = 0UL;
    if(simulator){
        simulator->store(req.dispatch, addr);
        store_queue[addr].push_back(req);
    }
    else
    {
        complete = req.dispatch + access_latency_store;
    }

#if SAVE_TRACE
    std::stringstream ss;
    ss <<"t"<<(req.ctx_ptr)->get_tid()<<".trace";
    const char* fname=ss.str().c_str();
    std::ofstream memtrace(fname, std::ios_base::app);
    const char* type = "W";
    memtrace << req.dispatch << " " << type << " 0x" << std::hex << addr << std::dec << " " << complete << std::endl;
#endif
    
#ifdef DEBUG_MEMSIM
    DB("%s::[req.addr=0x%lx, load addr=0x%lx, req.complete=%lu]",
       name.c_str(), req.addr, addr, complete);
#endif
    
    return complete;
}


bool MemObj::place_alloc(const MemAlloc& _alloc){
    
    if(avail_capacity < _alloc.size){
#ifdef VERBOSE
        INFO("Memory %s avail capacity %lu < _alloc.size %lu", name.c_str(), avail_capacity,_alloc.size);
#endif
        return false;
    }
    
    if(memobj_map.find(_alloc.mid)!=memobj_map.end())
        PANIC("Memory Object %s is already on Memory %s", _alloc.name.c_str(), name.c_str());
    
    memobj_map[_alloc.mid] = _alloc;
    avail_capacity -= _alloc.size;
    
    return true;
}


bool MemObj::remove_alloc(size_t oid){
    
    if(memobj_map.find(oid)==memobj_map.end())
        PANIC("Memory Object Id %i is not placed on Memory %s", oid, name.c_str());
    
    avail_capacity += memobj_map[oid].size;
    memobj_map.erase(oid);
    
#ifdef DEBUG
    std::cout << "Memory "<< name << " avail capacity %lu " << avail_capacity <<std::endl;
#endif
    
    return true;
}


void MemObj::start_complete(uint64_t t, uint64_t addr_st){
    
    vector<REQ_T>& rlist = load_queue[addr_st];
    
#ifdef DEBUG_MEMSIM
    DB("%s [0x%lx]", name.c_str(), addr_st);
    assert(rlist.size()>0);
#endif
    
    for (auto req : rlist) {
        req.complete = t;
        (req.prev_ptr)->prop_complete(req);
    }

    load_queue.erase(addr_st);
}




