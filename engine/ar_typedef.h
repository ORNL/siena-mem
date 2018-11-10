#ifndef AR_TYPEDEF_H
#define AR_TYPEDEF_H

#include <stdint.h>
#include <string>
#include <vector>
#include <cassert>
#include <map>
#include <unordered_map>
#include <cstdlib>


#define CACHELINE_BYTES 64
#define MAX_TRACE_BUFFER 8192
#define HLINE "======================================================================"


class CacheObj;
class MemObj;
class MemSim;
class Engine;
class Context;

typedef uint64_t ADDR_T;


typedef enum NodeType{
    DATA_NODE,
    OP_NODE
}NodeType;

typedef enum OpType{
    LOAD,
    STORE,
    SP,
    DP,
    MUL,
    DIV
}OpType;

typedef enum AccessPattern{
    STRIDE,
    RANDOM,
    GHOSTCELL,
    STENCIL3P,
    STENCIL4P,//an 2N point is (2N+1) point excluding the center point
    STENCIL5P,
    STENCIL8P,
    STENCIL9P,
    STENCIL26P,
    STENCIL27P,
    TILE,
    CONV,
    TRANSPOSE,
    TEMPLATE,
    COMPOSE,
    UNSET
}AccessPattern;

typedef struct REQ_T{
    ADDR_T      addr;
    uint64_t    dispatch;
    Context*    ctx_ptr;
    size_t      pos;
    MemObj*     mem_ptr;
    CacheObj*   prev_ptr;
    uint64_t    complete;
    
    //OpType      type;
    //size_t      thread_id;
    //uint64_t    issue_mem_cycle;
    //uint64_t    issue_cpu_cycle;
}REQ_T;

typedef enum OP_STATUS_T{
    issued,
    pending,    //pending for dependency
    ready,      //ready to be dispatched
    dispatched, //dispatched but not finished
    completed   //completed, ready to retire
}OP_STATUS_T;

typedef struct OP_T{
    OP_STATUS_T     status;
    size_t          pos;
    size_t          prev;
    OpType          type;
    size_t          comp;
    ADDR_T          addr;
    uint64_t        dispatch;
    uint64_t        complete;
    MemObj          *mem_ptr;
    std::vector<size_t> next;
    
    size_t      node_id;
    int64_t        step;
    int64_t global_step;
    bool           mark;
}OP_T;

typedef struct GRPCTRL_T{
    size_t st_node;
    size_t end_node;
    size_t curr_group_step;
    size_t group_steps;
    size_t num_ops;
}GRPCTRL_T;


typedef struct OFFSET_T{
    size_t offset;
    size_t num;
    OFFSET_T(size_t _offset, size_t _num){offset=_offset; num=_num;}
}OFFSET_T;


typedef struct ALLOC_T{
    uint64_t st;
    uint64_t end;
}ALLOC_T;

inline size_t gcd(size_t a, size_t b){
    return b == 0 ? a : gcd(b, a%b);
}

#define PANIC(format, ...){\
    printf("PANIC::%s, %s, %d::" format "\nAborting...\n\n", __FILE__, __func__, __LINE__, __VA_ARGS__);\
    std::exit(13);\
}

#define INFO(format, ...) printf("" format "\n", __VA_ARGS__)


#define DB(format, ...) printf("%s,%d,%s::" format "\n", __FILE__, __LINE__, __func__, __VA_ARGS__)

#define TODO() {printf("TODO::%s, %s, %d\n", __FILE__, __func__, __LINE__);std::exit(13);}

#endif
