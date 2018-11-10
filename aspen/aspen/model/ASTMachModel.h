// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_MACH_MODEL_H
#define AST_MACH_MODEL_H

#include "common/AST.h"
#include "mach/ASTMachComponent.h"
#include "expr/Expression.h"
#include "common/NameMap.h"

// ****************************************************************************
// Class:  ASTMachModel
//
// Purpose:
///   Encapsulates an Aspen Machine Model.
//
// Programmer:  Jeremy Meredith
// Creation:    May 21, 2013
//
// Modifications:
// ****************************************************************************
class ASTMachModel : public ASTNode
{
  public:
    const vector<ASTStatement*> globals;
    const vector<ASTMachComponent*> components;

    //
    // Acceleration structures
    //
    NameMap<const Expression*> paramMap;
    NameMap<const ASTMachComponent*> compMap;
    const ASTMachComponent *machine;
    vector<string> socketlist;
  public:
    ASTMachModel(ParseVector<ASTStatement*> globals,
                 ParseVector<ASTMachComponent*> components);
    virtual ~ASTMachModel();
    virtual void CompleteAndCheck();
    virtual void Print(ostream &out, int indent = 0) const;
    const ASTMachComponent *GetMachine() const;
    const ASTMachComponent* GetSubcomponent(string name) const {
        if( compMap.Contains(name) )
            return compMap[name];
        else
            return NULL;
    }
};

#endif
