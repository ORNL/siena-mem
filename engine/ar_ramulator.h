#ifndef __AR_RAMULATOR_H
#define __AR_RAMULATOR_H

#include "ar_memsim.h"
#include <functional>
#include <map>

#include "Config.h"
#include "Statistics.h"
#include "Request.h"
#include "MemoryFactory.h"
#include "Memory.h"
#include "DDR3.h"
#include "DDR4.h"
#include "LPDDR3.h"
#include "LPDDR4.h"
#include "GDDR5.h"
#include "WideIO.h"
#include "WideIO2.h"
#include "HBM.h"
#include "SALP.h"

namespace ramulator {

class Request;
class MemoryBase;
class Config;

const std::string RamulatorSim_name("Ramulator");
    
class RamulatorSim : public MemSim
{
private:
    MemoryBase *mem;
    const std::string configs_file;
    const int cacheline;
    const std::string outputfile;
    const std::function<void(ramulator::Request&)> read_cb_func;
    const std::function<void(ramulator::Request&)> write_cb_func;
    
    std::map<uint64_t, std::vector<ramulator::Request>> pending_ld_map;//ordered by dispatch time
    std::map<uint64_t, std::vector<ramulator::Request>> pending_st_map;

public:
    
    RamulatorSim(bool _isMainMemory, const std::string& _configs_file, const std::string& _outputfile);
    RamulatorSim(MemSim* _in, const std::string& _outputfile);
    ~RamulatorSim(){}
    
    void init_simulator(BaseObj* _baseobj);
    void load(uint64_t dispatch, ADDR_T addr);
    void store(uint64_t dispatch, ADDR_T addr);
    void dump_state();
    uint64_t get_next_event();
    void advance(uint64_t t);
    
    bool insert(REQ_T req);
    
    void clock();
    
    void finish_simulator(void);
    
    void read_complete(ramulator::Request& r);    
    void write_complete(ramulator::Request& r);
 
private:
    void try_insert_st();
    void try_insert_ld();
};

}
#endif
