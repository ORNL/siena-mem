#include "ar_dnode.h"
#include "ar_execblock.h"
#include "ar_core.h"
#include "ar_memobj.h"
#include "ar_baseobj.h"
#include "ar_memsim.h"
#include "ar_context.h"
#include "ar_cache.h"

MemSim::MemSim(const std::string &_simulator_name,
       const std::string &_mem_spec,
       const uint64_t _access_granularity,
       const bool _isMainMemory)
:
simulator_name(_simulator_name),
mem_spec(_mem_spec),
isMainMemory(_isMainMemory),
access_granularity(_access_granularity),
lineshift(FloorLog2(access_granularity))
{
    read_latency=0;
    write_latency=0;
    mem_frequency=0;
    capacity=0;
    isSimulatorInit = false;
    
    mem_clock = 0;
    total_read_latencies_cpu_cycle=0;
    //total_read_latencies_mem_cycle=0;
    total_write_latencies_cpu_cycle=0;
    //total_write_latencies_mem_cycle=0;
    num_read=0;
    num_write=0;
    
    cpu_clock = 0;
    //next_event= UINT64_MAX;
}

void MemSim::reset(){
    mem_clock = 0;
    total_read_latencies_cpu_cycle=0;
    //total_read_latencies_mem_cycle=0;
    total_write_latencies_cpu_cycle=0;
    //total_write_latencies_mem_cycle=0;
    num_read=0;
    num_write=0;
    
    cpu_clock = 0;
    //next_event= UINT64_MAX;
}


void MemSim::print(){
    
    INFO("\t%s::\n\tisEnabled = %u,\n\tspec=%s,\n\tcapacity=%lu,\n\tread_latency=%u,\n\twrite_latency=%u,\n\tfrequency=%lu",
         simulator_name.c_str(),
         isSimulatorInit,
         mem_spec.c_str(),
         capacity,
         read_latency,
         write_latency,
         mem_frequency);
}


void MemSim::print_stats(){
    
    cout << simulator_name <<"::"
         << "\n\tnum_loads=" << num_read
         << "\n\tnum_stores="<< num_write
	 << "\n\tAvg.lat=" << (total_read_latencies_cpu_cycle*1.0/num_read)
      //<< "\n\tAvg.lat.mem.cycles=" << (total_read_latencies_mem_cycle*1.0/num_read)
      //<< "\n\tAvg.lat.cpu.cycles=" << (total_write_latencies_cpu_cycle*1.0/num_write)
      //<< "\n\tAvg.lat.mem.cycles=" << (total_write_latencies_mem_cycle*1.0/num_write)
    << endl;
    
}


void MemSim::process_read_requests(ADDR_T addr){
    
    std::vector<uint64_t>& rlist = dispatch_queue_read[addr];
#ifdef DEBUG_MEMSIM
    DB("%s::pending read requests at %lu",simulator_name.c_str(), cpu_clock);
    for (auto req : dispatch_queue_read) {
        size_t i=0;
        for (auto t : req.second)
            DB("dispatch_queue_read[0x%lx][%u]=%lu", req.first, i++, t);
    }
#endif
    assert(rlist.size()>0);
    
    //count responses from mem
    total_read_latencies_cpu_cycle += (cpu_clock - rlist[0]);
#ifdef DEBUG_MEMSIM
    DB("%s::addr[0x%lx]:send=%lu, receive=%lu, latency_cpu_cycle=%lu",
       simulator_name.c_str(), addr, rlist[0], cpu_clock, (cpu_clock - rlist[0]));
#endif

    if(rlist.size()==1){
        dispatch_queue_read.erase(addr);
    }else{
        rlist.erase( rlist.begin() );
    }
        
    baseobj->start_complete(cpu_clock, addr);
    
}


void MemSim::process_write_requests(ADDR_T addr){
    
    
}


void MemSim::sync_core(size_t core_frequency){
    
    size_t max_div = gcd(core_frequency, mem_frequency);
    cpu_ticks = core_frequency/max_div;
    mem_ticks = mem_frequency/max_div;
        
#ifdef DEBUG_MEMSIM
    DB("%s::cpu_tick=%lu, mem_tick=%lu",
       simulator_name.c_str(),cpu_ticks,mem_ticks);
#endif

}


