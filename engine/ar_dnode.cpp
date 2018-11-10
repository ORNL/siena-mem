#include <sstream>
#include <iostream>
#include <deque>
#include <cstdio>
#include <map>
#include <cfloat>
#include <memory>
#include <ctime>

#include "ar_dnode.h"
#include "ar_memalloc.h"


using namespace std;


DataNode::DataNode(MemAlloc _mem_obj,size_t _id, OpType _accesstype, size_t _accesscount, ADDR_T _accesssize)
:   Node(_accesstype, _mem_obj.name, _accesscount),
    addr_st( _mem_obj.addr_st),
    addr_end(_mem_obj.addr_end),
    size(_mem_obj.size),
    num_dims(_mem_obj.num_dims),
    element_size(_mem_obj.element_size),
    element_num(_mem_obj.element_num),
    //isWrite(_accesstype==STORE),
    access_count(_accesscount),
    access_size(_accesssize),
    num_cacheline_per_access((size_t)((access_size-1)/(ADDR_T)CACHELINE_BYTES+1))
{
    
    dim.reserve(num_dims);
    for (size_t i=0; i<num_dims; i++)
        dim.push_back(_mem_obj.dim[i]);
    
    num_flops_per_access = 0.0;
    retired_step = -1;
    retired_global_step = -1;
    access_pattern=UNSET;
    
    if(access_size != element_size)
        PANIC("access_size(%llu)!=element_size(%llu)",access_size, element_size);
    
    pattern_generator = nullptr;
}


void DataNode::set_pattern(enum AccessPattern _p)
{
    access_pattern = _p;
    
    //perform sanity check
    //setup auxillary states
    switch(access_pattern){
        
        case STRIDE:
        {
            pattern_generator = &DataNode::stride_generator;
            break;
        }
        case RANDOM:
        {
            pattern_generator = &DataNode::random_generator;
            srand(time(0));
            break;
        }
        case GHOSTCELL:
        {
            assert(num_dims==2 || num_dims==3);
            diff0 = access_size;
            diff1 = dim[0]*access_size;
            diff2 = dim[0]*dim[1]*access_size;
            if( num_dims==2 )
                pattern_generator = &DataNode::ghostcell2d_generator;
            else
                pattern_generator = &DataNode::ghostcell3d_generator;
            break;
        }
        case STENCIL3P:
        {
	    assert(num_dims==3);
            diff0 = access_size;
            diff1 = dim[0]*access_size;
            diff2 = dim[0]*dim[1]*access_size;
            pattern_generator = &DataNode::stencil3p_generator;
            break;
        }
        case STENCIL4P:
        {
            assert(num_dims==2);
            diff0 = access_size;
            diff1 = dim[0]*access_size;
            diff2 = dim[0]*dim[1]*access_size;
            pattern_generator = &DataNode::stencil4p_generator;
            break;
        }        
        case STENCIL5P:
        {
            assert(num_dims==2);
            diff0 = access_size;
            diff1 = dim[0]*access_size;
            diff2 = dim[0]*dim[1]*access_size;
            pattern_generator = &DataNode::stencil5p_generator;
            break;
        }
        case STENCIL8P:
        {
            assert(num_dims==3);
            diff0 = access_size;
            diff1 = dim[0]*access_size;
            diff2 = dim[0]*dim[1]*access_size;
            pattern_generator = &DataNode::stencil8p_generator;
            break;
        }
        case STENCIL9P:
        {
            assert(num_dims>=2);
            diff0 = access_size;
            diff1 = dim[0]*access_size;
            diff2 = dim[0]*dim[1]*access_size;
            pattern_generator = &DataNode::stencil9p_generator;
            break;
        }
        case STENCIL26P:
        {
            assert(num_dims==3);
            diff0 = access_size;
            diff1 = dim[0]*access_size;
            diff2 = dim[0]*dim[1]*access_size;
            pattern_generator = &DataNode::stencil26p_generator;
            break;
        }
        case STENCIL27P:
        {
            assert(num_dims==3);
            diff0 = access_size;
            diff1 = dim[0]*access_size;
            diff2 = dim[0]*dim[1]*access_size;
            pattern_generator = &DataNode::stencil27p_generator;
            break;
        }
        case TILE:
        {
            if(num_dims==2){
                assert(tile_dir==0 || tile_dir==1);
                pattern_generator = &DataNode::tile2d_generator;
            }
            break;
        }
        case CONV:
        {
            assert(num_dims==2);
            diff0 = access_size;
            diff1 = dim[0]*access_size;
            pattern_generator = &DataNode::conv_generator;
            break;
        }
        case TRANSPOSE:
            break;
        case COMPOSE:
            break;
        default:
        {
            PANIC("Unknow access pattern %i", access_pattern);
            break;
        }
    }
}


void DataNode::set_tile(size_t dir, size_t x, size_t y, size_t z)
{
    tile_dir  = dir;
    tile_dimX = x;
    tile_dimY = y;
    tile_dimZ = z;
    
    cropped_dimX = (dim[0]/tile_dimX)*tile_dimX;
    cropped_dimY = (dim[1]/tile_dimY)*tile_dimY;
    cropped_dimZ = (dim[2]/tile_dimZ)*tile_dimZ;
    local_tile_control = cropped_dimX*cropped_dimY*cropped_dimZ/(tile_dimX*tile_dimY*tile_dimZ);
}


