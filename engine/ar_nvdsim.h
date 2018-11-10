#ifndef AR_NVDSIM_H
#define AR_NVDSIM_H

#include "ar_memsim.h"
#include <map>
#include "NVDIMM.h"
#include "FlashTransaction.h"

using namespace NVDSim;

const std::string NVDSim_name("NVDIMMSim");

class NVMSim : public MemSim
{
    
private:
    NVDIMM *mem;
    const string deviceIniFilename;
    const string systemIniFilename;
    const string pwdString;
    const string traceFileName;
    
    //ordered by dispatch time
    std::map<uint64_t, std::vector<FlashTransaction>> pending_ld_map;
    std::map<uint64_t, std::vector<FlashTransaction>> pending_st_map;
    
public:
    
    NVMSim(const std::string &_mem_name,const uint64_t _id,
           const bool _isMainMemory,
           const std::string &_deviceIniFilename,
           const std::string &_systemIniFilename,
           const std::string &_pwdString,
           const std::string &_traceFileName);
    NVMSim(MemSim* _in, const std::string& _outputfile);
    ~NVMSim(){}
    
    void init_simulator(BaseObj* _baseobj);
    void finish_simulator();
    
    void load(uint64_t dispatch, ADDR_T addr);
    void store(uint64_t dispatch, ADDR_T addr);
    void dump_state();
    uint64_t get_next_event();
    void advance(uint64_t t);
    
    bool insert(REQ_T req);
    
    void clock();

    
    void read_complete(uint64_t rid, uint64_t address, uint64_t done_cycle,bool mapped);
    void write_complete(uint64_t rid, uint64_t address, uint64_t done_cycle,bool mapped);
    void crit_complete(uint64_t rid, uint64_t address, uint64_t done_cycle,bool mapped);
    void power_complete(uint64_t rid, std::vector<std::vector<double>> data, uint64_t cycle, bool mapped);
};

#endif
