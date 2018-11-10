
#include "ar_memalloc.h"
#include "ar_dnode.h"
#include "ar_execblock.h"
#include "ar_memobj.h"
#include "ar_context.h"
#include "ar_core.h"
#include "ar_engine.h"

using namespace std;

MemSim* Engine::find_memsim(ADDR_T addr){
    
    //no need to search if only one memory exists
    if(!has_hms) return default_memsim_ptr;
    
    for (auto kv : memsim_alloc_map) {
        std::vector<ALLOC_T>& ranges = kv.second;
        for (auto alloc : ranges) {
            if(alloc.st<=addr && addr<alloc.end){
                return kv.first;
            }
        }
    }
    PANIC("addr=0x%lx not allocated",addr);
    return nullptr;
}


MemObj* Engine::find_memobj(ADDR_T addr){
    
    //no need to search if only one memory exists
    if(!has_hms) return default_mem_ptr;
    
    for (auto kv : mem_alloc_map) {
        std::vector<ALLOC_T>& ranges = kv.second;
        for (auto alloc : ranges) {
            if(alloc.st<=addr && addr<alloc.end){
                return kv.first;
            }
        }
    }
    PANIC("addr=0x%lx not allocated",addr);
    return nullptr;
}

void Engine::print_final_stats(){
    
    double total_mb = 0.0;
    
    for(unordered_map<string,MemAlloc>::iterator it=memobj_map.begin();it!=memobj_map.end();++it){
        total_mb += (it->second).size/1000000.0;
    }
    
    cout << HLINE<<endl;
    
    cout << "Total allocation(MB): "<<total_mb<<endl;
    
    for (unordered_map<string, CoreGroup>::iterator it=machine_cores.begin();it!=machine_cores.end();++it) {
        cout <<it->first << ":: ";
        const vector<uint64_t>& periods = (it->second).get_periods();
        size_t num_periods = periods.size();
        for (size_t i=0; i<num_periods; i=i+2) {
            cout << "["<< periods[i]<< " - " <<periods[i+1]<<"] ";
        }
        cout << endl;
    }
    

    //cout << "Total core_cycles [excl. warmup]: "<<total_cycles<<endl;
    //cout << "Total access(MB): "<<(total_bytes*1E-6)<<endl;
    //cout << "Avg.  BW(MB/s)    [excl. warmup]: "<<total_bytes/1048576.0/(total_cycles*1.0/core_frequency)<<endl;
    cout << HLINE<<endl;
    
}

void Engine::print_progress(){

    string curr_core_name = "";
    uint64_t curr_core_cycles = 0;
    
    INFO("Progress: core name %s, core cycles %lu", curr_core_name.c_str(), curr_core_cycles);

}



