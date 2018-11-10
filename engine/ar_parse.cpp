#include "ar_memobj.h"
#include "ar_memalloc.h"
#include "ar_execblock.h"
#include "ar_dnode.h"
#include "ar_onode.h"
#include "ar_core.h"
#include "ar_context.h"
#include "ar_engine.h"
#include "ar_execblock.h"

using namespace std;

void Engine::construct_graph(const ASTExecutionBlock *e){
    
  /*
    ExecBlock block;
    block.global_control = 0;
    block.total_bytes = 0;

    build_nodes(e, &block);
    set_globalcontrol(&block);
    build_execgraph(&block);
    
    executeblock_map[node_id] = block;
  */

    string node_id   = e->GetNodeid();
    string node_name = e->GetLabel();
    assert(executeblock_map.find(node_id)==executeblock_map.end());

    tuple<string, string> value = make_tuple(node_name,node_id);
    pair<unordered_map<string, ExecBlock>::iterator,bool> result = executeblock_map.emplace(piecewise_construct,forward_as_tuple(node_id),value);
    assert(result.second);
    ExecBlock& blk = ((result.first)->second);//executeblock_map[node_id];

    build_nodes(e, blk);
    set_globalcontrol(blk);
    build_execgraph(blk);
    blk.init_offsets();
    
    
#ifdef VERBOSE
    std::vector<DataNode>&  dl = blk.get_datanodes();
    for (auto d : dl) {
        d.print();
    }
    blk.print_group();
#endif
    
}


void Engine::build_nodes(const ASTExecutionBlock *e, ExecBlock& blk){
    
    const vector<ASTExecutionStatement*> stmts = e->GetStatements();
    const size_t nres = stmts.size();
    
    //Iterate all ASTExecutionStatement in the block
    for (size_t i=0; i<nres; ++i){
        
        const ASTExecutionStatement* s = stmts[i];
        
        //check if the statement is of type ASTRequiresStatement
        const ASTRequiresStatement *rs = dynamic_cast<const ASTRequiresStatement*>(s);
        if (!rs ) continue;
        
        string resource = rs->GetResource();
        size_t node_id  = blk.get_num_nodes();
        OpType access_type;
        
        //check if the statement requires computation
        if (resource.compare("flops")==0 ) {
            
            Expression* se = rs->GetCount()->Cloned();
            Expression* aa = se->Expanded(*paramStack);
            size_t flops_count = aa->Evaluate();
            delete(se);
            delete(aa);
            OpNode a(DP, "flops", flops_count);
            blk.add_opnode(a);
            continue;
        }
        else if (resource.compare("loads")==0)
            access_type = LOAD;
        else if( resource.compare("stores")==0)
            access_type = STORE;
        else
            continue;
        
        
        // Start sanity check
        string obj_name = rs->GetToFrom();
        
        if (obj_name.find_first_not_of(" \t")==string::npos)
            THROW(ModelError, "Empty data target for load/store");
        if(!rs->GetCount())
            THROW(ModelError, "Unknown access count for load/store ",obj_name);
        if(memobj_map.find(obj_name) == memobj_map.end())
            THROW(ModelError, "Unknown data object ", obj_name," in globals");
        // End sanity check
        
        
        //build a data node
        ADDR_T  access_size  =0;
        size_t  access_count =0;
        MemAlloc allocobj = memobj_map[obj_name];
        
        //get the access count
        Expression* se = rs->GetCount()->Cloned();
        Expression* aa = se->Expanded(*paramStack);
        access_count = aa->Evaluate();
        delete(se);
        delete(aa);
        if(access_count==0){
            //ignore data object with zero accesses
            continue;
        }
        
        if (!rs->GetSize()){
            //set default access granularity to the element size
            access_size = allocobj.element_size;
        }else{
            se = rs->GetSize()->Cloned();
            aa = se->Expanded(*paramStack);
            access_size = aa->Evaluate();
            delete(se);
            delete(aa);
        }
        DataNode a(allocobj,node_id,access_type,access_count,access_size);
        blk.add_datanode(a);
        blk.inc_bytes(access_count*access_size);
        
        //Analyzing all memory traits
        if(rs->GetNumTraits() > 0){
            parse_memtraits(rs, blk.get_last_datanode() );
        }
        //End of checking traits
        
    }
    //End of all ASTExecutionStatement in the block
    
}


