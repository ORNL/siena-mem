
#include "ar_memsim.h"
#include "ar_memobj.h"
#include "ar_cacheobj.h"
#include "ar_baseobj.h"
#include "ar_cache.h"
#include "ar_dnode.h"
#include "ar_execblock.h"
#include "ar_core.h"
#include "ar_engine.h"

BaseObj::BaseObj(bool _isMainMemory, std::string _name, uint64_t _capacity, uint64_t _access_granularity)
:
name(_name),
oid(num_instances++),
isMainMemory(_isMainMemory),
capacity(_capacity),
access_granularity(_access_granularity),
lineshift(FloorLog2(access_granularity))
{
    if(isMainMemory){ memsys_str.append(name);memsys_str.append(" ");}
    
    //setup default values
    access_latency_load = 0;
    access_latency_store= 0;
    frequency = 0;
    
    next  = nullptr;
    isFLC = false;
    
    simulator = nullptr;
    
}


void BaseObj::init_simulator(){
    
    if(simulator){
        if(isMainMemory) INFO("\tMemory %s::",name.c_str());
        else INFO("\tCache %s::",name.c_str());
        
        simulator->init_simulator(this);
        simulator->print();
        cout<<endl;
    }
    
}


void BaseObj::fini_simulator(){
    if(simulator && simulator->is_enabled() ){
        simulator->finish_simulator();
    }
}


//accessor
void BaseObj::set_nextlevel(BaseObj* _next){
    next = _next;
    next->add_prevlevel(this);
}

void BaseObj::add_prevlevel(BaseObj* prev){
    CacheObj* p = dynamic_cast<CacheObj*>(prev);
    assert(p);
    prev_list.push_back(p);
}


void BaseObj::set_read_latency(uint64_t _in){
    access_latency_load = _in;
}


void BaseObj::set_write_latency(uint64_t _in){
    access_latency_store = _in;
}


void BaseObj::set_frequency(uint64_t _in){
    frequency = _in;
}


void BaseObj::sync_core(size_t core_frequency){
    
    if(simulator){
        simulator->sync_core(core_frequency);
    }
    
}


string BaseObj::get_connection_str(){
    stringstream ss;
    if(!next){
        if( !isMainMemory )
            ss << name << " --> {"<<memsys_str<<"}";
        else
            ss << name ;
    }
    else
        ss<< name << " --> "<<next->get_connection_str();
    
    return ss.str();
}






