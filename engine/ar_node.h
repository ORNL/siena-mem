#ifndef AR_NODE_H
#define AR_NODE_H

#include "ar_typedef.h"

class Node{
    
public:
    static size_t num_instances;

    size_t                 node_id;
    OpType                 op_type;
    NodeType               node_type;
    std::string            node_name;
    size_t                 count;
    
    //dependency
    //std::vector<OFFSET_T>       next_ops;
    size_t                  num_prev_ops;
    
public:
    Node(OpType _type, std::string _node_name, size_t _count)
    :
    node_id(num_instances++),
    op_type(_type),
    node_name(_node_name),
    count(_count)
    {
      num_prev_ops = 0;
        switch (op_type) {
            case LOAD:
            case STORE:
                node_type = DATA_NODE;
                break;
            case SP:
            case DP:
            case MUL:
            case DIV:
                node_type = OP_NODE;
                break;
            default:
                PANIC("Invalid Op type %d", op_type);
                break;
        }
    
    }
    
    virtual ~Node(){};
    
    virtual void print(std::ostream &out) const{
        
        INFO("Node Id=%d, Name=%s, Type = %d, Op = %d, Count=%d, num_prev_ops=%d",
              node_id,    node_name.c_str(), node_type, op_type, count, num_prev_ops);
        
        size_t i = 0;
	/*
        for(auto s : next_ops)
            INFO("dependent_list[%d] = {%d, %d}",
                 i++, s.offset, s.num);
	*/
    }
        
    void set_num_prev_ops(size_t _in){num_prev_ops=_in;}
    
    std::string get_name()const{return node_name;}
    
    size_t get_count()const{return count;}
    
    size_t get_num_prev()const{return num_prev_ops;}
    
    NodeType get_node_type()const{return node_type;}
    
    OpType get_op()const{return op_type; }
        
};


#endif
