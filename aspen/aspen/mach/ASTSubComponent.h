// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_SUB_COMPONENT_H
#define AST_SUB_COMPONENT_H

#include "common/AST.h"

class ASTSubComponent : public ASTNode
{
  protected:
    const string type;
    const string name;
    const Expression *quantity;
  public:
    ASTSubComponent(string type, string name, Expression *quantity)
        : type(type), name(name), quantity(quantity)
    {
    }
    virtual ~ASTSubComponent()
    {
        delete quantity;
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Contains: "
            << type << ": " << name << endl;
        if (quantity)
        {
            out << Indent(indent+1) << "Of quantity: " << endl;
            quantity->Print(out,indent+2);
        }
    }

    const string &GetType() const { return type; }
    const string &GetName() const { return name; }
    const Expression *GetQuantity() const { return quantity; }
};

#endif
