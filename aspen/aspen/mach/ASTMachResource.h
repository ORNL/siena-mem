// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_MACH_RESOURCE_H
#define AST_MACH_RESOURCE_H

#include "common/AST.h"
#include "expr/Expression.h"
#include "mach/ASTTraitDefinition.h"

/// example: "resource flops(n) [n/clock] with simd [base/simdwidth]"
class ASTMachResource : public ASTNode
{
  protected:
    const string name;
    const string arg;
    const Expression *quantity;
    const vector<const ASTTraitDefinition*> traits;
  public:
    ASTMachResource(string name,
                    string arg,
                    Expression *quantity,
                    ParseVector<ASTTraitDefinition*> traits)
        : name(name), arg(arg), quantity(quantity), traits(traits.begin(), traits.end())
    {
        
    }
    virtual ~ASTMachResource()
    {
        delete quantity;
        for (unsigned int i=0; i<traits.size(); ++i)
            delete traits[i];
    }
    const ASTTraitDefinition *GetTraitDefinition(const string &tn) const
    {
        for (unsigned int i=0; i<traits.size(); ++i)
        {
            if (traits[i]->GetName() == tn)
                return traits[i];
        }
        return NULL;
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Resource '"<<name<<"' (arg="<<arg<<") quantity=" << endl;
        quantity->Print(out, indent+1);
        if (traits.size() == 0)
            out << Indent(indent+1) << "Traits: none" << endl;
        else
        {
            for (unsigned int i=0; i<traits.size(); ++i)
                traits[i]->Print(out, indent+1);
        }
    }

    const string &GetName() const { return name; }
    const string &GetArg() const { return arg; }
    const Expression *GetQuantity() const { return quantity; }
};

#endif
