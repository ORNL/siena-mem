#include "ar_baseobj.h"
#include "ar_cacheobj.h"
#include "ar_memobj.h"
#include "ar_core.h"
#include "ar_context.h"

//constructor
Core::Core(CoreGroup &cg){
    rob_depth = cg.rob_depth;
    lfb_depth = cg.lfb_depth;
    frequency = cg.frequency;
    ipc = cg.ipc;
    alu = cg.alu;
    ld  = cg.ld;
    st  = cg.st;
    l1  = nullptr;
}

void Core::connect_l1(CacheObj* _l1){
    l1 = _l1;
    l1->set_is_FLC();
}

//constructor
CoreGroup::CoreGroup(std::string _type, size_t num_cores)
:core_type(_type){
    
    //setup default parameters
    rob_depth = DEFAULT_INS_WIN;
    lfb_depth = DEFAULT_PEND_BUF;
    frequency = DEFAULT_FREQ;
    alu       = DEFAULT_AR;
    ld        = DEFAULT_LD;
    st        = DEFAULT_ST;
    ipc       = alu+ld+st;
    cycles_prev=0;
    
    for (size_t i=0; i<num_cores; i++) {
        core_list.emplace_back(*this);
    }
}

void CoreGroup::connect_cache(std::vector<CacheObj> &cache_list){
    
    size_t num_caches = cache_list.size();
    size_t num_cores  = core_list.size();
    assert(num_cores%num_caches==0);
    size_t stride = num_cores/num_caches;
    
    size_t core_id = 0;
    for (auto &core : core_list) {
        core.connect_l1(&(cache_list[core_id/stride]));
        core_id ++;
    }
    
}

void CoreGroup::connect_mem(std::vector<MemObj> &mem_list){
    TODO();
}

void CoreGroup::print(){
    
    INFO("%s[%d]::\n\tfrequency = %lu,\n\tMax. %u instructions per window,\n\tMax. %u pending cache misses,\n\tIssue %u instructions per cycle (including %u arithmetic, %u load, and %u store)",
         core_type.c_str(),
         core_list.size(),
         frequency,
         rob_depth,
         lfb_depth,
         ipc,
         alu,
         ld,
         st);
}


void CoreGroup::print_connection(){
    
    size_t core_id=0;
    for (auto &core : core_list) {
        stringstream ss;
        ss<< "\t"<<core_type << "["<<(core_id++)<<"]";
        if(core.l1){
            ss << " --> " << (core.l1)->get_connection_str();
        }
        string s = ss.str();
        INFO("%s", s.c_str());
    }
}


void CoreGroup::record_start(){
    active_periods.push_back(cycles_prev);
}

void CoreGroup::record_end(uint64_t elapsed_cycles){
    cycles_prev += elapsed_cycles;
    active_periods.push_back(cycles_prev);
}

void CoreGroup::link_core_ctx(std::vector<Context> &ctx_group){
    
    size_t num_ctx   = ctx_group.size();
    size_t num_cores = core_list.size();
    
    size_t core_id = 0;
    for (size_t tid=0; tid<num_ctx; tid++) {
        assert( core_list[core_id].l1 );
        ctx_group[tid].connect_core(core_list[core_id]);
        core_id ++;
        if(core_id >= num_cores)
            core_id = 0;
    }
    
}

