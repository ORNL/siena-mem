#ifndef AR_CTX_H
#define AR_CTX_H

#include "ar_typedef.h"

class CacheObj;
class ExecBlock;
class Core;
class CoreGroup;
class DataNode;


class  Context{

private:
    size_t tid;
    
    size_t fetched_global_step;
    size_t global_control_max;
    
    std::vector<DataNode>   datanode_list;
    std::vector<GRPCTRL_T> nodegroup_list;
    size_t                  curr_group_id;
    
    //used for window
    std::vector<OP_T> window;
    size_t win_head;
    size_t win_load;
    
    //timing info
    uint64_t            time_next_event;
    
    //execution
    size_t rob_depth, ib_depth, lfb_depth,watermark;
    size_t alu, ld, st;
    size_t avail_alu,avail_ld, avail_st;
    size_t t_cpu, t_alu, t_ld, t_st;
    bool   has_complete_op;
    bool   has_ready_ops;
    bool   is_done;
    CacheObj*   l1;
    
    
    //buffer for memory requests that are rejected by MC
    std::vector<REQ_T>  pendingreq_arr;
    std::vector<REQ_T>  pending_ld_list;
    std::vector<REQ_T>  pending_st_list;
    
    //stats
    //uint64_t   retired_ops;
    //size_t        ops_step;
    double   dispatch_util;
    double        lfb_util;
    double  pendreq_cycles;
    
public:
    Context();
    ~Context();
    
    void init(size_t _tid, size_t _st, size_t _end, CoreGroup &cores, ExecBlock& blk);
    void fini();
    void mark_done(){is_done=true;}
    void init_ports();
    void fetch_global_step();
    size_t fetch_global_step(size_t st);
    void dispatch(OP_T &op);
    void init_window();
    void fetch_ops();
    void retire_head();
    void resend_pending_req();
    void update_dependency(OP_T &op);
    void update_loop();
    void connect_core(Core& core);
    void complete_op(REQ_T req);
    void mark_complete_op(){has_complete_op = true;}
    void mark_ready_ops(){has_ready_ops = true;}
    size_t get_tid()const{return tid;}
    OP_T&  get_op(size_t pos){return window[pos];};
    bool advance(uint64_t t);
    bool advance_bk(uint64_t t);
    uint64_t get_next_event(bool mode)const;
    bool warmup();
    void dump_state();
    //uint64_t get_total_ops()const{return retired_ops;}

private:
    bool try_dispatch_ld(REQ_T req);
    bool try_dispatch_st(REQ_T req);
    void fetch_global_step_warmup();
};

#endif
