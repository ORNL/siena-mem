#ifndef AR_DRAMSIM_H
#define AR_DRAMSIM_H

#include "ar_memsim.h"
#include "MultiChannelMemorySystem.h"
#include "Transaction.h"

const std::string sim_name("DRAMSim2");

class DramSim : public MemSim
{
    
private:
    MultiChannelMemorySystem *mem;
    const string deviceIniFilename;
    const string systemIniFilename;
    const string pwdString;
    const string traceFileName;

    std::map<uint64_t, std::vector<DRAMSim::Transaction>> pending_ld_map;//ordered by dispatch time
    std::map<uint64_t, std::vector<DRAMSim::Transaction>> pending_st_map;
    

public:
 DramSim(const std::string &_mem_name,const int _mem_id,bool _isMainMemory,
	 const std::string &_deviceIniFilename,
	 const std::string &_systemIniFilename,
	 const std::string &_pwdString,
	 const std::string &_traceFileName)
    :
    MemSim(sim_name, _deviceIniFilename, 64, _isMainMemory),
    deviceIniFilename(_deviceIniFilename),
    systemIniFilename(_systemIniFilename),
    pwdString(_pwdString),
    traceFileName(_traceFileName)
    {
    }

    ~DramSim(){
    
        if(mem) delete(mem);
    
    }
    
    void dump_state();
    void advance(uint64_t t);
    void load(uint64_t dispatch, ADDR_T addr);
    void store(uint64_t dispatch, ADDR_T addr);
    uint64_t get_next_event();
    
    MultiChannelMemorySystem* get_mem() {return mem;}

    void init_simulator(BaseObj* _baseobj);
    void finish_simulator();
    
    bool insert(REQ_T req);
    
    void clock();
    


    void read_complete(unsigned id, uint64_t address, uint64_t done_cycle);

    void write_complete(unsigned id, uint64_t address, uint64_t done_cycle);

};

#endif
