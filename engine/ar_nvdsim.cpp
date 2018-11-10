#include "ar_nvdsim.h"
#include "ar_memobj.h"
#include "ar_core.h"
#include "ar_dnode.h"
#include "ar_execblock.h"


NVMSim::NVMSim(const std::string &_mem_name,const uint64_t _id,
           const bool _isMainMemory,
           const std::string &_deviceIniFilename,
           const std::string &_systemIniFilename,
           const std::string &_pwdString,
           const std::string &_traceFileName)
    :
    MemSim(NVDSim_name, _deviceIniFilename, 4096, _isMainMemory),
    deviceIniFilename(_deviceIniFilename),
    systemIniFilename(_systemIniFilename),
    pwdString(_pwdString),
    traceFileName(_traceFileName)
    {}

void NVMSim::init_simulator(BaseObj* _baseobj){
    
    if(isSimulatorInit)
        PANIC("this simulator %s has been initialized before", simulator_name.c_str());
    
    assert(mem_frequency>0);
    baseobj= _baseobj;
    isSimulatorInit = true;
    
    num_read = 0;
    num_write= 0;
    total_read_latencies_cpu_cycle = 0;
    
    mem = new NVDIMM(num_instances, deviceIniFilename, systemIniFilename,pwdString,traceFileName);
    assert(mem);
    
    //typedef CallbackBase<void,uint64_t,uint64_t,uint64_t,bool> Callback_t;
    Callback_t *rcb=new Callback<NVMSim, void,uint64_t,uint64_t,uint64_t, bool>(this,&NVMSim::read_complete);
    Callback_t *ccb=new Callback<NVMSim, void,uint64_t,uint64_t,uint64_t, bool>(this,&NVMSim::crit_complete);
    Callback_t *wcb=new Callback<NVMSim, void,uint64_t,uint64_t,uint64_t, bool>(this,&NVMSim::write_complete);
    Callback_v *pcb=new Callback<NVMSim, void,uint64_t,std::vector<std::vector<double>>,uint64_t, bool>(this,&NVMSim::power_complete);
    mem->RegisterCallbacks(rcb,ccb,wcb,pcb);
}



void NVMSim::load(uint64_t dispatch, ADDR_T addr)
{
    //ADDR_T addr=req.addr;
    //addr = addr>>12;
    //addr = addr<<12;
    num_read ++;
    
    FlashTransaction txn(DATA_READ,addr,(void *)0xdeadbeef);
    pending_ld_map[dispatch].push_back(txn);;
}


void NVMSim::store(uint64_t dispatch, ADDR_T addr)
{
    //ADDR_T addr = req.addr;
    //addr = addr>>12;
    //addr = addr<<12;
    num_write ++;
    
    FlashTransaction txn(DATA_WRITE, addr, (void *)0xdeadbeef);
    pending_st_map[dispatch].push_back(txn);;
}


uint64_t NVMSim::get_next_event()
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


void NVMSim::advance(uint64_t next_cpu_cycle)
{
    //step tick by tick
    uint64_t tick_st  = (cpu_clock+cpu_ticks)/cpu_ticks;
    uint64_t tick_end = next_cpu_cycle/cpu_ticks;
    
#ifdef DEBUG_MEMSIM
    DB("cpu_clock=%lu, mem_clock=%lu, tick_st=%lu(%lu), tick_end=%lu(%lu)",
       cpu_clock, mem_clock, tick_st, tick_st*cpu_ticks, tick_end, tick_end*cpu_ticks);
#endif
    
    
    //update before tick to update timing info
    cpu_clock = tick_st * cpu_ticks;
    
    for (uint64_t i=tick_st; i<=tick_end; i++) {
        
#ifdef DEBUG_MEMSIM
        DB("step to %lu, pending_ld_map.size=%u, pending_st_map.size=%u",
           cpu_clock, pending_ld_map.size(), pending_st_map.size());
#endif
        
        for(auto it = pending_st_map.begin(); it!= pending_st_map.end();)
        {
            if(it->first > cpu_clock) break;
            std::vector<FlashTransaction>& req_list = it->second;
            for (auto it2=req_list.begin(); it2!=req_list.end(); )
            {
                bool success = mem->add(*it2);
                if(success){
                    dispatch_queue_write[(*it2).address].push_back(cpu_clock);
                    it2=req_list.erase(it2);
#ifdef DEBUG_MEMSIM
                    DB("insert addr[0x%lx] at mem_clock=%lu, success",(*it2).address,mem_clock);
#endif
                }else {
#ifdef DEBUG_MEMSIM
                    DB("insert addr[0x%lx] at mem_clock=%lu, reject",(*it2).address,mem_clock);
#endif
                    break;
                }
            }
            if(req_list.size()==0)
                it = pending_st_map.erase(it);
            else
                break;
        }
        
        for(auto it = pending_ld_map.begin(); it != pending_ld_map.end();)
        {
            if(it->first > cpu_clock ) break;
            std::vector<FlashTransaction>& req_list = it->second;
            for (auto it2=req_list.begin(); it2!=req_list.end(); )
            {
                bool success = mem->add(*it2);
                if(success){
                    dispatch_queue_read[(*it2).address].push_back(cpu_clock);
                    it2=req_list.erase(it2);

#ifdef DEBUG_MEMSIM
                    DB("insert addr[0x%lx] at mem_clock=%lu, success",(*it2).address,mem_clock);
#endif

                }else {
#ifdef DEBUG_MEMSIM
                    DB("insert addr[0x%lx] at mem_clock=%lu, reject",(*it2).address,mem_clock);
#endif
                    break;
                }
            }
            if(req_list.size()==0)
                it = pending_ld_map.erase(it);
            else
                break;
        }
        
        //step memory simulator
        for(uint64_t n=0;n<mem_ticks;n++){
            mem_clock ++;
            //may trigger update to pending_ld_map
            mem->update();
        }
        
        //step to the next tick
        cpu_clock += cpu_ticks;
    }
    cpu_clock = next_cpu_cycle;
    
#ifdef DEBUG_MEMSIM
    dump_state();
#endif
    
}


