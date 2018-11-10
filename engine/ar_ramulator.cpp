#include "ar_ramulator.h"
#include "ar_memobj.h"
#include "ar_core.h"
#include "ar_dnode.h"
#include "ar_execblock.h"


using namespace ramulator;

static map<string, function<MemoryBase *(const Config&, int)> > name_to_func = {
    {"DDR3", &MemoryFactory<DDR3>::create},
    {"DDR4", &MemoryFactory<DDR4>::create},
    {"LPDDR3", &MemoryFactory<LPDDR3>::create},
    {"LPDDR4", &MemoryFactory<LPDDR4>::create},
    {"GDDR5", &MemoryFactory<GDDR5>::create},
    {"WideIO", &MemoryFactory<WideIO>::create},
    {"WideIO2", &MemoryFactory<WideIO2>::create},
    {"HBM", &MemoryFactory<HBM>::create},
    {"SALP-1", &MemoryFactory<SALP>::create},
    {"SALP-2", &MemoryFactory<SALP>::create},
    {"SALP-MASA", &MemoryFactory<SALP>::create},
};



RamulatorSim::RamulatorSim(bool _isMainMemory, const std::string& _configs_file, const std::string& _outputfile)
:
MemSim(RamulatorSim_name,_configs_file,64,_isMainMemory),
configs_file(_configs_file),
cacheline(64),
outputfile(_outputfile),
read_cb_func(std::bind(&RamulatorSim::read_complete,   this, std::placeholders::_1)),
write_cb_func(std::bind(&RamulatorSim::write_complete, this, std::placeholders::_1))
{
    assert(1<<lineshift==64);
}


RamulatorSim::RamulatorSim(MemSim* _in, const std::string& _outputfile)
:
MemSim(RamulatorSim_name,_in->get_mem_spec(),64,_in->get_isMainMemory()),
configs_file(_in->get_mem_spec()),
cacheline(64),
outputfile(_outputfile),
read_cb_func(std::bind(&RamulatorSim::read_complete,   this, std::placeholders::_1)),
write_cb_func(std::bind(&RamulatorSim::write_complete, this, std::placeholders::_1))
{
    assert(1<<lineshift==64);
}


void RamulatorSim::init_simulator(BaseObj* _baseobj)
{
    if(isSimulatorInit)
        PANIC("%s has been initialized before", simulator_name.c_str());
    
    assert(mem_frequency>0);
    baseobj= _baseobj;
    isSimulatorInit = true;
        
    Config configs(configs_file);
    configs.add("trace_type", "DRAM");
    configs.set_core_num(1);
    
    num_read = 0;
    num_write= 0;
    total_read_latencies_cpu_cycle = 0;
    //total_read_latencies_mem_cycle = 0;
    //TODO: fix it
    //Stats::statlist.output(outputfile);
    
    const string& standard = configs["standard"];
    assert(name_to_func.find(standard) != name_to_func.end() && "unrecognized standard name");
    mem = name_to_func[standard](configs, cacheline);
    assert(mem);
}


void RamulatorSim::finish_simulator()
{
    mem->finish();
    //MemSim::print_stats();
}


void RamulatorSim::load(uint64_t dispatch, ADDR_T addr)
{
    //addr = addr>>6;
    //addr = addr<<6;
    num_read ++;
    
    Request r( (long)addr, Request::Type::READ , read_cb_func);
    pending_ld_map[dispatch].push_back(r);

}

void RamulatorSim::try_insert_st(){

  for(auto it = pending_st_map.begin(); it!= pending_st_map.end();)
    {
      if(it->first > cpu_clock) break;
      std::vector<ramulator::Request>& req_list = it->second;
      for (auto it2=req_list.begin(); it2!=req_list.end(); )
	{
	  bool success = mem->send(*it2);
	  if(success){
#ifdef DEBUG_MEMSIM
	    DB("insert addr[0x%lx] at mem_clock=%lu, success",(*it2).addr,mem_clock);
#endif
	    dispatch_queue_write[(*it2).addr].push_back(cpu_clock);
	    it2=req_list.erase(it2);
	  }
	  else {
#ifdef DEBUG_MEMSIM
	    DB("insert addr[0x%lx] at mem_clock=%lu, reject",(*it2).addr,mem_clock);
#endif
	    break;
	  }
	}
      if(req_list.size()==0)
	it = pending_st_map.erase(it);
      else
	break;
    }
  return;
}

