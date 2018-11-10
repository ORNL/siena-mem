
#include "ar_execblock.h"
#include "ar_dnode.h"
#include "ar_onode.h"

using namespace std;

ExecBlock::ExecBlock(const string _name, const string _id)
:
block_name (_name), block_id(_id)
{

    global_control = 0;
    total_bytes = 0;

    INFO("Creating ExecBlock [id=%s,name=%s]", block_id.c_str(), block_name.c_str());
}
  
ExecBlock::~ExecBlock(){

    node_list.clear();
    datanode_list.clear();
    opnode_list.clear();
}

void ExecBlock::print()const{
    
    INFO("ExecBlock [%s, %s] global_control = %lu",
         block_name.c_str(), block_id.c_str(), global_control);
    
}

void ExecBlock::print_group()const{
    
    size_t group_id = 0;
    for (auto& group : nodegroup_list){
        INFO("node_group[%lu]: st_node=%lu, end_node=%lu",
             group_id++, group.st_node, group.end_node);
        
    }
    
}

Node*  ExecBlock::get_node(size_t pos){
    size_t t = node_list[pos].first;
    size_t i = node_list[pos].second;
    if(t==0)
        return &(datanode_list[i]);
    else
        return &(opnode_list[i]);
}

size_t  ExecBlock::get_node_pos(size_t pos){
    return node_list[pos].second;
}

void ExecBlock::add_datanode(DataNode a){
    
    node_list.push_back(make_pair(0,datanode_list.size()));
    datanode_list.push_back(a);
}

void ExecBlock::add_opnode(OpNode a){
    
    node_list.push_back(make_pair(1,opnode_list.size()));
    opnode_list.push_back(a);
}

void ExecBlock::inc_bytes(uint64_t inc_bytes){
    total_bytes += inc_bytes;
}

void ExecBlock::set_global_control(size_t _in){
    
    global_control = _in;
    
    if(global_control==0){
        size_t _out = get_node(0)->get_count();
        size_t i=1;
        size_t num=node_list.size();
        for(;i<num;i++)
            _out = gcd(_out, get_node(i)->get_count() );
        
        global_control = _out;
    }
}

void ExecBlock::init_offsets(){
    
    //one global walk through all groups
    size_t curr_group_id = 0;
    size_t fetched_global_step = 0;
    bool   done = false;
    window_size = 0;
    total_ops   = 0;
    
    while( !done )
    {
        GRPCTRL_T &group  = nodegroup_list[curr_group_id];
        size_t local_step = fetched_global_step*group.group_steps + group.curr_group_step;
        
        for (size_t node_id = group.st_node; node_id <= group.end_node; node_id++) {
            DataNode &node  = datanode_list[node_id];
            vector<ADDR_T> addr_list = node.gen_addr(local_step);
            node.ops_step   = addr_list.size();
            
            //inc total ops for this exec block
            total_ops += node.ops_step*( 1 + node.get_comp() );
            
            if(group.curr_group_step==0){
                node.offset = window_size;
                group.num_ops += node.ops_step;
            }
            
            window_size += node.ops_step;
            
            if(group.curr_group_step==0){
                for (auto prev_node_id : node.prev) {
                    DataNode  &prev_node = datanode_list[prev_node_id];
                    size_t prev_group_id = prev_node.get_group_id();
                    if(prev_group_id==curr_group_id)
                        node.num_prev_ops += prev_node.ops_step;
                    else
                        node.num_prev_ops += prev_node.ops_step * nodegroup_list[prev_group_id].group_steps;
                }
            }
        }
        
        
        //Start stepping forward to the next node
        group.curr_group_step ++;
        if( group.curr_group_step == group.group_steps ){
            group.curr_group_step = 0;
            curr_group_id ++;
            if(  curr_group_id ==  nodegroup_list.size() ){
                curr_group_id = 0;
                done = true;
            }
        }
        //End stepping forward to the next node
    }
    total_ops *= global_control;

    assert( curr_group_id==0 );
}


