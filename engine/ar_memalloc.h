#ifndef AR_MEMALLOC_H
#define AR_MEMALLOC_H

#include "ar_typedef.h"

class MemAlloc{
 
public:
    size_t      mid;
    std::string name;
    
    ADDR_T      addr_st;
    ADDR_T      addr_end;
    ADDR_T      size;
    size_t      num_dims;
    std::vector<size_t>      dim;
    ADDR_T      element_size;
    size_t      element_num;
    
public:
    MemAlloc(){}
    ~MemAlloc(){}
};
#endif
