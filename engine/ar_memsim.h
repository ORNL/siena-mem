#ifndef AR_MEMSIM_H
#define AR_MEMSIM_H

#include "ar_typedef.h"
#include <iostream>

class Engine;
class BaseObj;

class MemSim
{
protected:
    static size_t        num_instances;
    const std::string    simulator_name;
    const std::string    mem_spec;
    const bool           isMainMemory;
    const uint64_t       access_granularity;
    const uint64_t       lineshift;
    
    uint64_t    read_latency;
    uint64_t    write_latency;
    uint64_t    mem_frequency;
    uint64_t    mem_ticks,cpu_ticks;
    uint64_t    capacity;
    
    BaseObj*    baseobj;
    bool        isSimulatorInit;
    
    //timing info
    //uint64_t    next_event;
    uint64_t    mem_clock;
    uint64_t    cpu_clock;
    
    //for stats
    std::unordered_map<ADDR_T, std::vector<uint64_t>> dispatch_queue_read;
    std::unordered_map<ADDR_T, std::vector<uint64_t>> dispatch_queue_write;
    uint64_t total_read_latencies_cpu_cycle;
    //uint64_t total_read_latencies_mem_cycle;
    uint64_t total_write_latencies_cpu_cycle;
    //uint64_t total_write_latencies_mem_cycle;
    uint64_t num_read;
    uint64_t num_write;
    
public:
    MemSim(const std::string &_simulator_name,
           const std::string &_mem_spec,
           const uint64_t _access_granularity,
           const bool _isMainMemory);
    
    ~MemSim(){}
    

    virtual void init_simulator(BaseObj* _baseobj)=0;
    virtual void finish_simulator()=0;
    virtual void clock()=0;
    virtual void dump_state()=0;
    virtual void advance(uint64_t t)=0;
    virtual void load(uint64_t dispatch, ADDR_T addr)=0;
    virtual void store(uint64_t dispatch, ADDR_T addr)=0;
    
    
    //common functions
    void reset();
    void print();
    void print_stats();
    
    
    //getter and setter
    void set_capacity(uint64_t _in){ capacity=_in; }
    
    void set_read_latency(uint64_t _in){read_latency=_in;}
    
    void set_write_latency(uint64_t _in){write_latency=_in;}
    
    void set_mem_frequency(uint64_t _in){mem_frequency=_in;}
    
    uint64_t get_capacity()const{ return capacity; }
    
    uint64_t get_mem_clock()const{ return mem_clock;};
    
    uint64_t get_access_granularity()const{return access_granularity;}
    
    uint64_t get_lineshift()const{return lineshift;}
    
    uint64_t get_mem_frequency()const{return mem_frequency;}
    
    bool get_isMainMemory()const{return isMainMemory;}
    
    std::string get_mem_spec()const{return mem_spec;}
    
    virtual uint64_t get_next_event()=0;
    
    bool is_enabled()const{return isSimulatorInit; };
    
    void sync_core(size_t core_frequency);
    
protected:
    
    void process_read_requests(ADDR_T addr);
    
    void process_read_requests_bk(ADDR_T addr);
    
    void process_write_requests(ADDR_T addr);
    
};

#endif