void RamulatorSim::try_insert_ld(){
  
  for(auto it = pending_ld_map.begin(); it != pending_ld_map.end();)
    {
      if(it->first > cpu_clock) break;
      std::vector<ramulator::Request>& req_list = it->second;
      for (auto it2=req_list.begin(); it2!=req_list.end(); )
	{
          bool success = mem->send(*it2);

	  if(success){
	    dispatch_queue_read[(*it2).addr].push_back(cpu_clock);
#ifdef DEBUG_MEMSIM
	    DB("insert addr[0x%lx] at mem_clock=%lu, success, req_list.size=%u,dispatch_queue_read=%lu",
	       (*it2).addr,mem_clock,req_list.size(),dispatch_queue_read[(*it2).addr].back());
#endif                    

	    it2=req_list.erase(it2);
	  }else {
#ifdef DEBUG_MEMSIM
	    DB("insert addr[0x%lx] at mem_clock=%lu, reject, req_list.size=%u",
	       (*it2).addr,mem_clock,req_list.size());
#endif
	    break;
	  }
	}
      if(req_list.size()==0)
	it = pending_ld_map.erase(it);
      else
	break;
    }
  return;
}

void RamulatorSim::store(uint64_t dispatch, ADDR_T addr)
{
    //addr = addr>>6;
    //addr = addr<<6;
    num_write ++;

    Request r( (long)addr, Request::Type::WRITE , write_cb_func);
    pending_st_map[dispatch].push_back(r);
}


void RamulatorSim::advance(uint64_t next_cpu_cycle)
{
    //step tick by tick
    uint64_t tick_st  = (cpu_clock+cpu_ticks)/cpu_ticks;
    uint64_t tick_end = next_cpu_cycle/cpu_ticks;
    
#ifdef DEBUG_MEMSIM
    DB("cpu_clock=%lu, mem_clock=%lu, next_cpu_cycle=%lu, tick_st=%lu(%lu), tick_end=%lu(%lu)",
       cpu_clock, mem_clock, next_cpu_cycle, tick_st, tick_st*cpu_ticks, tick_end, tick_end*cpu_ticks);
#endif

    //update before tick to update timing info
    cpu_clock = tick_st * cpu_ticks;    
    for (uint64_t i=tick_st; i<=tick_end; i++) {
        
#ifdef DEBUG_MEMSIM
        DB("step to %lu, pending_ld_map.size=%u, pending_st_map.size=%u",
           cpu_clock, pending_ld_map.size(), pending_st_map.size());
#endif

	try_insert_st();
        
        try_insert_ld();
        
        //step memory simulator
        for(uint64_t n=0;n<mem_ticks;n++){
            mem_clock ++;
            //may trigger update to pending_ld_map
            mem->tick();
        }
        
        //step to the next tick
        cpu_clock += cpu_ticks;
    }

    cpu_clock = next_cpu_cycle;
    try_insert_st();
    try_insert_ld();
    
    //Stats::curTick++;

#ifdef DEBUG_MEMSIM
    dump_state();
#endif

}


uint64_t RamulatorSim::get_next_event()
{
    uint64_t t0 = (cpu_clock/cpu_ticks + 1)*cpu_ticks;
    uint64_t t1 = (pending_ld_map.size()>0 ? pending_ld_map.begin()->first : 0);
    uint64_t next_event  = (t0>t1 ?t0 :t1);
    
#ifdef DEBUG_MEMSIM
    DB("cpu_clock=%lu, mem_clock=%lu, next_event=%lu ",
       cpu_clock, mem_clock, next_event );
#endif
    
    return next_event;
}


void RamulatorSim::dump_state()
{
#ifdef DUMP_STATE    
    DB("cpu_clock=%lu, mem_clock=%lu, next_event=%lu ",
       cpu_clock, mem_clock, get_next_event() );
    
    for (auto l : pending_ld_map) {
        DB("pending_ld_map[%lu].size=%u",l.first,l.second.size());
        for (auto r : l.second) {
            DB("\taddr[0x%lx]",r.addr);
        }
    }
    for (auto l : pending_st_map) {
        DB("pending_st_map[%lu].size=%u",l.first,l.second.size());
        for (auto r : l.second) {
            DB("\taddr[0x%lx]",r.addr);
        }
    }
#endif
}


void RamulatorSim::clock()
{
#ifdef DEBUG_MEMSIM
    cout << "Ramulator::clock()"<<endl;
#endif
    mem->tick();
    mem_clock ++;
    //Stats::curTick++;
}


void RamulatorSim::read_complete(ramulator::Request& r)
{
    
    MemSim::process_read_requests(r.addr);

}


void RamulatorSim::write_complete(ramulator::Request& r)
{}






