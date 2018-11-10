
#ifdef DRAMSIM
#include "ar_dramsim.h"
#endif
#ifdef RAMULATOR
#include "ar_ramulator.h"
#endif
#ifdef NVDIMMSIM
#include "ar_nvdsim.h"
#endif
#include "ar_dnode.h"
#include "ar_execblock.h"
#include "ar_memsim.h"
#include "ar_memobj.h"
#include "ar_cacheobj.h"
#include "ar_memalloc.h"
#include "ar_core.h"
#include "ar_engine.h"

using namespace std;


void Engine::config_core(){
    
    const vector<ASTMachComponent*>& components = mach->components;
    const ASTMachComponent* machine = mach->GetMachine();
    //machine->Print(cout);
    
    //Start of configuring core
    for (size_t ci=0; ci<components.size(); ci++){
        ASTMachComponent* mach_comp = components[ci];
        if(mach_comp->GetType().compare("core")==0)
        {
            string core_name = mach_comp->GetName();
            const ASTSubComponent* core_comp = machine->GetSubComponent(core_name);
            
            //Start of sanity check
            if(!core_comp)
                THROW(ModelError, "Machine does not include Core %s",core_name.c_str());
            
            if(machine_cores.find(core_name)!=machine_cores.end())
                PANIC("Duplicated core name %s", core_name.c_str());
            //End of sanity check
            
            size_t num_cores = 1;
            if(core_comp->GetQuantity())
            {
                Expression* expr = core_comp->GetQuantity()->Cloned();
                num_cores = expr->EvaluateWithExpansion(mach->paramMap);
                delete (expr);
            }
            
            //Check if any connection information is provided
            const vector<const ASTMachConnectivity*> connections = mach_comp->GetConnections();
            for(auto c : connections){
                const vector<string> &values = c->GetValue();
                for(auto v : values){
                    connection_map[core_name] = v;
                }
            }
        
            CoreGroup core_group(core_name, num_cores);

            //Start of iteratiing all core properties
            const vector<const ASTMachProperty*>  props = mach_comp->GetProperties();
            for(size_t ps=0;ps<props.size();ps++){
                
                if(props[ps]->GetName().compare("ROB")==0)
                {
                    const Expression* prop_value =dynamic_cast<const Expression*>(props[ps]->GetValue());
                    if(!prop_value)
                        DB("No ROB specified for Core %s, use default value %d",
                             core_name.c_str(), core_group.get_rob());
                    else
                        core_group.set_rob( (size_t) (prop_value->EvaluateWithExpansion(mach->paramMap)) );
                }
                
                else if(props[ps]->GetName().compare("LFB")==0)
                {
                    const Expression* prop_value =dynamic_cast<const Expression*>(props[ps]->GetValue());
                    if(!prop_value)
                        DB("No LFB specified for Core %s, use default value %d",
                             core_name.c_str(), core_group.get_lfb());
                    else
                        core_group.set_lfb( (size_t)(prop_value->EvaluateWithExpansion(mach->paramMap)) );
                }
                
                else if(props[ps]->GetName().compare("frequency")==0)
                {
                    const Expression* prop_value =dynamic_cast<const Expression*>(props[ps]->GetValue());
                    if(!prop_value)
                        DB("No frequency specified for Core %s, use default value %d", 
			     core_name.c_str(), core_group.get_frequency());
                    else
                        core_group.set_frequency( (size_t)(prop_value->EvaluateWithExpansion(mach->paramMap)) );
                }
                
                else if(props[ps]->GetName().compare("ALU")==0)
                {
                    const Expression* prop_value =dynamic_cast<const Expression*>(props[ps]->GetValue());
                    if(!prop_value)
                        DB("No ALU specified for Core %s, use default value %d", 
			     core_name.c_str(), core_group.get_alu());
                    else
                        core_group.set_alu(  (size_t)(prop_value->EvaluateWithExpansion(mach->paramMap)) );
                }
                
                else if(props[ps]->GetName().compare("LD")==0)
                {
                    const Expression* prop_value =dynamic_cast<const Expression*>(props[ps]->GetValue());
                    if(!prop_value)
                        DB("No LD specified for Core %s, use default value %d", 
			     core_name.c_str(), core_group.get_ld());
                    else
                        core_group.set_ld( (size_t)(prop_value->EvaluateWithExpansion(mach->paramMap)) );
                }
                
                else if(props[ps]->GetName().compare("ST")==0)
                {
                    const Expression* prop_value =dynamic_cast<const Expression*>(props[ps]->GetValue());
                    if(!prop_value)
                        DB("No ST specified for Core %s, use default value %d", 
			     core_name.c_str(), core_group.get_st());
                    else
                        core_group.set_st( (size_t)(prop_value->EvaluateWithExpansion(mach->paramMap)) );
                }
                /*
                else {
                    DB("Unknown property %s for Core %s, ignored!", 
			 (props[ps]->GetName()).c_str(), core_name.c_str());
		}
		*/
                
            }
            //End of all properties
            
            machine_cores.insert(make_pair(core_name, core_group));
        }
    }
    //End of configuring core
    
    if(machine_cores.size()==0)
        PANIC("%i cores are found in the machine model",0);
    
}


