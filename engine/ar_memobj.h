#ifndef AR_MEMOBJ_H
#define AR_MEMOBJ_H

#include "ar_baseobj.h"
#include "ar_memalloc.h"

class MemObj : public BaseObj{

private:

    std::unordered_map<int,MemAlloc>    memobj_map;
    uint64_t                            avail_capacity;
    
    //function pointer
    //REQ_T (MemObj::*_loadfunc_ptr)(REQ_T);
    //REQ_T (MemObj::*_storefunc_ptr)(REQ_T);
    //void  (MemObj::*_advancefunc_ptr)(uint64_t,bool);

public:
    MemObj(std::string _name, uint64_t _capacity, uint64_t _access_granularity);

    ~MemObj(){}

    void start_complete(uint64_t t, uint64_t addr_st);
    void set_simulator(MemSim* _simulator);
    
    //overloading
    void print()const;
    void print_stats()const;
    uint64_t load(REQ_T req);
    uint64_t store(REQ_T req);
    
    bool place_alloc(const MemAlloc& _memalloc);
    bool remove_alloc(size_t oid);
    
    size_t get_num_memobj()const;
    
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