void DataNode::print(ostream &out)const{
    
    Node::print(out);
    
    stringstream ss("");
    switch(access_pattern){
        case STRIDE:
            ss << "Stride("<<local_stride<<")";
            break;
        case RANDOM:
            ss << "Random";
            break;
        case GHOSTCELL:
            ss<< "GhostCell";
            break;
        case STENCIL3P:
            ss <<"Stencil_3P";
            break;
        case STENCIL4P:
            ss <<"Stencil_4P";
            break;
        case STENCIL5P:
            ss<< "Stencil_5P";
            break;
        case STENCIL8P:
            ss<< "Stencil_8P";
            break;
        case STENCIL9P:
            ss<< "Stencil_9P";
            break;
        case STENCIL26P:
            ss<< "Stencil_26P";
            break;
        case STENCIL27P:
            ss<< "Stencil_27P";
            break;
        case TEMPLATE:
            ss << "Template";
            break;
        case TRANSPOSE:
            ss<< "Transpose";
            break;
        case TILE:
            ss<< "Tile:"<<tile_dimX<<", "<<tile_dimY<<", "<<tile_dimZ;
            break;
        case CONV:
            ss<< "Conv:"<<tile_dimX<<", "<<tile_dimY;
            break;
        case UNSET:
            ss << "Pattern is NOT set yet";
            break;
        default:
            PANIC("Unknow access pattern %i", access_pattern);
            break;
    }
    string s = ss.str();

    
    stringstream ss1("");
    for (auto d : dependency) {
        ss1 << "[" << d << "] ";
    }
    string s1 = ss1.str();
    
    
    stringstream ss2("");
    for (auto d : prev) {
        ss2 << "["<<d<<"] ";
    }
    string s2 = ss2.str();
    
    
    stringstream ss3("");
    for (auto d : next) {
        ss3 << "["<<d<<"] ";
    }
    string s3 = ss3.str();
    
    INFO("DataNode %s::\n\tMemAlloc: [0x%lx - 0x%lx], [%d] dimensions\n\tAccessed: %lu of size[%lu]\n\tOps_per_access: %.1f\n\tPattern: %s\n\tDependency: %s\n\tPrev: %s\n\tNext: %s",
         node_name.c_str(),
         addr_st,
         addr_end,
         num_dims,
         access_count,
         access_size,
         num_flops_per_access,
         s.c_str(),
         s1.c_str(),
         s2.c_str(),
         s3.c_str());
    
}


inline vector<ADDR_T> DataNode::stride_generator(size_t local_control)
{
    
    vector<ADDR_T> list;
    size_t total_stride = local_control%element_num;
    ADDR_T access_addr  = addr_st + total_stride*local_stride;
    
    list.push_back(access_addr);
    for(size_t i=1;i<num_cacheline_per_access;i++)
    {
        access_addr += CACHELINE_BYTES;
        list.push_back(access_addr);
    }
    
#ifdef DEBUG_DATA
    for (auto addr : list) {
        DB("%s::addr = 0x%lx", node_name.c_str(), addr);
        if( addr > addr_end){
            PANIC("%s::addr(0x%lx) > addr_end(0x%lx)",
                  node_name.c_str(),addr,addr_end);
        }
    }
#endif
    
    return list;
}


inline vector<ADDR_T> DataNode::random_generator(size_t local_control)
{
    
    vector<ADDR_T> list;
    //generate random within the data range
    double r = ((double)rand()/(double)RAND_MAX);
    //double r = ((double)(local_control)/(double)RAND_MAX);
    ADDR_T n = (ADDR_T) (element_num*r);
    ADDR_T access_addr = addr_st + n*access_size;
    
    list.push_back(access_addr);
    for(size_t i=1;i<num_cacheline_per_access;i++)
    {
        access_addr += CACHELINE_BYTES;
        list.push_back(access_addr);
    }
    
#ifdef DEBUG_DATA
    for (auto addr : list) {
        DB("%s::addr = 0x%lx", node_name.c_str(), addr);
        if( addr > addr_end){
            PANIC("%s::addr(0x%lx) > addr_end(0x%lx)",
                  node_name.c_str(),addr,addr_end);
        }
    }
#endif
    
    return list;
}


inline vector<ADDR_T> DataNode::ghostcell2d_generator(size_t local_control)
{
    //excuclding ghost cells
    //the center coord starts from (1,1) to (dim[0]-2, dim[1]-2)
    size_t num_dim0 = local_control/(dim[0]-2);
    size_t id_dim0  = local_control - num_dim0*(dim[0]-2) + 1;
    size_t id_dim1  = num_dim0 + 1;
    ADDR_T access_addr = addr_st + diff1*id_dim1 + diff0*id_dim0;
    
#ifdef DEBUG_DATA
    DB("%s::center coord[ %u %u ] addr=0x%lx",node_name.c_str(),id_dim0,id_dim1,access_addr);
#endif
    
    vector<ADDR_T> list;
    list.push_back(access_addr);
    for(size_t i=1;i<num_cacheline_per_access;i++)
    {
        access_addr += CACHELINE_BYTES;
        list.push_back(access_addr);
    }
    
#ifdef DEBUG_DATA
    for (auto addr : list) {
        DB("%s::addr = 0x%lx", node_name.c_str(), addr);
        if( addr > addr_end){
            PANIC("%s::addr(0x%lx) > addr_end(0x%lx)",
                  node_name.c_str(),addr,addr_end);
        }
    }
#endif
    
    return list;
    
}


inline vector<ADDR_T> DataNode::ghostcell3d_generator(size_t local_control)
{
    //excuclding ghost cells
    //the center coord starts from (1,1,1) to (dim[0]-2, dim[1]-2, dim[2]-2)
    size_t num_dim0 = local_control/(dim[0]-2);
    size_t id_dim0  = local_control - num_dim0*(dim[0]-2);
    size_t id_dim2  = num_dim0/(dim[1]-2);
    size_t id_dim1  = num_dim0 - id_dim2*(dim[1]-2);
    id_dim0 ++;
    id_dim1 ++;
    id_dim2 ++;
    
    ADDR_T access_addr = addr_st + diff2*id_dim2 + diff1*id_dim1 + diff0*id_dim0;
    
#ifdef DEBUG_DATA
    DB("%s::center coord[ %u %u %u ] addr=0x%lx",node_name.c_str(),id_dim0,id_dim1,id_dim2,access_addr);
#endif
    
    vector<ADDR_T> list;
    list.push_back(access_addr);
    for(size_t i=1;i<num_cacheline_per_access;i++)
    {
        access_addr += CACHELINE_BYTES;
        list.push_back(access_addr);
    }
    
#ifdef DEBUG_DATA
    for (auto addr : list) {
        DB("%s::addr = 0x%lx", node_name.c_str(), addr);
        if( addr > addr_end){
            PANIC("%s::addr(0x%lx) > addr_end(0x%lx)",
                  node_name.c_str(),addr,addr_end);
        }
    }
#endif
    
    return list;
    
}