void Engine::config_cache(){
    
    const vector<ASTMachComponent*>& components = mach->components;
    const ASTMachComponent* machine = mach->GetMachine();
    
    //Start of configuring cache
    for (size_t ci=0; ci<components.size(); ci++){
        ASTMachComponent* mach_comp = components[ci];
        if(mach_comp->GetType().compare("cache")==0)
        {
            string cache_name = mach_comp->GetName();
            const ASTSubComponent* cache_comp = machine->GetSubComponent(cache_name);
            
            //Start of sanity check
            if(!cache_comp)
                THROW(ModelError, "Machine does not include Cache %s",cache_name.c_str());
            
            if(machine_caches.find(cache_name)!=machine_caches.end())
                PANIC("Duplicated cache name %s", cache_name.c_str());
            //End of sanity check
            
            
            size_t num_caches = 1;
            if(cache_comp->GetQuantity())
            {
                Expression* expr = cache_comp->GetQuantity()->Cloned();
                num_caches = expr->EvaluateWithExpansion(mach->paramMap);
                delete (expr);
            }
            
            //Check if any connection information is provided
            const vector<const ASTMachConnectivity*> connections = mach_comp->GetConnections();
            for(auto c : connections){
                const vector<string> &values = c->GetValue();
                for(auto v : values){
                    connection_map[cache_name] = v;
                }
            }
            
            //default cache config
            uint64_t cache_capacity = 33554432;
            uint64_t cache_line = 64;
            uint64_t cache_associativity = 8;
            uint64_t read_latency=0;
            uint64_t write_latency=0;
            MemSim*  simulator = nullptr;
            uint64_t frequency = 0;
            
            //Start of iterating cache properties
            const vector<const ASTMachProperty*>  props = mach_comp->GetProperties();
            for(size_t ps=0;ps<props.size();ps++){
                
                string prop_name = props[ps]->GetName();
                
                if(prop_name.compare("capacity")==0)
                {
                    const Expression* prop_value =dynamic_cast<const Expression*>(props[ps]->GetValue());
                    if(!prop_value)
                        printf("No capacity specified for Cache %s, use default %lu\n",cache_name.c_str(),cache_capacity);
                    else
                        cache_capacity = (uint64_t) (prop_value->EvaluateWithExpansion(mach->paramMap));
                }
                
                else if(prop_name.compare("linesize")==0)
                {
                    const Expression* prop_value =dynamic_cast<const Expression*>(props[ps]->GetValue());
                    if(!prop_value)
                        printf("No linesize specified for Cache %s, use default %lu\n",cache_name.c_str(),cache_line);
                    else
                        cache_line = (uint64_t) (prop_value->EvaluateWithExpansion(mach->paramMap));
                }
                
                else if(prop_name.compare("associativity")==0)
                {
                    const Expression* prop_value =dynamic_cast<const Expression*>(props[ps]->GetValue());
                    if(!prop_value)
                        printf("No associativity specified for Cache %s, use default %lu\n",cache_name.c_str(),cache_associativity);
                    else
                        cache_associativity = (uint32_t) (prop_value->EvaluateWithExpansion(mach->paramMap));
                }
                
                else if(prop_name.compare("frequency")==0)
                {
                    const Expression* prop_value =dynamic_cast<const Expression*>(props[ps]->GetValue());
                    if(!prop_value)
                        printf("No frequency specified for Cache %s, use default %lu\n",
                               cache_name.c_str(), frequency);
                    else
                        frequency = (uint64_t)(prop_value->EvaluateWithExpansion(mach->paramMap));
                }
                
                //For analytical modeling
                else if(prop_name.compare("read_latency")==0)
                {
                    const Expression* prop_value =dynamic_cast<const Expression*>(props[ps]->GetValue());
                    if(!prop_value)
                        printf("No read_latency specified for Cache %s, use default %f\n",
                               cache_name.c_str(),read_latency);
                    else
                        read_latency = (uint64_t)prop_value->EvaluateWithExpansion(mach->paramMap);
                }
                
                //For analytical modeling
                else if(prop_name.compare("write_latency")==0)
                {
                    const Expression* prop_value =dynamic_cast<const Expression*>(props[ps]->GetValue());
                    if(!prop_value)
                        printf("No write_latency specified for Cache %s, use default %lu\n",
                               cache_name.c_str(),write_latency);
                    else
                        write_latency = (uint64_t)prop_value->EvaluateWithExpansion(mach->paramMap);
                }
                
                //For detailed DRAM timing
#ifdef RAMULATOR
                else if(prop_name.compare("ramulator")==0)
                {
                    const Identifier* prop_value =dynamic_cast<const Identifier*>(props[ps]->GetValue());
                    
                    //sanity check
                    if(!prop_value)
                        THROW(ModelError, "Cache ",cache_name," uses ramulator without config file in property");
                    
                    string pathstr;
                    if(const char* env_var = getenv("RAMULATOR_ROOT")){
                        pathstr.append(env_var);
                    }
                    
                    const string configFilename(pathstr+"/"+prop_value->GetName());
                    const string stats_out("ramulator." +app->GetName()+"."+cache_name+ ".stats");
                    simulator = new ramulator::RamulatorSim(false, configFilename, stats_out);
                }
#endif
                else {
                    DB("Unknown property %s for Cache %s, ignored!\n", prop_name.c_str(), cache_name.c_str());
                }
            }
            //End of iterating cache properties
            
            
            //construct a vector of num_caches CacheObj in-place
            machine_caches[cache_name].reserve(num_caches);
            
            for(size_t ci=0; ci<num_caches; ci++){
                stringstream ss;
                ss << cache_name << "-" << ci;
                string cache_name_i = ss.str();

                machine_caches[cache_name].emplace_back(cache_name_i, cache_capacity, cache_line, cache_associativity,this);
                CacheObj &obj = machine_caches[cache_name].back();
                obj.set_read_latency( read_latency);
                obj.set_write_latency(write_latency);
                obj.set_frequency(frequency);

#ifdef RAMULATOR
                if(simulator){
                    if(ci>0){
                        const string stats_out("ramulator."+app->GetName()+"."+cache_name_i+ ".stats");
                        obj.set_simulator(new ramulator::RamulatorSim(simulator, stats_out));
                    }else
                        obj.set_simulator(simulator);
                }
#endif
            }
            has_caches = true;
        }
    }
    //End of configuring cache
    
}


