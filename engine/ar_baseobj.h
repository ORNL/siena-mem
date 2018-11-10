#ifndef AR_BASEOBJ_H
#define AR_BASEOBJ_H

#include "ar_typedef.h"

class CacheObj;
class BaseObj
{
private:
    static size_t       num_instances;
    static std::string  memsys_str;

public:
    const std::string   name;
    
protected:

    const size_t        oid;
    const bool          isMainMemory;
    const uint64_t      capacity;
    
    //access related parameters
    const uint64_t      access_granularity;
    const uint64_t      lineshift;
    
    uint64_t            access_latency_load;
    uint64_t            access_latency_store;
    uint64_t            frequency;
    
    //hierarchical info
    BaseObj*            next;
    std::vector<CacheObj*> prev_list;
    bool                isFLC;
    
    //timing info
    uint64_t            time_next_event;
    
    //simulation details if not analytical
    MemSim*             simulator;
    std::unordered_map<ADDR_T, std::vector<REQ_T>> load_queue;
    std::unordered_map<ADDR_T, std::vector<REQ_T>> store_queue;
    
public:
    BaseObj(bool _isMainMemory, std::string _name,
            uint64_t _capacity, uint64_t _access_granularity);
    
    ~BaseObj(){}
    
    void sync_core(size_t core_frequency);
    
    virtual void print()const=0;
    
    virtual void print_stats()const=0;
    
    virtual uint64_t load(REQ_T req)=0;
    
    virtual uint64_t store(REQ_T req)=0;
    
    virtual void start_complete(uint64_t t, uint64_t addr_st)=0;
    
    virtual void set_simulator(MemSim* _simulator)=0;
    
    void init_simulator();
    
    void fini_simulator();
    
    //accessor
    void set_nextlevel(BaseObj* _next);
    void add_prevlevel(BaseObj* _prev);
    void set_read_latency(uint64_t _in);
    void set_write_latency(uint64_t _in);
    void set_frequency(uint64_t _in);
    void set_is_FLC(){isFLC=true;}
    
    std::string get_name()const{return name;}
    
    BaseObj* get_nextlevel(){return next;}
    
    std::string get_connection_str();
    
    bool use_simulator()const{return (simulator!=nullptr);}
    
    MemSim* get_simulator()const{return simulator;}
    
    uint64_t get_read_latency()const{return access_latency_load;}
    
    uint64_t get_write_latency()const{return access_latency_store;}
    
    bool is_LLC()const{return (next==nullptr);}
    bool is_FLC()const{return isFLC;}
    
};

#endif