void Engine::parse_memtraits(const ASTRequiresStatement *rs, DataNode& a){

    const size_t num_traits = rs->GetNumTraits();
    const vector<ASTTrait*> trait_list = rs->GetTraits();
    
    for(size_t i=0;i<num_traits;i++){
        
        ASTTrait* trait = trait_list[i];
    
        string trait_name = trait->GetName();
        const Expression *trait_expr = trait->GetValue();
        const vector<Expression *>trait_exprlist = trait->GetValueList();
        
#ifdef DEBUG_PARSE
        INFO("Memory Trait:%s", trait_name.c_str());
#endif
        
        if(trait_name.compare("stride")==0){
            
            a.set_pattern(STRIDE);
            
            //default stride is the element size of the data structure
            ADDR_T stride = a.access_size;
            if(trait_expr){
                Expression *oe = trait_expr->Cloned();
                Expression *oex= oe->Expanded(*paramStack);
                stride = (ADDR_T)(oex->Evaluate());
                delete (oe);
                delete (oex);
            }
            
            a.set_stride(stride);
            
        }
        //aux to stride
        else if(trait_name.compare("offset")==0){
            if(trait_expr){
                Expression *oe = trait_expr->Cloned();
                Expression *oex= oe->Expanded(*paramStack);
                ADDR_T offset = (ADDR_T)(oex->Evaluate());
                a.set_stride_offset(offset);
                delete (oe);
                delete (oex);
            }else
                THROW(ModelError, "The offset trait has no value");
            
        }
        else if(trait_name.compare("random")==0){
            
            a.set_pattern(RANDOM);
            if(trait_expr){
                Expression *oe = trait_expr->Cloned();
                Expression *oex= oe->Expanded(*paramStack);
                size_t random_dist = (size_t)(oex->Evaluate());
                a.set_random_distfunc(random_dist);
                delete (oe);
                delete (oex);
            }
            
        }
        //aux to random
        else if(trait_name.compare("cdf")==0){
            if(trait_expr){
                Expression *oe = trait_expr->Cloned();
                Expression *oex= oe->Expanded(*paramStack);
                ADDR_T cdf = (ADDR_T)(oex->Evaluate());
                a.set_random_distfunc(cdf);
                delete (oe);
                delete (oex);
            }else
                THROW(ModelError, "The cdf trait has no value");
        }
        else if(trait_name.compare("ghostcell")==0){
            a.set_pattern(GHOSTCELL);
            if(trait_expr){//aux to ghostcell
                Expression *oe = trait_expr->Cloned();
                Expression *oex= oe->Expanded(*paramStack);
                size_t bcell = (ADDR_T)(oex->Evaluate());
                a.set_bcell(bcell);
                delete (oe);
                delete (oex);
            }
        }
        else if(trait_name.compare("stencil3p")==0){
            a.set_pattern(STENCIL3P);
        }else if(trait_name.compare("stencil4p")==0){
            a.set_pattern(STENCIL4P);
        }else if(trait_name.compare("stencil5p")==0){
            a.set_pattern(STENCIL5P);
        }else if(trait_name.compare("stencil8p")==0){
            a.set_pattern(STENCIL8P);
        }else if(trait_name.compare("stencil9p")==0){
            a.set_pattern(STENCIL9P);
        }else if(trait_name.compare("stencil26p")==0){
            a.set_pattern(STENCIL26P);
        }else if(trait_name.compare("stencil27p")==0){
            a.set_pattern(STENCIL27P);
        }else if(trait_name.compare("transpose")==0){
            a.set_pattern(TRANSPOSE);
        }
        else if(trait_name.compare("tile")==0){
            a.set_pattern(TILE);
            if(trait_exprlist.size()!=4)
                PANIC("tile trait has an expression [dir, dim0...dimN], %zu found", trait_exprlist.size());
            
            size_t tile_dir  = (size_t)(trait_exprlist[0]->EvaluateWithExpansion(*paramStack));
            size_t tile_dimX = (size_t)(trait_exprlist[1]->EvaluateWithExpansion(*paramStack));
            size_t tile_dimY = (size_t)(trait_exprlist[2]->EvaluateWithExpansion(*paramStack));
            size_t tile_dimZ = (size_t)(trait_exprlist[3]->EvaluateWithExpansion(*paramStack));
            assert(tile_dimX <= a.dim[0]);
            if(a.num_dims>1) assert(tile_dimY <= a.dim[1]);
            if(a.num_dims>2) assert(tile_dimZ <= a.dim[2]);

            if((tile_dimX<=0)||(tile_dimY<=0)||(tile_dimZ<=0))
                PANIC("A tile must have all dimensions > 0 [%i, %i, %i]",
                      tile_dimX, tile_dimY, tile_dimZ);
            
            if(tile_dir>=a.num_dims)
                PANIC("A tile must have a direction (%i) between 0-Ndim",
                      tile_dir);
            
            a.set_tile(tile_dir, tile_dimX, tile_dimY, tile_dimZ);
            
#ifdef DEBUG_PARSE
            INFO("TILE::[%u %u %u %u]", tile_dir, tile_dimX, tile_dimY, tile_dimZ);
#endif
            
        }
        else if(trait_name.compare("conv")==0){
            a.set_pattern(CONV);
            if(trait_exprlist.size()!=2)
                PANIC("conv trait should have 2 dimensions, %zu found", trait_exprlist.size());
            
            int tile_dimX = (int)(trait_exprlist[0]->EvaluateWithExpansion(*paramStack));
            int tile_dimY = (int)(trait_exprlist[1]->EvaluateWithExpansion(*paramStack));
            assert(tile_dimX <= a.dim[0]);
            assert(tile_dimY <= a.dim[1]);
            a.set_tile(0, tile_dimX, tile_dimY,1);
            
#ifdef DEBUG_PARSE
            INFO("CONV::[%u %u]", tile_dimX, tile_dimY);
#endif
            
        }
        else if(trait_name.compare("compose")==0){
            
            a.set_pattern(COMPOSE);
            
            const size_t num_traits = trait_exprlist.size();
            if(num_traits<=1)
                PANIC("compose trait should have at least 2 traits, %zu found", trait_exprlist.size());
            
            for(size_t jj=0; jj<num_traits; jj++){
                cout << "Compose ["<< jj<<"]  ";trait_exprlist[jj]->Print(cout);
                //size_t tile_dir  = (size_t)(trait_exprlist[0]->EvaluateWithExpansion(*paramStack));
            }
            TODO();
            
        }
        else if(trait_name.compare("dep")==0){
            if(trait_expr){
                const Identifier* id = dynamic_cast<const Identifier*>(trait_expr);
                if(id) {
#ifdef DEBUG_PARSE
                    INFO("\tdependency on %s", (id->GetName()).c_str() );
#endif
                    (a.dependency).push_back(id->GetName());
                }
            }
            else if(trait_exprlist.size()>0){
                
                const size_t num_traits = trait_exprlist.size();
                for(size_t i=0;i<num_traits;i++){
                    
                    const Identifier* id = dynamic_cast<const Identifier*>(trait_exprlist[i]);
                    if(!id)
                        THROW(ModelError, "dependency list has ", id->GetName(), " NOT an identifier");
                    
                    if(memobj_map.find(id->GetName())==memobj_map.end())
                        THROW(ModelError, "The dependency ", id->GetName(), " not found in globals");
#ifdef DEBUG_PARSE
                    INFO("\tdependency on %s", (id->GetName()).c_str() );
#endif
                    (a.dependency).push_back(id->GetName());
                }
            }
        }
        else{
            DB("Unknown trait: %s ignored!", trait_name.c_str());
        }
        
    }//End of all memory traits
}


