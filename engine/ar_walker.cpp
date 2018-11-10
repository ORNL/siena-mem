#include "ar_dnode.h"
#include "ar_onode.h"
#include "ar_context.h"
#include "ar_execblock.h"
#include "ar_core.h"
#include "ar_memobj.h"
#include "ar_engine.h"


void Engine::StartKernel(TraversalMode mode, const ASTKernel *k)
{
    const Expression* counter = paramStack->Find("counter");
    if(counter){
        int i= (int)counter->EvaluateWithExpansion(*paramStack);
        INFO("Starting kernel %s, counter=%i\n",(k->GetName()).c_str(),i);
    }
}

void Engine::EndKernel(TraversalMode mode, const ASTKernel *k)
{
  //cout << "\tEnding kernel '"<<k->GetName()<<"'\n";
}

void Engine::StartSeq(TraversalMode mode, const ASTControlSequentialStatement *s)
{
  //cout << Indent(level) << "StartSeq \n";
}

void Engine::EndSeq(TraversalMode, ASTControlSequentialStatement const*)
{
  //cout << Indent(level) << "EndSeq \n";
}

void Engine::StartIterate(TraversalMode mode, const ASTControlIterateStatement *s)
{
    int c = s->GetQuantity()->EvaluateWithExpansion(app->paramMap);
    cout << "Starting iterate "<<s->GetLabel()<<" (count="<<c<<")\n";
    
    is_warmup = false;
}

void Engine::EndIterate(TraversalMode mode, const ASTControlIterateStatement *s)
{
  //cout << "\tEndIterate \n";
}

void Engine::StartMap(TraversalMode mode, const ASTControlMapStatement *s)
{
    double c = s->GetQuantity()->EvaluateWithExpansion(app->paramMap);
    double oldpar = CurrentPar();
    double newpar = oldpar * c;
    PushPar(newpar);
    /*    
#ifdef VERBOSE
    cout << Indent(level) << "StartMap, par="<<CurrentPar()
	 <<",stack size="<<parstack.size()
	 <<", popping "  <<c<<" items and pushing 1\n";
#endif
    */
}

void Engine::EndMap(TraversalMode mode, const ASTControlMapStatement *s)
{
    PopPar();
}

void Engine::StartPar(TraversalMode mode, const ASTControlParallelStatement *s){
  /*
#ifdef VERBOSE
  cout << Indent(level) << "StartPar, stack size="<<parstack.size()
       <<", popping "<<s->GetItems().size()<<" items and pushing 1\n";
#endif
  */
}


void Engine::EndPar(TraversalMode mode, const ASTControlParallelStatement *s){
  /*
#ifdef VERBOSE
  cout << Indent(level) << "EndPar, stack size="<<parstack.size()
       <<", popping "<<s->GetItems().size()<<" items and pushing 1\n";
#endif
  */
}

void Engine::PushPar(double v)
{
    parstack.push_back(v);
}

double Engine::PopPar()
{
    double v = parstack.back();
    parstack.pop_back();
    return v;
}

double Engine::CurrentPar()
{
    return parstack.back();
}

void Engine::Execute(const ASTExecutionBlock *e)
{
    string node_id   = e->GetNodeid();
    string node_name = e->GetLabel();
    
    //Check whether this execution block has been cached
    if( executeblock_map.find(node_id)==executeblock_map.end() ){
        cout << HLINE<<endl;
        cout << "Init Execute Block ["<<node_name<<"] [Id="<< node_id <<"]\n";
        construct_graph(e);
        cout << HLINE<<endl;
    }
    //End parsing exec block
    
    
    //Determine the parallelism for the block
    size_t new_par = (size_t) e->GetParallelism()->EvaluateWithExpansion(*paramStack);
    const size_t nThreads = new_par*CurrentPar();
    
    
    //Check kernel-to-core mapping
    //place to the first core group if not specified
    string core_group;
    if(mapping && mapping->get_kernelmapping(node_name)){
        core_group = mapping->get_kernelmapping(node_name)->GetName();
    }else{
        core_group = machine_cores.begin()->first;
    }
    
    
    cout << HLINE<<endl;
    INFO("Execute Kernel %s on %s [nThreads=%lu, Warm Up=%d]",
         node_name.c_str(), core_group.c_str(),
         //e->GetStatements().size(), 
	 nThreads,is_warmup);
    
    exec_node_longstep(core_group, node_id, nThreads);
    cout << HLINE << endl;
    
    if(!is_warmup) {
        total_bytes += (executeblock_map.at(node_id)).get_total_bytes();
    }
    
}