inline vector<ADDR_T> DataNode::stencil3p_generator(size_t local_control)
{
    //two direct neighbours: xLeft + xRight + itself
    //the center coord starts from (1,1,1) to (dim[0]-2, dim[1]-2, dim[2]-2)
    size_t num_dim0 = local_control/(dim[0]-2);
    size_t id_dim0  = local_control - num_dim0*(dim[0]-2);
    size_t id_dim2  = num_dim0/(dim[1]-2);
    size_t id_dim1  = num_dim0 - id_dim2*(dim[1]-2);
    id_dim0 ++;
    id_dim1 ++;
    id_dim2 ++;
    
    ADDR_T access_addr = addr_st + diff2*id_dim2 + diff1*id_dim1 + diff0*id_dim0;
    
#ifdef DEBUG_DATA
    DB("%s::center coord[ %u %u %u ] addr=0x%lx",node_name.c_str(),id_dim0,id_dim1,id_dim2,access_addr);
#endif
    
    ADDR_T addrs[3];
    addrs[0] = access_addr - diff0;
    addrs[1] = access_addr;
    addrs[2] = access_addr + diff0;
    
    vector<ADDR_T> list;
    for(size_t i=0;i<3;i++)
    {
        ADDR_T access_addr = addrs[i];
        list.push_back(access_addr);
        for(size_t i=1;i<num_cacheline_per_access;i++)
        {
            access_addr += CACHELINE_BYTES;
            list.push_back(access_addr);
        }
    }
    
#ifdef DEBUG_DATA
    for (auto addr : list) {
        DB("%s::addr = 0x%lx", node_name.c_str(), addr);
        if( addr > addr_end){
            PANIC("%s::addr(0x%lx) > addr_end(0x%lx)",
                  node_name.c_str(),addr,addr_end);
        }
    }
#endif
    
    return list;
}

inline vector<ADDR_T> DataNode::stencil4p_generator(size_t local_control)
{
    //the center coord starts from (1,1) to (dim[0]-2, dim[1]-2)
    size_t num_dim0 = local_control/(dim[0]-2);
    size_t id_dim0  = local_control - num_dim0*(dim[0]-2) + 1;
    size_t id_dim1  = num_dim0 +1;
    ADDR_T access_addr = addr_st + diff1*id_dim1 + diff0*id_dim0;
    
#ifdef DEBUG_DATA
    DB("%s::center coord[ %u %u ] addr=0x%lx",node_name.c_str(),id_dim0,id_dim1,access_addr);
#endif
    
    ADDR_T addrs[4];
    //four direct neighbours
    addrs[0] = access_addr - diff0;
    addrs[1] = access_addr + diff0;
    addrs[2] = access_addr - diff1;
    addrs[3] = access_addr + diff1;
    
    vector<ADDR_T> list;
    for(size_t i=0;i<4;i++)
    {
        ADDR_T access_addr = addrs[i];
        list.push_back(access_addr);
        for(size_t i=1;i<num_cacheline_per_access;i++)
        {
            access_addr += CACHELINE_BYTES;
            list.push_back(access_addr);
        }
    }
    
#ifdef DEBUG_DATA
    for (auto addr : list) {
        DB("%s::addr = 0x%lx", node_name.c_str(), addr);
        if( addr > addr_end){
            PANIC("%s::addr(0x%lx) > addr_end(0x%lx)",
                  node_name.c_str(),addr,addr_end);
        }
    }
#endif
    
    return list;
}


inline vector<ADDR_T> DataNode::stencil5p_generator(size_t local_control)
{
    //two direct neighbours: xLeft + xRight + itself + yLeft + yRight
    //the center coord starts from (1,1,1) to (dim[0]-2, dim[1]-2, dim[2]-2)
    size_t num_dim0 = local_control/(dim[0]-2);
    size_t id_dim0  = local_control - num_dim0*(dim[0]-2);
    size_t id_dim2  = num_dim0/(dim[1]-2);
    size_t id_dim1  = num_dim0 - id_dim2*(dim[1]-2);
    id_dim0 ++;
    id_dim1 ++;
    id_dim2 ++;
    
    ADDR_T access_addr = addr_st + diff2*id_dim2 + diff1*id_dim1 + diff0*id_dim0;
    
#ifdef DEBUG_DATA
    DB("%s::center coord[ %u %u %u ] addr=0x%lx",node_name.c_str(),id_dim0,id_dim1,id_dim2,access_addr);
#endif
    
    ADDR_T addrs[5];
    addrs[0] = access_addr - diff0;
    addrs[1] = access_addr;
    addrs[2] = access_addr + diff0;
    addrs[3] = access_addr - diff1;
    addrs[4] = access_addr + diff1;
    
    vector<ADDR_T> list;
    for(size_t i=0;i<5;i++)
    {
        ADDR_T access_addr = addrs[i];
        list.push_back(access_addr);
        for(size_t i=1;i<num_cacheline_per_access;i++)
        {
            access_addr += CACHELINE_BYTES;
            list.push_back(access_addr);
        }
    }
    
#ifdef DEBUG_DATA
    for (auto addr : list) {
        DB("%s::addr = 0x%lx", node_name.c_str(), addr);
        if( addr > addr_end){
            PANIC("%s::addr(0x%lx) > addr_end(0x%lx)",
                  node_name.c_str(),addr,addr_end);
        }
    }
#endif
    
    return list;
}

inline vector<ADDR_T> DataNode::stencil8p_generator(size_t local_control)
{
    
    //four direct neighbours + four diagonal neighbours
    //the center coord starts from (0,1,1) to (dim[0]-1, dim[1]-2, dim[2]-2)
    size_t num_dim0 = local_control/dim[0];
    size_t id_dim0  = local_control - num_dim0*dim[0];
    size_t id_dim2  = num_dim0/(dim[1]-2);
    size_t id_dim1  = num_dim0 - id_dim2*(dim[1]-2);
    id_dim1 ++;
    id_dim2 ++;
    
    ADDR_T access_addr = addr_st + diff2*id_dim2 + diff1*id_dim1 + diff0*id_dim0;
    
#ifdef DEBUG_DATA
    DB("%s::center coord[ %u %u %u ] addr=0x%lx",node_name.c_str(),id_dim0,id_dim1,id_dim2,access_addr,diff0,diff1,diff2);
#endif
    
    ADDR_T addrs[8];
    //four direct neighbours
    addrs[0] = access_addr - diff1;
    addrs[1] = access_addr + diff1;
    addrs[2] = access_addr - diff2;
    addrs[3] = access_addr + diff2;
    //four diagonal neighbours on the same slice
    addrs[4] = access_addr - diff1- diff2;
    addrs[5] = access_addr + diff1- diff2;
    addrs[6] = access_addr - diff1+ diff2;
    addrs[7] = access_addr + diff1+ diff2;
    
    vector<ADDR_T> list;
    for(size_t i=0;i<8;i++)
    {
        ADDR_T access_addr = addrs[i];
        list.push_back(access_addr);
        for(size_t i=1;i<num_cacheline_per_access;i++)
        {
            access_addr += CACHELINE_BYTES;
            list.push_back(access_addr);
        }
    }
    
#ifdef DEBUG_DATA
    for (auto addr : list) {
        DB("%s::addr = 0x%lx", node_name.c_str(), addr);
        if( addr > addr_end){
            PANIC("%s::addr(0x%lx) > addr_end(0x%lx)",
                  node_name.c_str(),addr,addr_end);
        }
    }
#endif
    
    return list;
}