/*
 bool NVMSim::insert(REQ_T req)
 {
 //At Page Granularity
 ADDR_T addr = req.addr;
 addr >>= 12;
 addr <<= 12;
 
 bool success = true;
 if(req.type==LOAD){
 if( MQ_read.find(addr)==MQ_read.end() ){
 FlashTransaction txn=FlashTransaction(DATA_READ,addr,(void *)0xdeadbeef);
 success = mem->add(txn);
 #ifdef DEBUG_MEMSIM
 cout << "NVMSim::insert_MC addr=" << addr << ", mem_clock="<<mem_clock<< ", success="<<success<< endl;
 #endif
 }
 if(success){
 req.issue_mem_cycle = mem_clock;
 req.issue_cpu_cycle = engine->cpu_cycles;
 MQ_read[addr].push_back(req);
 #ifdef DEBUG_MEMSIM
 cout << "NVMSim::buffer req.addr=" << req.addr << " to addr="<<addr<< ", MQ_read[addr].size="<<MQ_read[addr].size()<< endl;
 #endif
 }
 }else{
 FlashTransaction txn=FlashTransaction(DATA_WRITE,addr,(void *)0xdeadbeef);
 success = mem->add(txn);
 }
 return success;
 }
 
 */


void NVMSim::finish_simulator(){
 
    //MemSim::print_stats();
}


void NVMSim::read_complete(uint64_t id,uint64_t addr, uint64_t done_cycle, bool mapped)
{
    MemSim::process_read_requests(addr);
}
    
void NVMSim::write_complete(uint64_t id, uint64_t addr, uint64_t done_cycle,bool mapped)
{}

void NVMSim::crit_complete(uint64_t id, uint64_t addr, uint64_t done_cycle,bool mapped)
{}

void NVMSim::power_complete(uint64_t id, std::vector<std::vector<double>> data, uint64_t cycle, bool mapped){
  
    cout<<"[Callback] Power Data for cycle: "<<cycle<<endl;
	for(uint64_t i = 0; i < NUM_PACKAGES; i++){
	  for(uint64_t j = 0; j < data.size(); j++){
	    if(DEVICE_TYPE.compare("PCM") == 0){
	      if(j == 0){
              cout<<"    Package: "<<i<<" Idle Energy: "<<data[0][i]<<"\n";
	      }else if(j == 1){
              cout<<"    Package: "<<i<<" Access Energy: "<<data[1][i]<<"\n";
	      }
	      if(GARBAGE_COLLECT == 1){
              if(j == 2){
                  cout<<"    Package: "<<i<<" Erase Energy: "<<data[2][i]<<"\n";
              }else if(j == 3){
                  cout<<"    Package: "<<i<<" VPP Idle Energy: "<<data[3][i]<<"\n";
              }else if(j == 4){
                  cout<<"    Package: "<<i<<" VPP Access Energy: "<<data[4][i]<<"\n";
              }else if(j == 5){
                  cout<<"    Package: "<<i<<" VPP Erase Energy: "<<data[5][i]<<"\n";
              }
	      }else{
              if(j == 2){
                  cout<<"    Package: "<<i<<" VPP Idle Energy: "<<data[2][i]<<"\n";
              }else if(j == 3){
                  cout<<"    Package: "<<i<<" VPP Access Energy: "<<data[3][i]<<"\n";
              }
	      }
	    }else{
	      if(j == 0){
              cout<<"    Package: "<<i<<" Idle Energy: "<<data[0][i]<<"\n";
	      }else if(j == 1){
              cout<<"    Package: "<<i<<" Access Energy: "<<data[1][i]<<"\n";
	      }else if(j == 2){
              cout<<"    Package: "<<i<<" Erase Energy: "<<data[2][i]<<"\n";
	      }
	    }
	  }
	}
}


void NVMSim::clock(){
    mem->update();
    mem_clock ++;
}



void NVMSim::dump_state(){
#ifdef DUMP_STATE
    DB("cpu_clock=%lu, mem_clock=%lu, next_event=%lu ",
       cpu_clock, mem_clock, get_next_event() );
    
    for (auto l : pending_ld_map) {
        DB("pending_ld_map[%lu].size=%u",l.first,l.second.size());
        for (auto r : l.second) {
            DB("\taddr[0x%lx]",r.address);
        }
    }
    for (auto l : pending_st_map) {
        DB("pending_st_map[%lu].size=%u",l.first,l.second.size());
        for (auto r : l.second) {
            DB("\taddr[0x%lx]",r.address);
        }
    }
#endif
}



