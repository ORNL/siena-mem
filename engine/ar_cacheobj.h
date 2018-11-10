#ifndef AR_CACHEOBJ_H
#define AR_CACHEOBJ_H

#include "ar_typedef.h"
#include "ar_cache.h"
#include "ar_baseobj.h"

class Engine;

class CacheObj : public BaseObj{
    
private:
    
    CACHE_T*    cache_array;
    uint64_t    cache_associativity;
    Engine*     engine;

    unordered_map<uint64_t, vector<REQ_T>>  pendingCl_req_map;
    unordered_map<uint64_t, uint64_t>       pendingCl_delay_map;
    
    //function pointer
    //REQ_T (CacheObj::*_loadfunc_ptr)(REQ_T);
    //REQ_T (CacheObj::*_storefunc_ptr)(REQ_T);
    //void  (CacheObj::*_advancefunc_ptr)(uint64_t,bool);
    
    bool          has_memsim;
    
public:
    CacheObj(string _name, uint64_t _capacity, uint64_t _granularity, uint64_t _associativity, Engine *_engine);

    ~CacheObj(){}
    
    size_t  get_num_pending_cl()const{return pendingCl_req_map.size();}
    void    set_simulator(MemSim* _simulator);
    void    lookup(ADDR_T addr, OpType op);
    void    advance(uint64_t next_cpu_cycle);
    
    //overloading
    void print()const;
    void print_stats()const;
    void dump_state();
    uint64_t load(REQ_T req);
    uint64_t store(REQ_T req);
    void start_complete(uint64_t t, uint64_t addr_st);
    void prop_complete(REQ_T req);
    void set_has_memsim(bool _in){has_memsim = _in;}
    
    /*
private:
    REQ_T simple_load(REQ_T req);
    
    REQ_T simulate_load(REQ_T req);
    
    REQ_T simple_store(REQ_T req);
    
    REQ_T simulate_store(REQ_T req);

    void simple_advance(uint64_t t,bool log);
    
    void simulate_advance(uint64_t t,bool log);
    */
};


#endif
