// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_MACH_PROPERTY_H
#define AST_MACH_PROPERTY_H

#include "common/AST.h"
#include "expr/Expression.h"

/// example: "property capacity [18*giga]"
/// example: "property shared"
class ASTMachProperty : public ASTNode
{
  protected:
    const string name;
    const Expression *value;
  public:
    ASTMachProperty(string name, Expression *value) : name(name), value(value)
    {
    }
    virtual ~ASTMachProperty()
    {
        delete value;
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Property '"<<name<<"'="<< endl;
        if (value)
            value->Print(out, indent+1);
    }

    const string &GetName() const { return name; }
    const Expression *GetValue() const { return value; }
};

#endif