inline vector<ADDR_T> DataNode::stencil9p_generator(size_t local_control)
{
    
    //four direct neighbours + self + four diagonal neighbours
    //the center coord starts from (0,1,1) to (dim[0]-1, dim[1]-2, dim[2]-2)
    size_t num_dim0 = local_control/dim[0];
    size_t id_dim0  = local_control - num_dim0*dim[0];
    size_t id_dim2  = num_dim0/(dim[1]-2);
    size_t id_dim1  = num_dim0 - id_dim2*(dim[1]-2);
    id_dim1 ++;
    id_dim2 ++;
    
    ADDR_T access_addr = addr_st + diff2*id_dim2 + diff1*id_dim1 + diff0*id_dim0;
    
#ifdef DEBUG_DATA
    DB("%s::center coord[ %u %u %u ] addr=0x%lx",node_name.c_str(),id_dim0,id_dim1,id_dim2,access_addr,diff0,diff1,diff2);
#endif
    
    ADDR_T addrs[9];
    addrs[0] = access_addr;
    //four direct neighbours
    addrs[1] = access_addr - diff1;
    addrs[2] = access_addr + diff1;
    addrs[3] = access_addr - diff2;
    addrs[4] = access_addr + diff2;
    //four diagonal neighbours on the same slice
    addrs[5] = access_addr - diff1 - diff2;
    addrs[6] = access_addr + diff1 - diff2;
    addrs[7] = access_addr - diff1 + diff2;
    addrs[8] = access_addr + diff1 + diff2;
    
    vector<ADDR_T> list;
    for(size_t i=0;i<9;i++)
    {
        ADDR_T access_addr = addrs[i];
        list.push_back(access_addr);
        for(size_t i=1;i<num_cacheline_per_access;i++)
        {
            access_addr += CACHELINE_BYTES;
            list.push_back(access_addr);
        }
    }
    
#ifdef DEBUG_DATA
    for (auto addr : list) {
        DB("%s::addr = 0x%lx", node_name.c_str(), addr);
        if( addr > addr_end){
            PANIC("%s::addr(0x%lx) > addr_end(0x%lx)",
                  node_name.c_str(),addr,addr_end);
        }
    }
#endif
    
    return list;
}


inline vector<ADDR_T> DataNode::stencil26p_generator(size_t local_control)
{
    
    //6 direct neighbours + 12 diagonal neighbours + 8 corners
    //the center coord starts from (0,1,1) to (dim[0]-1, dim[1]-2, dim[2]-2)
    size_t num_dim0 = local_control/dim[0];
    size_t id_dim0  = local_control - num_dim0*dim[0];
    size_t id_dim2  = num_dim0/(dim[1]-2);
    size_t id_dim1  = num_dim0 - id_dim2*(dim[1]-2);
    id_dim1 ++;
    id_dim2 ++;
    
    ADDR_T access_addr = addr_st + diff2*id_dim2 + diff1*id_dim1 + diff0*id_dim0;
    
#ifdef DEBUG_DATA
    DB("%s::center coord[ %u %u %u ] addr=0x%lx",node_name.c_str(),id_dim0,id_dim1,id_dim2,access_addr,diff0,diff1,diff2);
#endif
    
    ADDR_T addrs[26];
    //6 direct neighbours
    addrs[0] = access_addr - diff0;
    addrs[1] = access_addr + diff0;
    addrs[2] = access_addr - diff1;
    addrs[3] = access_addr + diff1;
    addrs[4] = access_addr - diff2;
    addrs[5] = access_addr + diff2;
    //12 diagonal neighbours on the same slice
    addrs[6] = access_addr - diff0 - diff1;
    addrs[7] = access_addr - diff0 + diff1;
    addrs[8] = access_addr + diff0 - diff1;
    addrs[9] = access_addr + diff0 + diff1;

    addrs[10] = access_addr - diff0 - diff2;
    addrs[11] = access_addr - diff0 + diff2;
    addrs[12] = access_addr + diff0 - diff2;
    addrs[13] = access_addr + diff0 + diff2;
    
    addrs[14] = access_addr - diff1 - diff2;
    addrs[15] = access_addr - diff1 + diff2;
    addrs[16] = access_addr + diff1 - diff2;
    addrs[17] = access_addr + diff1 + diff2;
    
    //8 diagonal neighbours on the same slice
    addrs[18] = access_addr - diff0 - diff1 - diff2;
    addrs[19] = access_addr - diff0 - diff1 + diff2;
    addrs[20] = access_addr - diff0 + diff1 - diff2;
    addrs[21] = access_addr - diff0 + diff1 + diff2;
    addrs[22] = access_addr + diff0 - diff1 - diff2;
    addrs[23] = access_addr + diff0 - diff1 + diff2;
    addrs[24] = access_addr + diff0 + diff1 - diff2;
    addrs[25] = access_addr + diff0 + diff1 + diff2;
    
    vector<ADDR_T> list;
    for(size_t i=0;i<26;i++)
    {
        ADDR_T access_addr = addrs[i];
        list.push_back(access_addr);
        for(size_t i=1;i<num_cacheline_per_access;i++)
        {
            access_addr += CACHELINE_BYTES;
            list.push_back(access_addr);
        }
    }
    
#ifdef DEBUG_DATA
    for (auto addr : list) {
        DB("%s::addr = 0x%lx", node_name.c_str(), addr);
        if( addr > addr_end){
            PANIC("%s::addr(0x%lx) > addr_end(0x%lx)",
                  node_name.c_str(),addr,addr_end);
        }
    }
#endif
    
    return list;
}


