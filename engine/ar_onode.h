#ifndef AR_ONODE_H
#define AR_ONODE_H

#include "ar_typedef.h"
#include "ar_node.h"


class OpNode : public Node{

public:
    
    OpNode(OpType _type, std::string _name, size_t _count)
    :
    Node(_type, _name, _count){}
    
    ~OpNode(){}
    
    size_t get_ops()const{return count;}
    
    void print(std::ostream &out=std::cout)const{
        
        Node::print(out);
        
    }
};

#endif