void Engine::config_mem(){
    
    const vector<ASTMachComponent*>& components = mach->components;
    
    //Start of configuring memory
    for (size_t ci=0; ci<components.size(); ci++){
        ASTMachComponent* mach_comp = components[ci];
        if(mach_comp->GetType().compare("memory")==0)
        {
            string mem_name = mach_comp->GetName();
            if(machine_memories.find(mem_name)!=machine_memories.end())
                THROW(ModelError, "Memory ", mem_name, " already exists. Assign different names to each memory component in the machine model.");
            
            //large capacity with multiple dramsim2 instances can cause stack overflow
            //default 20 memory clock cycles
            uint64_t mem_capacity = 16777216;
            uint64_t mem_frequency= 1E9;
            uint64_t read_latency  = 20;
            uint64_t write_latency = 20;
            uint64_t access_granularity = 64;
            MemSim*  simulator = nullptr;
            
            //Start of iterating memory properties
            const vector<const ASTMachProperty*>  props = mach_comp->GetProperties();
            for(size_t ps=0;ps<props.size();ps++){
                
                string prop_name = props[ps]->GetName();
                
                if(prop_name.compare("capacity")==0)
                {
                    const Expression* prop_value =dynamic_cast<const Expression*>(props[ps]->GetValue());
                    if(!prop_value)
                        printf("No capacity specified for Memory %s, use default %lu%\n",
                               mem_name.c_str(), mem_capacity);
                    else
                        mem_capacity = (uint64_t)prop_value->EvaluateWithExpansion(mach->paramMap);
                }
                
                else if(prop_name.compare("frequency")==0)
                {
                    const Expression* prop_value =dynamic_cast<const Expression*>(props[ps]->GetValue());
                    if(!prop_value)
                        printf("No frequency specified for Memory %s, use default %lu\n",
                               mem_name.c_str(), mem_frequency);
                    else
                        mem_frequency = (uint64_t)(prop_value->EvaluateWithExpansion(mach->paramMap));
                }
                //For analytical model
                else if(prop_name.compare("read_latency")==0)
                {
                    const Expression* prop_value =dynamic_cast<const Expression*>(props[ps]->GetValue());
                    if(!prop_value)
                        printf("No read_latency specified for Memory %s, use default [%lu] memory cycles\n",
                               mem_name.c_str(),read_latency);
                    else
                        read_latency = (uint64_t)prop_value->EvaluateWithExpansion(mach->paramMap);
                }
                //For analytical model
                else if(prop_name.compare("write_latency")==0)
                {
                    const Expression* prop_value =dynamic_cast<const Expression*>(props[ps]->GetValue());
                    if(!prop_value)
                        printf("No write_latency specified for Memory %s, use default [%lu] memory cycles\n",
                               mem_name.c_str(),write_latency);
                    else
                        write_latency = (uint64_t)prop_value->EvaluateWithExpansion(mach->paramMap);
                }
                //For analytical model
                else if(prop_name.compare("granularity")==0)
                {
                    const Expression* prop_value =dynamic_cast<const Expression*>(props[ps]->GetValue());
                    if(!prop_value)
                        printf("No granularity specified for Memory %s, use default [%lu]\n",
                               mem_name.c_str(), access_granularity);
                    else
                        access_granularity = (uint64_t)prop_value->EvaluateWithExpansion(mach->paramMap);
                }
#ifdef DRAMSIM
                else if(prop_name.compare("dramsim2")==0)
                {
                    const Identifier* prop_value =dynamic_cast<const Identifier*>(props[ps]->GetValue());
                    
                    //sanity check
                    if(!prop_value)
                        THROW(ModelError, "Memory ", mem_name, " uses dramsim2 without device.ini file in property");
                    
                    string pwdString;
                    if(const char* env_var = getenv("DRAMSIM_ROOT")){
                        pwdString.append(env_var);
                    }
                    
                    string systemIniFilename("system.ini");
                    string traceFileName(mem_name);
                    string deviceIniFilename(prop_value->GetName());
                    
                    //DRAMSim use global varial for config, different instances will still have the same configuration
                    simulator=new DramSim(mem_name,machine_memories.size(),true,deviceIniFilename, systemIniFilename, pwdString, traceFileName);
                    
#ifdef VERBOSE
                    cout << "Memory "<< mem_name << " uses DRAMSim2 simulator "<<endl;
                    cout << "dramsim2 systemIni: "<< systemIniFilename << '\n';
                    cout << "dramsim2 deviceIni: "<< deviceIniFilename << '\n';
#endif
                }
#endif
                
#ifdef RAMULATOR
                else if(prop_name.compare("ramulator")==0)
                {
                    const Identifier* prop_value =dynamic_cast<const Identifier*>(props[ps]->GetValue());
                    
                    //sanity check
                    if(!prop_value)
                        THROW(ModelError, "Memory ", mem_name, " uses ramulator without config file in property");
                    
                    string pathstr;
                    if(const char* env_var = getenv("RAMULATOR_ROOT")){
                        pathstr.append(env_var);
                    }
                    
                    const string configFilename(pathstr+"/"+prop_value->GetName());
                    const string stats_out("ramulator." +app->GetName()+"."+mem_name+ ".stats");
                    
                    simulator=new ramulator::RamulatorSim(true,configFilename,stats_out);
                }
#endif
                
#ifdef NVDIMMSIM
                else if(prop_name.compare("nvdimmsim")==0)
                {
                    const Identifier* prop_value =dynamic_cast<const Identifier*>(props[ps]->GetValue());
                    
                    //sanity check
                    if(!prop_value)
                        THROW(ModelError, "Memory ", mem_name, " uses nvdimmsim without device.ini file in property");
                    
                    string pathstr;
                    if(const char* env_var = getenv("NVDIMMSIM_ROOT")){
                        pathstr.append(env_var);
                    }
                    string pwdString(pathstr);
                    string systemIniFilename("nvm_system.ini");
                    string deviceIniFilename(prop_value->GetName());
                    string traceFileName(mem_name);
                    
                    simulator = new NVMSim(mem_name, machine_memories.size(), true, deviceIniFilename, systemIniFilename, pwdString, traceFileName);
                }
#endif
		/*
                else {
                    DB("Unknown property %s for Memory %s, ignored!", prop_name.c_str(), mem_name.c_str());
                }
		*/
            }
            //End of iterating memory properties
            
            //construct MemObj in-place
            tuple<string, uint64_t, uint64_t> value = make_tuple(mem_name, mem_capacity, access_granularity);
            pair<unordered_map<string, MemObj>::iterator,bool> result = machine_memories.emplace(piecewise_construct,forward_as_tuple(mem_name),value);
            assert(result.second);
            MemObj &obj = (result.first)->second;
            obj.set_read_latency( read_latency);
            obj.set_write_latency(write_latency);
            obj.set_frequency(mem_frequency);
            if(simulator)
                obj.set_simulator(simulator);
            
        }
    }
    //End of configuring memory
    
    if(machine_memories.size()==0)
        PANIC("%i memories are found in the machine model",0);
    
}


