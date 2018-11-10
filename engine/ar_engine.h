#ifndef AR_ENGINE_H
#define AR_ENGINE_H

#include "ar.h"
#include "ar_typedef.h"

#include "walkers/AspenTool.h"
#include "model/ASTAppModel.h"
#include "model/ASTMachModel.h"
#include "model/ASTDataStatement.h"
#include "expr/Expression.h"
#include "expr/Identifier.h"
#include "app/ASTRequiresStatement.h"
#include "mach/ASTMachComponent.h"

class Node;
class DataNode;
class Context;
class CoreGroup;
class CacheObj;
class MemObj;
class MemAlloc;
class ExecBlock;

class Engine : public AspenTool{

public:
    uint64_t  cpu_cycles;
    ADDR_T    free_addr;
    bool      has_caches;

private:
    //Application
    std::unordered_map<string,ExecBlock>    executeblock_map;
    std::unordered_map<string,MemAlloc>     memobj_map;
    
    //Cores
    std::unordered_map<string, CoreGroup>   machine_cores;
    
    //Caches
    std::unordered_map<string, std::vector<CacheObj>> machine_caches;

    //Memories
    std::unordered_map<string, MemObj>      machine_memories;

    //execution
    std::vector<Context>                    t_ctx;
    size_t                                  num_threads,num_finished_threads;
    size_t                                  thread_st,thread_end;
    size_t                                  global_control;
    vector<double>                          parstack;
    uint64_t                                total_cycles;
    uint64_t                                total_bytes;
    bool                                    is_warmup;

    //mapping
    std::unordered_map<string, string>      connection_map;
    std::unordered_map<string, string>      alloc_mem_map;
    std::unordered_map<string, CoreGroup*>  kernel_core_map;
    
    
    //Auxillary
    std::unordered_map<size_t,int*>         random_number;
    MemSim*                                 default_memsim_ptr;
    MemObj*                                 default_mem_ptr;
    bool                                    has_hms;
    std::vector<MemSim*>                    memsim_ptr_list;
    std::unordered_map<MemSim*,std::vector<ALLOC_T>> memsim_alloc_map;
    std::unordered_map<MemObj*,std::vector<ALLOC_T>> mem_alloc_map;

public:
    
    Engine(ASTAppModel *app, ASTMachModel *mach, ASTMapModel *mapping);
    ~Engine();
    void process_read_resp(ADDR_T addr_st, ADDR_T addr_end, size_t mem_level);
    void process_write_resp(ADDR_T addr_st, ADDR_T addr_end, size_t mem_level);
    Context& get_ctx(size_t tid);
    MemSim*  find_memsim(ADDR_T addr);
    MemObj*  find_memobj(ADDR_T addr);
    void dump_state();
 
protected:

  TraversalMode TraversalModeHint(const ASTControlStatement *here){ return Explicit;}
  void StartKernel(TraversalMode mode, const ASTKernel *k);
  void EndKernel(TraversalMode mode, const ASTKernel *k);
    
  void StartIterate(TraversalMode mode, const ASTControlIterateStatement *s);
  void EndIterate(TraversalMode mode, const ASTControlIterateStatement *s);
    
  void StartMap(TraversalMode mode, const ASTControlMapStatement *s);
  void EndMap(  TraversalMode mode, const ASTControlMapStatement *s);

  void StartPar(TraversalMode mode, const ASTControlParallelStatement *s);
  void EndPar(  TraversalMode mode, const ASTControlParallelStatement *s);

  void StartSeq(TraversalMode mode, const ASTControlSequentialStatement *s);  
  void EndSeq(  TraversalMode mode, const ASTControlSequentialStatement *s);

  void Execute(const ASTExecutionBlock *e);
    
 private:
    void init();
    void fini();
    void config_core();
    void config_cache();
    void config_mem();
    void connect_components();
    void  sync_ticks(CoreGroup &core_group);

    void init_data_placement();
    void init_kernel_placement();
    void config_machine();
    void init_simulator();
    
    void construct_graph(const ASTExecutionBlock *e);
    void build_nodes(const ASTExecutionBlock*, ExecBlock& blk);
    void parse_memtraits(const ASTRequiresStatement *rs, DataNode& a);
    void set_globalcontrol(ExecBlock& blk);
    void build_execgraph( ExecBlock& blk);
    void build_tracegraph(ExecBlock& blk);
    void end_cachesim();
    void end_memsim();
    
    void print_final_stats();
    void print_progress();
    void cache_warmup(ADDR_T addr, bool isWrite);
    bool cache_lookup_load(size_t pos, size_t tid);
    void cache_lookup_store(ADDR_T addr, size_t tid);

    int find_group_link_dep(int ig, ExecBlock* blk);    
    void add_group(int ig, int group_id, ExecBlock* blk);
    
    void exec_node_longstep(string core_type, string nodeid, size_t nThreads);
    void exec_node(string core_type, string nodeid, size_t nThreads);
    void exec_warmup();
    void generate_trace(string nodeid, size_t nThreads);
    void trace_driven(string nodeid, size_t nThreads);
    void init_thread_ctx(CoreGroup &cores, ExecBlock& blk);
    void fini_thread_ctx();

    void print_graph_dependency(ExecBlock* blk);
    void print_graph_group(ExecBlock* blk);
   
    
    void PushPar(double v);
    double PopPar();
    double CurrentPar();    
    
};

#endif