inline vector<ADDR_T> DataNode::stencil27p_generator(size_t local_control)
{
    
    //6 direct neighbours + 12 diagonal neighbours + 8 corners
    //the center coord starts from (0,1,1) to (dim[0]-1, dim[1]-2, dim[2]-2)
    size_t num_dim0 = local_control/dim[0];
    size_t id_dim0  = local_control - num_dim0*dim[0];
    size_t id_dim2  = num_dim0/(dim[1]-2);
    size_t id_dim1  = num_dim0 - id_dim2*(dim[1]-2);
    id_dim1 ++;
    id_dim2 ++;
    
    ADDR_T access_addr = addr_st + diff2*id_dim2 + diff1*id_dim1 + diff0*id_dim0;
    
#ifdef DEBUG_DATA
    DB("%s::center coord[ %u %u %u ] addr=0x%lx",node_name.c_str(),id_dim0,id_dim1,id_dim2,access_addr,diff0,diff1,diff2);
#endif
    
    ADDR_T addrs[27];
    addrs[0] = access_addr;
    //6 direct neighbours
    addrs[1] = access_addr - diff0;
    addrs[2] = access_addr + diff0;
    addrs[3] = access_addr - diff1;
    addrs[4] = access_addr + diff1;
    addrs[5] = access_addr - diff2;
    addrs[6] = access_addr + diff2;
    //12 diagonal neighbours on the same slice
    addrs[7] = access_addr - diff0 - diff1;
    addrs[8] = access_addr - diff0 + diff1;
    addrs[9] = access_addr + diff0 - diff1;
    addrs[10] = access_addr + diff0 + diff1;
    
    addrs[11] = access_addr - diff0 - diff2;
    addrs[12] = access_addr - diff0 + diff2;
    addrs[13] = access_addr + diff0 - diff2;
    addrs[14] = access_addr + diff0 + diff2;
    
    addrs[15] = access_addr - diff1 - diff2;
    addrs[16] = access_addr - diff1 + diff2;
    addrs[17] = access_addr + diff1 - diff2;
    addrs[18] = access_addr + diff1 + diff2;
    
    //8 diagonal neighbours on the same slice
    addrs[19] = access_addr - diff0 - diff1 - diff2;
    addrs[20] = access_addr - diff0 - diff1 + diff2;
    addrs[21] = access_addr - diff0 + diff1 - diff2;
    addrs[22] = access_addr - diff0 + diff1 + diff2;
    addrs[23] = access_addr + diff0 - diff1 - diff2;
    addrs[24] = access_addr + diff0 - diff1 + diff2;
    addrs[25] = access_addr + diff0 + diff1 - diff2;
    addrs[26] = access_addr + diff0 + diff1 + diff2;
    
    vector<ADDR_T> list;
    for(size_t i=0;i<27;i++)
    {
        ADDR_T access_addr = addrs[i];
        list.push_back(access_addr);
        for(size_t i=1;i<num_cacheline_per_access;i++)
        {
            access_addr += CACHELINE_BYTES;
            list.push_back(access_addr);
        }
    }
    
#ifdef DEBUG_DATA
    for (auto addr : list) {
        DB("%s::addr = 0x%lx", node_name.c_str(), addr);
        if( addr > addr_end){
            PANIC("%s::addr(0x%lx) > addr_end(0x%lx)",
                  node_name.c_str(),addr,addr_end);
        }
    }
#endif
    
    return list;
}


inline vector<ADDR_T> DataNode::tile2d_generator(size_t local_control)
{

    local_control = local_control%local_tile_control;
    size_t id_dim0=0, id_dim1=0, id_dim2=0;
    
    //X or Y direction
    if(tile_dir==0){
        size_t num_dim0 = local_control*tile_dimX;
        id_dim0  = num_dim0%cropped_dimX;
        size_t num_dim1 = num_dim0/cropped_dimX*tile_dimY;
        id_dim1  = num_dim1%cropped_dimY;
    }else{
        size_t num_dim1 = local_control*tile_dimY;
        id_dim1  = num_dim1%cropped_dimY;
        size_t num_dim0 = num_dim1/cropped_dimY*tile_dimX;
        id_dim0  = num_dim0%cropped_dimX;
    }
    
    assert(id_dim0<dim[0] && id_dim1<dim[1]);
    
    const size_t num = tile_dimX*tile_dimY;
    ADDR_T addrs[num];
    size_t pos = 0;
    for (size_t y=0; y<tile_dimY; y++)
        for (size_t x=0; x<tile_dimX; x++)
        {
            addrs[pos++]= addr_st + diff1*(y+id_dim1) + diff0*(x+id_dim0);
        }
    
    vector<ADDR_T> list;
    for(size_t i=0;i<num;i++)
    {
        ADDR_T access_addr = addrs[i];
        list.push_back(access_addr);
        for(size_t i=1;i<num_cacheline_per_access;i++)
        {
            access_addr += CACHELINE_BYTES;
            list.push_back(access_addr);
        }
    }
    
    
#ifdef DEBUG_DATA
    for (auto addr : list) {
        DB("%s::addr = 0x%lx", node_name.c_str(), addr);
        if( addr > addr_end){
            PANIC("%s::addr(0x%lx) > addr_end(0x%lx)",
                  node_name.c_str(),addr,addr_end);
        }
    }
#endif
    
    return list;

}


inline vector<ADDR_T> DataNode::conv_generator(size_t local_control)
{
    
    size_t id_dim1 = local_control/(dim[0]-tile_dimX+1);
    size_t id_dim0 = local_control%(dim[0]-tile_dimX+1);
    
#ifdef DEBUG_DATA
    DB("local_control=%u, left bottom coord[ %u, %u ]", local_control, id_dim0, id_dim1);
#endif
    
    const size_t num = tile_dimX*tile_dimY;
    size_t pos = 0;
    ADDR_T addrs[num];
    for (size_t y=0; y<tile_dimY; y++)
        for (size_t x=0; x<tile_dimX; x++)
        {
            addrs[pos++]= addr_st + diff1*(y+id_dim1) + diff0*(x+id_dim0);
        }
    
    vector<ADDR_T> list;
    for(size_t i=0;i<num;i++)
    {
        ADDR_T access_addr = addrs[i];
        list.push_back(access_addr);
        for(size_t i=1;i<num_cacheline_per_access;i++)
        {
            access_addr += CACHELINE_BYTES;
            list.push_back(access_addr);
        }
    }
    
    
#ifdef DEBUG_DATA
    for (auto addr : list) {
        DB("%s::addr = 0x%lx", node_name.c_str(), addr);
        if( addr > addr_end){
            PANIC("%s::addr(0x%lx) > addr_end(0x%lx)",
                  node_name.c_str(),addr,addr_end);
        }
    }
#endif
    
    return list;

}