void Engine::connect_components(){
    
    for (unordered_map<string, CoreGroup>::iterator it = machine_cores.begin(); it != machine_cores.end(); ++it)
    {
        string     core_name  = it->first;
        CoreGroup &core_group = it->second;
        
        if(connection_map.find(core_name)==connection_map.end()){
            if(machine_caches.size()>0)
                core_group.connect_cache(machine_caches.at(0));
            else{
                //core_group.connect_mem();
                TODO();
            }
        }else{
            string next = connection_map[core_name];
            assert(machine_caches.find(next)!=machine_caches.end());
            core_group.connect_cache(machine_caches[next]);
        }
    }
    
    for (unordered_map<string, vector<CacheObj>>::iterator it = machine_caches.begin(); it != machine_caches.end(); ++it)
    {
        string           cache_name  = it->first;
        vector<CacheObj> &cache_list = it->second;
        
        if(connection_map.find(cache_name)==connection_map.end()){
	   /*INFO("%s is LLC",cache_name.c_str());
            
            size_t num_mem = machine_memories.size();
            assert(num_mem>0);
            if(num_mem>1){
                TODO();
            }else{
                for (auto &cache : cache_list) {
                    cache.set_nextlevel(&(machine_memories.begin()->second));
                }
            }*/
        }else{
            string next = connection_map[cache_name];
            assert(machine_caches.find(next) != machine_caches.end());
            vector<CacheObj> &next_cache_list = machine_caches[next];
            
            size_t num_cache0 = cache_list.size();
            size_t num_cache1 = next_cache_list.size();
            assert(num_cache0>=num_cache1);
            assert(num_cache0%num_cache1==0);
            size_t stride = num_cache0/num_cache1;
            size_t c_id=0;
            for (auto &cache : cache_list) {
                cache.set_nextlevel(&(next_cache_list[c_id/stride]));
                c_id ++;
            }
        }
    }
    
    
    
    for (auto &it : machine_caches)
    {
        vector<CacheObj> &cache_list = it.second;
        for (auto &cache : cache_list) {
            if(cache.is_LLC()){
                for (auto &kv : machine_memories) {
                    MemObj& mem=kv.second;
                    mem.add_prevlevel(&cache);
                }
                
            }
        }
    }
}


