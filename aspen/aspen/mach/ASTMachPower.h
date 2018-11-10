// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_MACH_POWER_H
#define AST_MACH_POWER_H

#include "common/AST.h"
#include "expr/Expression.h"

/// example: "dynamic power [tdp-idle]" or "static power [30]"
class ASTMachPower : public ASTNode
{
  protected:
    const Expression *staticPower;
    const Expression *dynamicPower;
  public:
    ASTMachPower(Expression *staticPower = NULL, Expression *dynamicPower = NULL)
        : staticPower(staticPower), dynamicPower(dynamicPower)
    {
    }
    virtual ~ASTMachPower()
    {
        delete staticPower;
        delete dynamicPower;
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Static power: " << (!staticPower ? "undefined":"") << endl;
        if (staticPower)
            staticPower->Print(out, indent+1);
        out << Indent(indent) << "Dynamic power: " << (!dynamicPower ? "undefined":"") << endl;
        if (dynamicPower)
            dynamicPower->Print(out, indent+1);
    }
};

#endif
