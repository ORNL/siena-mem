#ifndef AR_EXECBLK_H
#define AR_EXECBLK_H

#include "ar_typedef.h"

class Node;
class DataNode;
class OpNode;

class  ExecBlock{

private:
    const std::string block_name;
    const std::string block_id;
    
    size_t      global_control;
    size_t      total_ops;
    uint64_t    total_bytes;
    size_t      window_size;
    std::vector<std::pair<size_t, size_t>> node_list;
    std::vector<DataNode>   datanode_list;
    std::vector<OpNode>     opnode_list;
    std::vector<GRPCTRL_T>  nodegroup_list;


 public:
    ExecBlock(const std::string _name, const std::string _id);
    
    ~ExecBlock();
    
    Node*  get_node(size_t pos);
    size_t  get_node_pos(size_t pos);
    
    std::vector<DataNode>&  get_datanodes(){
        return datanode_list;
    }
    
    std::vector<GRPCTRL_T>&  get_nodegroups(){
        return nodegroup_list;
    }

    DataNode& get_last_datanode(){
        return datanode_list.back();
    }
    
    DataNode& get_datanode(size_t i){
        return datanode_list[i];
    }
    
    uint64_t get_total_bytes()const{return total_bytes;}
    size_t get_total_ops()const{return total_ops;}
    size_t get_global_control()const{return global_control;}
    size_t get_num_nodes()const{return node_list.size(); }
    size_t get_num_datanodes()const{return datanode_list.size(); }
    size_t get_window_size()const{return window_size;}
    
    void add_opnode(OpNode a);
    void add_datanode(DataNode a);
    void set_global_control(size_t _in);
    void print()const;
    void print_group()const;
    void inc_bytes(uint64_t inc_bytes);
    void init_offsets();
    
};

#endif