void Engine::config_machine(){
    
    config_core();

    config_cache();
    
    config_mem();

    connect_components();
    
    
#ifdef VERBOSE
    for (unordered_map<string, CoreGroup>::iterator it = machine_cores.begin(); it != machine_cores.end(); ++it)
    {
        (it->second).print();
    }
    
    for (unordered_map<string, vector<CacheObj>>::iterator it = machine_caches.begin(); it != machine_caches.end(); ++it)
    {
        string cache_name = it->first;
        vector<CacheObj>& cache_list = it->second;
        INFO("Cache %s[%d]::", cache_name.c_str(), cache_list.size());
        cache_list[0].print();
    }
    
    for (unordered_map<string, MemObj>::iterator it = machine_memories.begin(); it != machine_memories.end(); ++it)
    {
        (it->second).print();
    }
    
    cout <<HLINE<<endl;
    cout <<"Topology"<<endl<<endl;
    for (unordered_map<string, CoreGroup>::iterator it = machine_cores.begin(); it != machine_cores.end(); ++it)
    {
        (it->second).print_connection();
        cout<<endl;
    }
    
#endif
    
}


void Engine::init_data_placement(){
    
    //Start of allocating global data structures
    vector<ASTStatement*> globals = app->GetGlobals();
    for(size_t gs=0; gs<globals.size(); gs++)
    {
        ASTStatement* s = globals[gs];
        //s->Print(cout);
        
        //find data structures
        const ASTDataStatement *ds = dynamic_cast<const ASTDataStatement*>(s);
        if( ds )
        {
            MemAlloc a;
            a.name = ds->GetName();
            a.mid  = memobj_map.size();
            if(ds->GetQuantity())
            {
                Expression* expr = ds->GetQuantity()->Cloned();
                a.size = expr->EvaluateWithExpansion(app->paramMap);
                a.element_size = 1;
                a.num_dims = 1;
                a.dim.push_back(a.size);
                delete (expr);
            }
            else if(ds->GetAs()){
                //Option 0: "data mat as Matrix(n,n,wordsize)"
                //Option 1: "data arr as Array(n,wordsize)"
                const FunctionCall *as = ds->GetAs();
                size_t argc = as->arguments.size();
                
                //sanity check
                if(as->name.compare("Matrix")!=0 && as->name.compare("Array")!=0)
                    THROW(ModelError, "Data ", a.name, " is neither matrix nor array");
                if( argc < 2)
                    THROW(ModelError,"Data ", a.name, " dimension < 1 ");
                
                a.size = 1;
                a.num_dims = argc - 1;
                a.dim.reserve(a.num_dims);
                for(size_t ai=0;ai<argc;ai++)
                {
                    Expression* ase = as->arguments[ai]->Cloned();
                    double value = ase->EvaluateWithExpansion(app->paramMap);
                    delete(ase);
                    a.size *= (ADDR_T)value;
                    if(ai < (argc-1)){
                        a.dim.push_back((size_t)value);
                    }else{
                        a.element_size = (ADDR_T)value;
                    }
                }
            }
            else {
                PANIC("Wrong Data Construct: %s",a.name.c_str());
            }
            
            a.element_num = a.size/a.element_size;
            
            
            //Check data-to-memory mapping
            //place to the default memory if not specified
            string mem_type;
            if(mapping && mapping->get_datamapping(a.name)){
                mem_type = mapping->get_datamapping(a.name)->GetName();
            }else{
                mem_type = machine_memories.begin()->first;
            }
            
            unordered_map<string,MemObj>::iterator it = machine_memories.find(mem_type);
            if(it==machine_memories.end())
                PANIC("data %s is mapped to %s, which is not provided in the machine model",
                      a.name.c_str(), mem_type.c_str());
            
            if( !((it->second).place_alloc(a)) )
                PANIC("MemObj %s ( %lu bytes) cannot be placed on Memory %s",
                      a.name.c_str(), a.size, (it->second).name.c_str());

            //Allocate address space to memory objects
            /*Option I
            bool hasOverlap = true;
            while(hasOverlap){
              a.addr_st  = ((ADDR_T)rand())/((ADDR_T)CACHELINE_BYTES)*((ADDR_T)CACHELINE_BYTES);
              a.addr_end = a.addr_st + a.size;
              hasOverlap = false;
              for(auto kv : memobj_map) {
                if( a.addr_st > (kv.second).addr_end || a.addr_end < (kv.second).addr_st )
                {}
                else{
                  hasOverlap = true;
                  break;
                }
              }
            }
            */
            //Option II
            a.addr_st  = free_addr;
            a.addr_end = a.addr_st + a.size;
            free_addr +=  (a.size-1)/CACHELINE_BYTES*CACHELINE_BYTES+CACHELINE_BYTES;

            alloc_mem_map[a.name] = (it->second).name;
            memobj_map.insert(make_pair(a.name, a));
            
        }
    }
    //End of allocating global data structures
    
    //for fast lookup
    if(machine_memories.size()==1){
        has_hms = false;
        MemObj& mem = machine_memories.begin()->second;
	default_mem_ptr = &mem;
        if(mem.use_simulator())
            default_memsim_ptr = mem.get_simulator();
        else
            default_memsim_ptr = nullptr;
    }else{
        has_hms = true;
        default_memsim_ptr = nullptr;
	default_mem_ptr    = nullptr;
        for (auto kv : memobj_map) {
            MemAlloc a  = kv.second;
	    ALLOC_T t={a.addr_st, a.addr_end};

            string mem_name = alloc_mem_map[a.name];
            MemObj& mem = machine_memories.at(mem_name);

	    mem_alloc_map[&mem].push_back(t);

            if(mem.use_simulator()){
                MemSim* ptr = mem.get_simulator();
                memsim_alloc_map[ptr].push_back(t);
            }
        }
    }
    
#ifdef VERBOSE
    for (unordered_map<string, MemAlloc>::iterator it=memobj_map.begin(); it!=memobj_map.end(); ++it) {
        
        MemAlloc a = it->second;
        
        INFO("\t%s::\n\t[%d] elements of size [%d]\n\t[0x%lx - 0x%lx]\n\tmapped to %s",
             a.name.c_str(),
             a.element_num, a.element_size,
             a.addr_st, a.addr_end,
             alloc_mem_map[a.name].c_str());
    }

#endif
}


