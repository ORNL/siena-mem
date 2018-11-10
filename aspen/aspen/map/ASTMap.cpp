// Copyright 2013-2017 UT-Battelle, LLC.  See LICENSE.txt for more information.
// ****************************************************************************
// Class:     ASTMap
//
// Purpose:   Map Application Data/Kernel to Machine Components
//
// Programmer:  Ivy Bo PENG
// Creation:    Nov, 2017
//
// Modifications:
// ****************************************************************************
 
#include "ASTMap.h"
#include "model/ASTDataStatement.h"

ASTMap::ASTMap(int _type, ParseVector<ASTMapStatement*> _mapstmts)
   :mapstmts(_mapstmts.begin(), _mapstmts.end()){

	switch(_type){
	case(0): 
	  type = MapType::KERNEL;
	  map_name = "Kernel Map";
	  break;
	case(1): 
	  type = MapType::DATA;
	  map_name = "Data Map";
	  break;
	default:
	  THROW(ModelError, "Invalid Mapping Type");
	}	
}

void ASTMap::CompleteAndCheck(ASTAppModel  *_app, ASTMachModel *_mach){
    
    app = _app;
    mach= _mach;
    
    if(type == MapType::DATA){
    
        NameMap<const ASTDataStatement*> dsMap;
        
        const vector<ASTStatement*> globals = app->GetGlobals();
        for (size_t i=0; i<globals.size(); i++)
        {
            const ASTStatement *s = globals[i];
            const ASTDataStatement *ds = dynamic_cast<const ASTDataStatement*>(s);
            if (!ds) continue;
            
            dsMap[ds->GetName()] = ds;
        }
        
        for (size_t i=0; i<mapstmts.size(); i++)
        {
            const ASTMapStatement *s = mapstmts[i];
            string d = (s->app_comp)->GetName();
            string m = (s->mach_comp)->GetName();
            
            if( !dsMap.Contains(d))
                THROW(ModelError, "Data ", d, " in mapping model is not found in app model");
            
            if( datakernelMap.Contains(d))
                THROW(ModelError, "Data ", d, " in mapping model is mapped more than once");
                
            const ASTMachComponent* mach_comp = mach->GetSubcomponent(m);
            if( mach_comp == NULL)
                THROW(ModelError, "Data ", d, " mapped to ", m, " not found in mach model");
            
            datakernelMap[d] = mach_comp;
            //cout << "Mapping::Data " << d << " to " << m << endl;
        }
        
    }else if(type == MapType::KERNEL){
        
        NameMap<const ASTKernel*> kMap;

        const vector<ASTKernel*> &kernel_list = app->GetKernels();
        
        for (size_t i=0; i<kernel_list.size(); i++)
        {
            const ASTKernel* k = kernel_list[i];
            kMap[k->GetName()] = k;
        }
        
        for (size_t i=0; i<mapstmts.size(); i++)
        {
            const ASTMapStatement *s = mapstmts[i];
            string d = (s->app_comp)->GetName();
            string m = (s->mach_comp)->GetName();
            
            if( !kMap.Contains(d))
                THROW(ModelError, "Kernel ", d, " in mapping model is not found in app model");
            
            if( datakernelMap.Contains(d))
                THROW(ModelError, "Kernel ", d, " in mapping model is mapped more than once");
            
            const ASTMachComponent* mach_comp = mach->GetSubcomponent(m);
            if( mach_comp == NULL)
                THROW(ModelError, "Kernel ", d, " mapped to ", m, " not found in mach model");
            
            datakernelMap[d] = mach_comp;
            //cout << "Mapping::Kernel" << d << " to " << m << endl;
        }
    }
    
    
}


void ASTMap::Print(ostream &out, int indent) const{

  if(type==DATA)
    out << Indent(indent) << "Place Data: "<< endl;
  else if(type==KERNEL)
    out << Indent(indent) << "Execute Kernels: "<< endl;

  for (size_t i=0; i<mapstmts.size(); i++)
    {
      mapstmts[i]->Print(out, indent+1);
    }
}

