// Copyright 2013-2017 UT-Battelle, LLC.  See LICENSE.txt for more information.
// ****************************************************************************
// Class:     ASTMapModel
//
// Purpose:   A model that descries Application-to-Machine mapping
//
// Programmer:  Ivy Bo PENG
// Creation:    Nov, 2017
//
// Modifications:
// ****************************************************************************

#include "model/ASTMapModel.h"
#include "model/ASTAssignStatement.h"
#include "model/ASTEnumDeclaration.h"

void ASTMapModel::Print(ostream &out, int indent) const{
      out << Indent(indent) << "Mapping model "<<name<< ":" << endl;
      out << Indent(indent+1) << "Globals:" << endl;
      for (unsigned int i=0; i<globals.size(); i++)
	{
	  globals[i]->Print(out, indent+2);
	}
      kernelmap->Print(out, indent+2);
      datamap->Print(out, indent+2);
    }

void ASTMapModel::CompleteAndCheck(ASTAppModel  *_app, ASTMachModel *_mach){

    app = _app;
    mach= _mach;
    
    if (!paramMap.Empty()) return;
    
    for (size_t i=0; i<globals.size(); i++)
    {
        ASTAssignStatement *assign = dynamic_cast<ASTAssignStatement*>(globals[i]);
        if (assign)
        {
            paramMap[assign->GetName()] = assign->GetValue();
        }else{
            ASTEnumDeclaration *enumdecl = dynamic_cast<ASTEnumDeclaration*>(globals[i]);
            if (enumdecl)
            {
                for (unsigned int i=0; i<enumdecl->GetNumValues(); ++i)
                {
                    paramMap[enumdecl->GetValue(i)] = new Integer(i);
                }
            }
        }
    }
    
    if(kernelmap) kernelmap->CompleteAndCheck(_app, _mach);
    if(datamap)   datamap->CompleteAndCheck(_app, _mach);
}