void Engine::init_kernel_placement(){

    const vector<ASTKernel*> &kernel_list = app->GetKernels();
    
    for ( auto k: kernel_list ) {
        string kernel_name = k->GetName();
        string core_type;
        if(mapping && mapping->get_kernelmapping(kernel_name)){
            core_type = mapping->get_kernelmapping(kernel_name)->GetName();
        }else{
            core_type = machine_cores.begin()->first;
        }
        
        cout <<"\t"<<std::setw(8)<<std::left<<kernel_name<<std::setw(6)<<" <--> "<<std::setw(8)<<core_type<< endl;
    }

}


void Engine::init_simulator(){
    
    //Init Memory Simulators if any
    //MEM before CACHE
    bool has_memsim = false;
    for (unordered_map<string, MemObj>::iterator it=machine_memories.begin(); it!=machine_memories.end();++it) {
        MemObj &mem = it->second;
        //only init mem with allocation to avoid slow down
        if( mem.use_simulator() && mem.get_num_memobj()>0 ){
            mem.init_simulator();
            memsim_ptr_list.push_back(mem.get_simulator());
            has_memsim = true;
        }
    }
    for (unordered_map<string, vector<CacheObj>>::iterator it=machine_caches.begin(); it!=machine_caches.end();++it) {
        vector<CacheObj> &caches = it->second;
        for (auto &cache : caches) {
            cache.set_has_memsim(has_memsim);
            if( cache.use_simulator() ){
                cache.init_simulator();
                memsim_ptr_list.push_back(cache.get_simulator());
            }
        }
    }
}


