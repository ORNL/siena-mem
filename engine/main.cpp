#include "ar_baseobj.h"
#include "ar_memobj.h"
#include "ar_memsim.h"
#include "ar_core.h"
#include "ar_node.h"
#include "ar_dnode.h"
#include "ar_execblock.h"
#include "ar_engine.h"
#include "parser/Parser.h"
#include <sys/time.h>

#include <exception>
#include <signal.h>
#include <stdlib.h>

std::string BaseObj::memsys_str("");
size_t BaseObj::num_instances = 0;
size_t Node::num_instances = 0;
size_t MemSim::num_instances=0;
AspenTool *engine;

void sig_to_exception(int s)
{
  throw "CaughtInterrupt";
}

int main(int argc, char **argv)
{
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = sig_to_exception;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);
    
    try
    {
        if (argc!=3 && argc!=4)
        {
            cerr << "Usage: "<<argv[0]<<" app.aspen  mach.aspen  [mapping.aspen]" << endl;
            return -1;
        }
	
        ASTAppModel *app;
        ASTMachModel*mach;
        ASTMapModel *mapping;
        
        if(argc > 3){
            bool success = LoadAppandMachandMapModel(argv[1], argv[2], argv[3], app, mach, mapping);
            if(!success)
                THROW(ModelError, "Failed LoadAppandMachandMapModel");
        }else{
            app = LoadAppModel(argv[1]);
            mach= LoadMachineModel(argv[2]);
            mapping = NULL;
            if(!app || !mach)
                THROW(ModelError, "Failed loading application or machine models");
        }
	    
        engine= new Engine(app,mach,mapping);

#if false
        app->Print(cout);
        mach->Print(cout);
        if(mapping) mapping->Print(cout);
#endif
        
        //Initialize the engine
        engine->InitializeTraversal();
        
        
#ifdef PROFILING
        struct timeval time_st;
        gettimeofday(&time_st,NULL);
        double time_sec= (double)time_st.tv_sec+(double)time_st.tv_usec*.000001;
#endif
        
        
        //start execution from the main kernel
        app->kernelMap["main"]->Traverse(engine);


#ifdef PROFILING
        gettimeofday(&time_st,NULL);
        time_sec= (double)time_st.tv_sec+(double)time_st.tv_usec*.000001 - time_sec;
        INFO(" \nProfiling time: %f", time_sec);
#endif
    
        //free all resources
        engine-> FinalizeTraversal();
        
        delete(engine);
        
    }
    catch (const AspenException &exc)
    {
        cerr << exc.PrettyString() << endl;
        return -1;
    }
    //catch(InterruptException& e)
    catch(const char*)  
    {
        std::cout << "Dumping state " << std::endl;
	((Engine*)engine)->dump_state();
        return -1;
    }
    catch (std::exception& e) {
      std::cerr << "standard exeception\n";
      std::terminate();
    }
}