void Engine::set_globalcontrol(ExecBlock& blk){

    //Find global control
    size_t global_control = 0;
    const Expression* gc_exp = paramStack->Find("global_control");
    if(gc_exp){
        Expression* e = gc_exp->Cloned();
        global_control= e->EvaluateWithExpansion(*paramStack);
        delete(e);
    }
    blk.set_global_control(global_control);
    
#ifdef VERBOSE
    INFO("global_control=%u", blk.get_global_control());
#endif
}


void Engine::build_execgraph(ExecBlock& blk){
    
    size_t num_nodes = blk.get_num_nodes();
    size_t global_control = blk.get_global_control();
    
    //Start of iterating all nodes
    size_t pos=0;
    for(size_t i=0; i<num_nodes; i++){
        
        Node* node = blk.get_node(i);
        
        //Check the type
        NodeType node_type = node->get_node_type();
        switch (node_type) {
            case OP_NODE:
            {
                OpNode* b = dynamic_cast<OpNode*>(node);
                if(b){
                    Node* node_next = blk.get_node(i+1);
                    DataNode*  a2   = dynamic_cast<DataNode*>(node_next);
                    if(!a2)
                        THROW(ModelError, "Comp Node [",b->get_name(),"] does not connect to a data node ", node_next->get_name());
                    assert(b->get_ops()%a2->get_accesses() == 0);
                    a2->set_comp(b->get_ops()/a2->get_accesses());
                }
                break;
            }
            case DATA_NODE:
            {
                DataNode* a = dynamic_cast<DataNode*>(node);
                //determine dependency
                if( a->has_dependency() ){
                    size_t num_dep = (a->dependency).size();
                    size_t num_datanodes = blk.get_num_datanodes();
                    
                    //Start of resolving all dependencies
                    for(size_t ii = 0; ii<num_dep; ii++)
                    {
                        string dep_data = a->dependency[ii];
                        bool found = false;
			//here we allow self dependecy
			//if(pos==0) assert(dep_data==my_name);
			assert(pos>0);
                        for(int j=pos-1; j>=0 ; j--)
                        {
                            DataNode& aNode = blk.get_datanode(j);
                            if(dep_data.compare(aNode.get_name())==0 ){
                                (a->prev).push_back(j);
                                aNode.next.push_back( pos );
                                found = true;
                                break;
                            }
                        }
                        if(!found)
                            THROW(ModelError, "Cannot find dependent data ", dep_data);
                    }
                    //End of resolving all dependencies
                }
                
                assert(alloc_mem_map.find(a->get_name())!=alloc_mem_map.end());
                string mem_name = alloc_mem_map[a->get_name()];
                assert(machine_memories.find(mem_name)!=machine_memories.end());
                a->set_mem_ptr( &(machine_memories.at(mem_name)) );
                
                size_t steps = a->access_count/global_control;
                vector<GRPCTRL_T>& group_list = blk.get_nodegroups();
                if( group_list.size()==0 || steps!=group_list.back().group_steps)
                {
		  size_t node_pos = blk.get_node_pos(i);
                    GRPCTRL_T grp = {node_pos, node_pos, 0, steps, 0};
                    group_list.push_back(grp);
                    
                }else{
                    group_list.back().end_node ++;
                }
                a->set_group_id( group_list.size()-1 );
                pos ++;
                break;
            }
            default:
                PANIC("Invalid node type %d", node_type);
                break;
        }
    }
    //End of iterating all nodes

}