void Engine::sync_ticks(CoreGroup &core_group){

    size_t core_frequency = core_group.get_frequency();
    
    for (unordered_map<string, vector<CacheObj>>::iterator it = machine_caches.begin(); it != machine_caches.end(); ++it)
    {
        vector<CacheObj>& cache_list = it->second;
        for (auto &cache : cache_list)
            cache.sync_core(core_frequency);
    }
    
    
    for (unordered_map<string, MemObj>::iterator it=machine_memories.begin(); it!=machine_memories.end();++it) {
        
        MemObj &mem = it->second;
        mem.sync_core(core_frequency);
    }
    
}


void Engine::init(){
    
    cpu_cycles = 0;
    free_addr  = 4096;//skip the first page
    total_bytes= 0;
    has_caches = false;
    is_warmup  = true;
    
    cout << HLINE<<endl;
    cout << "Configure Machine Model\n\n";
    config_machine();
    cout << HLINE<<endl;
    
    cout << HLINE<<endl;
    cout << "Map Memory Objects\n\n";
    init_data_placement();
    cout << HLINE<<endl;
    
    cout << HLINE<<endl;
    cout << "Map Kernels\n\n";
    init_kernel_placement();
    cout << HLINE<<endl;
    
    cout << HLINE<<endl;
    cout << "Init Simulators\n\n";
    init_simulator();
    cout << HLINE<<endl;
    
    
}


void Engine::end_cachesim(){
    

    for (unordered_map<string, vector<CacheObj>>::iterator it = machine_caches.begin(); it != machine_caches.end(); ++it)
    {
        vector<CacheObj>& cache_list = it->second;
        for (auto &cache : cache_list){
            cache.fini_simulator();
        }
    }
        
}


void Engine::end_memsim(){
    
    for (unordered_map<string, MemObj>::iterator it=machine_memories.begin(); it!=machine_memories.end(); it++)
    {
        (it->second).fini_simulator();
    }
}


void Engine::fini(){

    cout << "Finalizing...\n";
    
    end_cachesim();
    
    end_memsim();
    
    print_final_stats();
    
}