vector<ADDR_T> DataNode::gen_addr(size_t local_control)
{
    return (this->*pattern_generator)(local_control);
    
    
    /*ADDR_T access_addr;
    
    switch(access_pattern){
        case STRIDE:
        {
	  size_t total_stride = local_control%element_num;
	  access_addr = addr_st + total_stride*local_stride;

	  if(access_addr > addr_end){
                PANIC("Access %s addr(%llu) > addr_end(%llu)",
                       node_name.c_str(),access_addr,addr_end);
	  }

	  for(size_t i=0;i<num_cacheline_per_access;i++)
            {
                list.push_back(access_addr + (ADDR_T)CACHELINE_BYTES*i);
#ifdef DEBUG_DATA
                DB("%s::Stride::addr = 0x%lx", node_name.c_str(), list.back());
#endif
            }
            
            break;
        }
        case RANDOM:
        {
            //generate random within the data range
            double r = ((double)rand()/(double)RAND_MAX);
	    //double r = ((double)(local_control)/(double)RAND_MAX);
            ADDR_T n = (ADDR_T) (element_num*r);
            access_addr = addr_st + n*access_size;
            for(size_t i=0;i<num_cacheline_per_access;i++)
            {
                list.push_back(access_addr + (ADDR_T)CACHELINE_BYTES*i);
#ifdef DEBUG_DATA
                DB("%s::Random::addr = 0x%lx", node_name.c_str(), list.back());
#endif
            }
            
            break;
        }
        case STENCIL0D://excuclding ghost cells
        {
	  if(num_dims==3 ){
            //the center coord starts from (1,1,1) to (dim[0]-2, dim[1]-2, dim[2]-2)
            size_t num_dim0 = local_control/(dim[0]-2);
            size_t id_dim0  = local_control - num_dim0*(dim[0]-2);
            size_t id_dim2  = num_dim0/(dim[1]-2);
            size_t id_dim1  = num_dim0 - id_dim2*(dim[1]-2);
            id_dim0 ++;
            id_dim1 ++;
            id_dim2 ++;
            
            ADDR_T access_addr = addr_st + diff2*id_dim2 + diff1*id_dim1 + diff0*id_dim0;
#ifdef DEBUG_DATA
	    DB("%s::stencil0d::center coord[0x%lx, 0x%lx, 0x%lx] addr = 0x%lx",
		 node_name.c_str(),id_dim0,id_dim1,id_dim2,access_addr);
#endif
            if(access_addr > addr_end){
                PANIC("STENCIL0D addr(%llu)>addr_end(%llu)",access_addr,addr_end);
            }

            for(size_t i=0;i<num_cacheline_per_access;i++)
            {
                list.push_back(access_addr + (ADDR_T)CACHELINE_BYTES*i);
#ifdef DEBUG_DATA
                cout<<hex<<"   addr "<<list.back()<<dec<< endl;
#endif
            } 
	  }else if(num_dims==2 ){
            //the center coord starts from (1,1) to (dim[0]-2, dim[1]-2)
            size_t num_dim0 = local_control/(dim[0]-2);
            size_t id_dim0  = local_control - num_dim0*(dim[0]-2) + 1;
            size_t id_dim1  = num_dim0 + 1;            
            ADDR_T access_addr = addr_st + diff1*id_dim1 + diff0*id_dim0;

#ifdef DEBUG_DATA
            cout<<"   "<< node_name<< " STENCIL0D center coord[ " << id_dim0 <<", " << id_dim1<<"] addr "
		<< hex << access_addr<< dec<< ", diff0 " <<diff0<< ", diff1 "<< diff1<<endl;
#endif
            if(access_addr > addr_end){
                PANIC("STENCIL0D addr(%llu)>addr_end(%llu)",access_addr,addr_end);
            }

            for(size_t i=0;i<num_cacheline_per_access;i++)
            {
                list.push_back(access_addr + (ADDR_T)CACHELINE_BYTES*i);
#ifdef DEBUG_DATA
                cout <<hex<<"   addr "<<list.back()<<dec<< endl;
#endif
            }
	  }

	  break;
        }
        case STENCIL1D://two direct neighbours: xLeft + xRight + itself
        {
            //the center coord starts from (1,1,1) to (dim[0]-2, dim[1]-2, dim[2]-2)
            size_t num_dim0 = local_control/(dim[0]-2);
            size_t id_dim0  = local_control - num_dim0*(dim[0]-2);
            size_t id_dim2  = num_dim0/(dim[1]-2);
            size_t id_dim1  = num_dim0 - id_dim2*(dim[1]-2);
            id_dim0 ++;
            id_dim1 ++;
            id_dim2 ++;
            
            ADDR_T access_addr = addr_st + diff2*id_dim2 + diff1*id_dim1 + diff0*id_dim0;
#ifdef DEBUG_DATA
            cout <<"STENCIL1D center coord[ " << id_dim0 <<", " << id_dim1<<", " 
		 << id_dim2<<"] addr "<< hex<<access_addr<< dec
		 << ", diff0 " <<diff0<< ", diff1 " <<diff1<< ", diff2 " <<diff2<<endl;
#endif
            ADDR_T addrs[3];
            addrs[0] = access_addr - diff0;
            addrs[1] = access_addr;
            addrs[2] = access_addr + diff0;
            
            for(size_t j=0;j<3;j++)
            {
                if(addrs[j] > addr_end){
                    PANIC("STENCIL1D addrs[%zu](%llu)>addr_end(%llu)",j, addrs[j], addr_end);
                }
                
                for(size_t i=0;i<num_cacheline_per_access;i++)
                {
                    list.push_back(addrs[j] + (ADDR_T)CACHELINE_BYTES*i);
#ifdef DEBUG_DATA
                    cout <<hex<<" addr "<<list.back()<<dec<< endl;
#endif
                }
                
            }
            
            break;
        }
        case STENCIL2D://four direct neighbours + four diagonal neighbours
        {
	  if(num_dims==3 ){
            //the center coord starts from (0,1,1) to (dim[0]-1, dim[1]-2, dim[2]-2)
            size_t num_dim0 = local_control/dim[0];
            size_t id_dim0  = local_control - num_dim0*dim[0];
            size_t id_dim2  = num_dim0/(dim[1]-2);
            size_t id_dim1  = num_dim0 - id_dim2*(dim[1]-2);
            id_dim1 ++;
            id_dim2 ++;
            
            ADDR_T access_addr = addr_st + diff2*id_dim2 + diff1*id_dim1 + diff0*id_dim0;
#ifdef DEBUG_DATA
            cout<<"   "<<node_name<< " STENCIL2D center coord[ " << id_dim0 <<", " << id_dim1<<", " << id_dim2<<"] addr "
		<< hex<<access_addr<<dec<<", diff0 "<<diff0<<", diff1 " <<diff1<< ", diff2 " <<diff2<<endl;
#endif
            ADDR_T addrs[8];
            //four direct neighbours
            addrs[0] = access_addr - diff1;
            addrs[1] = access_addr + diff1;
            addrs[2] = access_addr - diff2;
            addrs[3] = access_addr + diff2;
            //four diagonal neighbours on the same slice
            addrs[4] = access_addr - diff1- diff2;
            addrs[5] = access_addr + diff1- diff2;
            addrs[6] = access_addr - diff1+ diff2;
            addrs[7] = access_addr + diff1+ diff2;

            for(size_t j=0;j<8;j++)
            {
                if(addrs[j] > addr_end){
                    PANIC("STENCIL2D addrs[%zu](%llu)>addr_end(%llu)",j, addrs[j], addr_end);
                }

                for(size_t i=0;i<num_cacheline_per_access;i++)
                {
                    list.push_back(addrs[j] + (ADDR_T)CACHELINE_BYTES*i);
#ifdef DEBUG_DATA
		    cout <<hex<<"   addr "<<list.back()<<dec<< endl;
#endif
                }
            }
	  }else if(num_dims ==2 ){

	    //the center coord starts from (1,1) to (dim[0]-2, dim[1]-2)
            size_t num_dim0 = local_control/(dim[0]-2);
            size_t id_dim0  = local_control - num_dim0*(dim[0]-2) + 1;
            size_t id_dim1  = num_dim0 +1;
            ADDR_T access_addr = addr_st + diff1*id_dim1 + diff0*id_dim0;

#ifdef DEBUG_DATA
            cout<<"   "<<node_name<<" STENCIL2D center coord[ " << id_dim0 <<", " << id_dim1<<"] addr "
		<< hex<<access_addr<<dec<<", diff0 "<<diff0<<", diff1 " <<diff1<<endl;
#endif

            ADDR_T addrs[4];
            //four direct neighbours
            addrs[0] = access_addr - diff0;
            addrs[1] = access_addr + diff0;
            addrs[2] = access_addr - diff1;
            addrs[3] = access_addr + diff1;

            for(size_t j=0;j<4;j++)
            {
                if(addrs[j] > addr_end){
                    PANIC("STENCIL2D addrs[%zu](%llu)>addr_end(%llu)",j, addrs[j], addr_end);
                }

                for(size_t i=0;i<num_cacheline_per_access;i++)
                {
                    list.push_back(addrs[j] + (ADDR_T)CACHELINE_BYTES*i);
#ifdef DEBUG_DATA
		    cout <<hex<<"   addr "<<list.back()<<dec<< endl;
#endif
                }
            }
	  } 
            break;
        }
        case STENCIL0:
        {
	  //the center coord starts from (1,1) to (dim[0]-2, dim[1]-2)
	  size_t num_dim0 = local_control/(dim[0]-2);
	  size_t id_dim0  = local_control - num_dim0*(dim[0]-2);
	  size_t id_dim1  = num_dim0;
	  id_dim0 ++;
	  id_dim1 ++;

#ifdef DEBUG_DATA
	  cout<<"DATA::STENCIL0:diff0="<<diff0<<",diff1="<<diff1<< ",local_control="<<local_control
	      <<", center["<<id_dim0<<", "<<id_dim1<<"]"<<endl;
#endif
	  ADDR_T access_addr = addr_st + diff1*id_dim1 + diff0*id_dim0;
	  for(size_t k=0;k<num_cacheline_per_access;k++)
	    {
	      list.push_back(access_addr + (ADDR_T)CACHELINE_BYTES*k);
#ifdef DEBUG_DATA
	      cout<<"DATA::STENCIL0 addr "<< hex<<list.back()<<dec<< endl;
	      assert( list.back()<addr_end);
#endif
	    }
	    break;
	}
        case STENCIL5:
        {
            //the center coord starts from (1,1) to (dim[0]-2, dim[1]-2)
	    size_t num_dim0 = local_control/(dim[0]-2);
            size_t id_dim0  = local_control - num_dim0*(dim[0]-2);
            size_t id_dim1  = num_dim0;
            id_dim0 ++;
            id_dim1 ++;

#ifdef DEBUG_DATA
            cout<<"DATA::STENCIL5:diff0="<<diff0<<",diff1="<<diff1<<",local_control="<<local_control
		<<", center["<<id_dim0<<", "<<id_dim1<<"]"<<endl;
#endif
            ADDR_T addrs[5];
	    ADDR_T access_addr = addr_st + diff1*id_dim1 + diff0*id_dim0;
	    
	    //1 center point
	    addrs[0] = access_addr;
	    //4 direct neighbours
	    addrs[1] = access_addr - diff0;
	    addrs[2] = access_addr + diff0;
	    addrs[3] = access_addr - diff1;
	    addrs[4] = access_addr + diff1;
         
	    for(size_t j=0;j<5;j++)
            {
                for(size_t k=0;k<num_cacheline_per_access;k++)
                {
                    list.push_back(addrs[j] + (ADDR_T)CACHELINE_BYTES*k);
#ifdef DEBUG_DATA
		    cout<<"DATA::STENCIL5 addr "<< hex<<list.back()<<dec<< endl;
		    assert( list.back()<addr_end);
#endif
		}
	    }
            break;
        }
    case STENCIL27:
      {
	if(num_dims!=3 ) PANIC("STENCIL27::num_dims(%i)!=3",num_dims);
	
	//the center coord starts from (1,1,1) to (dim[0]-2, dim[1]-2, dim[2]-2)
	size_t num_dim0 = local_control/(dim[0]-2);
	size_t id_dim0  = local_control - num_dim0*(dim[0]-2);
	size_t id_dim2  = num_dim0/(dim[1]-2);
	size_t id_dim1  = num_dim0 - id_dim2*(dim[1]-2);
	id_dim1 ++;
	id_dim2 ++;

#ifdef DEBUG_DATA
	cout<<"DATA::STENCIL27:diff0="<<diff0<<",diff1="<<diff1<<",diff2="<<diff2<<",local_control="
	    <<local_control<<", left slice center["<<id_dim0<<", "<<id_dim1<<", "<<id_dim2<<"]"<<endl;
#endif

	ADDR_T addrs[9];
	ADDR_T access_addr = addr_st + diff2*id_dim2 + diff1*id_dim1 + diff0*id_dim0;
	    
	//1 center point
	addrs[0] = access_addr;
	//4 direct neighbours
	addrs[1] = access_addr - diff1;
	addrs[2] = access_addr + diff1;
	addrs[3] = access_addr - diff2;
	addrs[4] = access_addr + diff2;
	//4 diagonal neighbours on the same slice
	addrs[5] = access_addr - diff1- diff2;
	addrs[6] = access_addr - diff1+ diff2;
	addrs[7] = access_addr + diff1- diff2;
	addrs[8] = access_addr + diff1+ diff2;
         
	for(size_t i=0;i<3;i++)
	  for(size_t j=0;j<9;j++)
            {
	      for(size_t k=0;k<num_cacheline_per_access;k++)
                {
                    list.push_back(addrs[j] + (ADDR_T)CACHELINE_BYTES*k);
#ifdef DEBUG_DATA
		    cout<<"DATA::STENCIL27 addr "<< hex<<list.back()<<dec<< endl;
		    if(list.back() > addr_end){
		      PANIC("STENCIL27:: addrs[%zu](%llu)>addr_end(%llu)",j*i,list.back(),addr_end);
		    }
#endif
                }
		addrs[j] += diff0;
            }
	break;
      }
    case TILE: //tile(dx,dy,dz) with at least one of dx,dy,dz=1
        {
#ifdef DEBUG_DATA
	  cout <<"TILE: local_control="<<local_control<<" ,"<<local_control%local_tile_control<<endl;
#endif
	  local_control = local_control%local_tile_control;	  
	  size_t id_dim0=0, id_dim1=0, id_dim2=0;
	  //XY tile
	  if(tile_dimZ==1){
                size_t num_dim0 = local_control*tile_dimX;
                id_dim0  = num_dim0%cropped_dimX;
                size_t num_dim1 = num_dim0/cropped_dimX*tile_dimY;
                id_dim1  = num_dim1%cropped_dimY;
                id_dim2  = num_dim1/cropped_dimY;
#ifdef DEBUG_DATA
		cout << "num_dim0 "<<num_dim0<<", num_dim1 "<<num_dim1
		     <<" id_dim ["<<id_dim0<<", "<<id_dim1<<", "<<id_dim2<<"], "
		     << "cropped_dim["<<cropped_dimX<<", "<<cropped_dimY<<", "<<cropped_dimZ<<"]"<<endl;
#endif
            }else if(tile_dimY==1){
                size_t num_dim0 = local_control*tile_dimX;
                id_dim0  = num_dim0%cropped_dimX;
                size_t num_dim2 = num_dim0/cropped_dimX*tile_dimZ;
                id_dim2  = num_dim2%cropped_dimZ;
                id_dim1  = num_dim2/cropped_dimZ;
            }else if(tile_dimX==1){
                size_t num_dim1 = local_control*tile_dimY;
                id_dim1  = num_dim1%cropped_dimY;
                size_t num_dim2 = num_dim1/cropped_dimY*tile_dimZ;
                id_dim2  = num_dim2%cropped_dimZ;
                id_dim0  = num_dim2/cropped_dimX;
            }

	    if(id_dim0>=dim[0] || id_dim1>=dim[1] || id_dim2>=dim[2])
	      PANIC("id_dim0(%zu)>=dim[0](%<u) || id_dim1(%zu)>=dim[1](%<u) || id_dim2(%zu)>=dim[2](%zu)",
		    id_dim0,dim[0], id_dim1,dim[1], id_dim2,dim[2]);
            
            for (size_t z=0; z<tile_dimZ; z++)
            for (size_t y=0; y<tile_dimY; y++)
            for (size_t x=0; x<tile_dimX; x++)
            {
                ADDR_T access_addr = addr_st + diff2*(z+id_dim2) + diff1*(y+id_dim1) + diff0*(x+id_dim0);
                if(access_addr > addr_end){
                    PANIC("Tile coord[%zu,%zu,%zu]=(%x)>addr_end(%x)",(x+id_dim0),(y+id_dim1),(z+id_dim2),access_addr,addr_end);
                }   
                for(size_t i=0;i<num_cacheline_per_access;i++)
                {
                    list.push_back(access_addr + (ADDR_T)CACHELINE_BYTES*i);
#ifdef DEBUG_DATA
                    cout <<"TILE coord[" << (x+id_dim0)<<","<< (y+id_dim1)<<","<<(z+id_dim2)
			 <<"] addr "<< hex<<list.back()<<dec<<endl;
#endif
                }                
            }            
            break;
        }
        case CONV:
        {
	  size_t id_dim1 = local_control/(dim[0]-tile_dimX+1);
	  size_t id_dim0 = local_control%(dim[0]-tile_dimX+1);
#ifdef DEBUG_DATA
	  cout <<"CONV:control="<<local_control<<" left bottom["<<id_dim0<<", "<<id_dim1<<"] "<<endl;
#endif      
	  for (size_t y=0; y<tile_dimY; y++)
	    for (size_t x=0; x<tile_dimX; x++)
	      {
		ADDR_T access_addr=addr_st+diff1*(y+id_dim1)+diff0*(x+id_dim0);
		if(access_addr > addr_end){
		  PANIC("CONV=([%u,%u]%x)>addr_end(%x)",(x+id_dim0),(y+id_dim1),access_addr,addr_end);
		}
		for(size_t i=0;i<num_cacheline_per_access;i++)
		  {
		    list.push_back(access_addr + (ADDR_T)CACHELINE_BYTES*i);
#ifdef DEBUG_DATA
		    cout <<"CONV coord["<< (x+id_dim0)<<","<< (y+id_dim1)
			 <<"] addr "<< hex<<list.back()<<dec<<endl;
#endif
		  }
	      }
	  break;
	}
    default:break;
    }

    return list;*/
}


