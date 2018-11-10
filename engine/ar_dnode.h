#ifndef AR_DNODE_H
#define AR_DNODE_H

#include "ar_typedef.h"
#include "ar_node.h"
#include <iostream>


class MemAlloc;


class DataNode : public Node{

 public:
    
    //memory obj
    ADDR_T addr_st;
    ADDR_T addr_end;
    ADDR_T size;
    size_t num_dims;
    std::vector<size_t> dim;
    ADDR_T element_size;
    size_t element_num;
    MemObj* mem_ptr;

    
    //accesses
    size_t access_count;
    ADDR_T access_size;
    size_t num_cacheline_per_access;
    AccessPattern access_pattern;
    double num_flops_per_access;
    
    //dependency
    std::vector<std::string> dependency;
    std::vector<size_t> prev;//data dependency on
    std::vector<size_t> next;//data dependency by
    size_t num_ops_per_access;
    
    //pattern
    std::vector<ADDR_T> (DataNode::*pattern_generator)(size_t);
    ADDR_T local_stride;
    size_t dist_func;
    ADDR_T diff0, diff1, diff2;
    ADDR_T s01,s0;
    size_t tile_dimX,tile_dimY,tile_dimZ,tile_dir;
    size_t cropped_dimX, cropped_dimY, cropped_dimZ,local_tile_control;
    size_t bcell;
    
    //group
    size_t group_id;
    size_t offset, ops_step;
    int64_t retired_step;
    int64_t retired_global_step;
    
 public:
    
    DataNode(MemAlloc _mem_obj,size_t _id, OpType _accesstype, size_t _accesscount, ADDR_T _accesssize);
    
    ~DataNode(){ fini();}
    
    std::vector<ADDR_T> gen_addr(size_t local_control);
    void print(std::ostream &out=std::cout)const;
    
    void set_mem_ptr(MemObj* _mem_ptr){mem_ptr=_mem_ptr;}
    void set_comp(double _in){num_flops_per_access=_in;}
    void set_group_id(size_t _gid){group_id = _gid;}
    
    void set_pattern(enum AccessPattern _p);
    void set_stride(size_t s){ local_stride = s;}
    void set_stride_offset(ADDR_T off){addr_st += off;}
    void set_random_distfunc(ADDR_T _in){dist_func = _in;}
    void set_tile(size_t dir, size_t x, size_t y, size_t z);
    void set_bcell(size_t _in){bcell=_in;}
    

    bool has_dependency()const{return dependency.size()>0; }
    double get_comp()const{return num_flops_per_access;}
    size_t get_accesses()const{return access_count;}
    MemObj* get_mem_ptr(){ return mem_ptr;}
    size_t get_group_id()const{return group_id;}
    
private:

    std::vector<ADDR_T> stride_generator(size_t m);
    std::vector<ADDR_T> random_generator(size_t m);
    
    std::vector<ADDR_T> ghostcell2d_generator(size_t m);
    std::vector<ADDR_T> ghostcell3d_generator(size_t m);
    
    std::vector<ADDR_T> stencil3p_generator(size_t m);
    std::vector<ADDR_T> stencil4p_generator(size_t m);
    std::vector<ADDR_T> stencil5p_generator(size_t m);
    std::vector<ADDR_T> stencil8p_generator(size_t m);
    std::vector<ADDR_T> stencil9p_generator(size_t m);
    std::vector<ADDR_T> stencil26p_generator(size_t m);
    std::vector<ADDR_T> stencil27p_generator(size_t m);
    
    std::vector<ADDR_T> transpose2d_generator(size_t m);
    std::vector<ADDR_T> transpose3d_generator(size_t m);
    
    std::vector<ADDR_T> tile2d_generator(size_t m);
    std::vector<ADDR_T> tile3d_generator(size_t m);
    
    std::vector<ADDR_T> conv_generator(size_t m);
    std::vector<ADDR_T> compose_generator(size_t m);
    
    void fini(){}
};


#endif
