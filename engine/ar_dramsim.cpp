#include "ar_core.h"
#include "ar_dramsim.h"
#include "ar_memobj.h"
#include "ar_dnode.h"
#include "ar_execblock.h"


void DramSim::init_simulator(BaseObj* _baseobj){
    
    if(isSimulatorInit)
      PANIC("this simulator %s has been initialized before",simulator_name.c_str());
    
    assert(mem_frequency>0);
    isSimulatorInit = true;
    
    //set a default capacity if not set
    unsigned megsOfMemory = capacity/1048576;
    
    mem = new MultiChannelMemorySystem(deviceIniFilename, systemIniFilename,
                                       pwdString,traceFileName,megsOfMemory);
    assert(mem);
    
    Callback_t *rcb=new Callback<DramSim,void,unsigned,uint64_t,uint64_t>(this,&DramSim::read_complete);
    Callback_t *wcb=new Callback<DramSim,void,unsigned,uint64_t,uint64_t>(this,&DramSim::write_complete);
    
    mem->RegisterCallbacks(rcb,wcb,NULL);
    
    //set the frequency ratio to 1:1
    mem->setCPUClockSpeed(0);
}
    



void DramSim::load(uint64_t dispatch, ADDR_T addr)
{
    num_read ++;
    //unsigned throwAwayBits = THROW_AWAY_BITS;
    //req.addr >>= 6;
    //req.addr <<= 6;
    
    DRAMSim::Transaction txn(DRAMSim::DATA_READ, addr, NULL);
    pending_ld_map[dispatch].push_back(txn);
}


void DramSim::store(uint64_t dispatch, ADDR_T addr)
{
    num_write ++;
    //unsigned throwAwayBits = THROW_AWAY_BITS;
    //req.addr >>= 6;
    //req.addr <<= 6;
    
    DRAMSim::Transaction txn(DRAMSim::DATA_WRITE, addr, NULL);
    pending_st_map[dispatch].push_back(txn);;
}


void DramSim::advance(uint64_t next_cpu_cycle)
{
    //step tick by tick
    uint64_t tick_st  = (cpu_clock+cpu_ticks)/cpu_ticks;
    uint64_t tick_end = next_cpu_cycle/cpu_ticks;
    
#ifdef DEBUG_MEMSIM
    DB("cpu_clock=%lu, mem_clock=%lu, tick_st=%lu(%lu), tick_end=%lu(%lu)",
       cpu_clock, mem_clock, tick_st, tick_st*cpu_ticks, tick_end, tick_end*cpu_ticks);
#endif
    
    for (uint64_t i=tick_st; i<=tick_end; i++) {
        
        //update before tick to update timing info
        cpu_clock = i * cpu_ticks;
        
#ifdef DEBUG_MEMSIM
        DB("step to %lu, pending_ld_map.size=%u, pending_st_map.size=%u",
           cpu_clock, pending_ld_map.size(), pending_st_map.size());
#endif
        
        for(auto it = pending_st_map.begin(); it!= pending_st_map.end();)
        {
            if(it->first > cpu_clock) break;
            std::vector<DRAMSim::Transaction>& req_list = it->second;
            for (auto it2=req_list.begin(); it2!=req_list.end(); )
            {
                bool success = mem->addTransaction(*it2);
                if(success){
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
            std::vector<DRAMSim::Transaction>& req_list = it->second;
            for (auto it2=req_list.begin(); it2!=req_list.end(); )
            {
                bool success = mem->addTransaction(*it2);
                if(success){
#ifdef DEBUG_MEMSIM
                    DB("insert addr[0x%lx] at mem_clock=%lu, success",(*it2).address,mem_clock);
#endif
                    it2=req_list.erase(it2);
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
    }
    cpu_clock = next_cpu_cycle;
    
#ifdef DEBUG_MEMSIM
    dump_state();
#endif
    
}


void DramSim::clock()
{

#ifdef DEBUG_MEMSIM
    cout << "DRAMSim::clock()"<<endl;
#endif
    mem->update();
    mem_clock ++;
}

/*
bool DramSim::insert(REQ_T req)
{
    //unsigned throwAwayBits = THROW_AWAY_BITS;
    req.addr >>= 6;
    req.addr <<= 6;

    enum DRAMSim::TransactionType  transType;
    transType = (req.type==STORE ?DRAMSim::DATA_WRITE : DRAMSim::DATA_READ);
    DRAMSim::Transaction t(transType, req.addr, NULL);

    bool succeess = mem->addTransaction(t);
    if(succeess){
      if(req.type==LOAD)
          (MQ_read[req.addr]).push_back(req);
      else
          (MQ_write[req.addr]).push_back(req);

    }
    return succeess;
}
*/

void DramSim::finish_simulator(){

    mem->printStats(true);

    //MemSim::print_stats();

}


void DramSim::read_complete(unsigned id, uint64_t address, uint64_t done_cycle)
{
    process_read_requests(address);

    /*
    std::unordered_map<ADDR_T, vector<MQEnt>>::iterator it = MQ_read.find(address);
    if( it==MQ_read.end() )
    {
      PANIC("DRAMSim2::read_complete: cannot find the read request %lu", address);
    }
  
    vector<MQEnt>& read_list    = it->second;
    read_list[0].response_cycle = done_cycle;
    
    cout<<"DramSim::"<<mem_name<<std::hex<<" Read: 0x"<< address<<std::dec<< " cycle["<< read_list[0].request_cycle << " - " << read_list[0].response_cycle<<"] "<< done_cycle << endl;

    engine->process_read_resp(read_list[0]);
    
    if(read_list.size()>1)
        read_list.erase(read_list.begin());
    else
        MQ_read.erase(it);
     */
}
    
void DramSim::write_complete(unsigned id, uint64_t address, uint64_t done_cycle)
{
    /*
    std::unordered_map<ADDR_T, vector<REQ_T>>::iterator it = MQ_write.find(address);
    
    if( it==MQ_write.end() )
    {
        PANIC("DRAMSim2::write_complete: cannot find thre write request %lu", address);
    }
    
    vector<REQ_T>& write_list    = it->second;
    write_list[0].response_cycle = done_cycle;
    
    cout << "Write: 0x"<< std::hex<< address<<std::dec<< " cycle["<< write_list[0].request_cycle << " - " << write_list[0].response_cycle<<"]\n";
    */
    
    process_write_requests(address);

}

void DramSim::dump_state()
{
    
    DB("cpu_clock=%lu, mem_clock=%lu, next_event=%lu ",
       cpu_clock, mem_clock,get_next_event() );
    
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
}


uint64_t DramSim::get_next_event()
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

