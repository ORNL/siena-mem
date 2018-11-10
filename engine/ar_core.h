#ifndef AR_COREGROUP_H
#define AR_COREGROUP_H


#define DEFAULT_INS_WIN  128     //instruction window
#define DEFAULT_PEND_BUF  10     //pending cache misses
#define DEFAULT_FREQ 2000000000  //core frequency
#define DEFAULT_IR   6           //no. of instructions per cycle
#define DEFAULT_AR   3           //no. of arithmetic instructions per cycle
#define DEFAULT_LD   2           //no. of load instructions per cycle
#define DEFAULT_ST   1           //no. of store instructions per cycle

#include "ar_typedef.h"
class CoreGroup;
class CacheObj;
class MemObj;
class Context;

class Core{

    friend class CoreGroup;
    
private:
    //relavant architectural parameters
    size_t   rob_depth;
    size_t   lfb_depth;
    size_t   frequency;
    size_t      ipc;
    size_t      alu;
    size_t       ld;
    size_t       st;
    
    CacheObj*    l1;
    
    //uint64_t cycles;
    //OP_T*    window;

public:
    Core(CoreGroup &cg);
    CacheObj* get_l1_ptr(){return l1;}
    
private:
    
    void connect_l1(CacheObj* _l1);
    
};


class  CoreGroup{
    
    friend class Core;
    
private:
    size_t rob_depth;
    size_t lfb_depth;
    size_t frequency;
    size_t       ipc;
    size_t       alu;
    size_t        ld;
    size_t        st;
    std::vector<uint64_t> active_periods;
    uint64_t                 cycles_prev;
    
    const std::string    core_type;
    std::vector<Core>    core_list;
    
public:
    
    //constructor
    CoreGroup(std::string _type, size_t num_cores);
    
    //destructor
    ~CoreGroup(){}
    
    void print();
    void print_connection();
    void record_start();
    void record_end(uint64_t elapsed_cycles);
    void link_core_ctx(std::vector<Context> &ctx_group);
    void connect_cache(std::vector<CacheObj> &cache_list);
    void connect_mem(std::vector<MemObj> &mem_list);
    
    std::vector<uint64_t> get_periods(){return active_periods;}
    
    size_t get_rob()const{return rob_depth;}
        
    size_t get_ipc()const{return ipc;}
    
    size_t get_lfb()const{return lfb_depth;}
    
    size_t get_frequency()const{return frequency;}
    
    size_t get_alu()const{return alu;}
    
    size_t get_ld()const{return ld;}
    
    size_t get_st()const{return st;}
    
    void set_rob(size_t _rob_depth) {
        rob_depth = _rob_depth;
    }
    
    void set_lfb(size_t _lfb_depth) {lfb_depth =_lfb_depth;}
    
    void set_frequency(size_t _frequency) {frequency=_frequency;}
    
    void set_alu(size_t _alu) {
        alu=_alu;
        
        //update derived parameters
        ipc = alu+ld+st;
    }
    
    void set_ld(size_t _ld) {
        ld=_ld;
    
        //update derived parameters
        ipc = alu+ld+st;
    }
    
    void set_st(size_t _st) {
        st=_st;
    
        //update derived parameters
        ipc = alu+ld+st;
    }

};

#endif
