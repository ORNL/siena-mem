// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_TRAIT_DEFINITION_H
#define AST_TRAIT_DEFINITION_H

#include "common/AST.h"
#include "expr/Expression.h"

/// example: "with simd [base/simdwidth]" or "with stride(n) [base*n]"
class ASTTraitDefinition : public ASTNode
{
  protected:
    const string name;
    const string arg;
    const Expression *quantity;
  public:
    ASTTraitDefinition(string name, Expression *quantity) : name(name), arg(""), quantity(quantity) { }
    ASTTraitDefinition(string name, string arg, Expression *quantity) : name(name), arg(arg), quantity(quantity) { }
    virtual ~ASTTraitDefinition()
    {
        delete quantity;
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Trait '" << name << "'";
        if (arg != "")
            out << " (arg="<<arg<<")";
        out << ": quantity=" << endl;
        quantity->Print(out, indent+2);
    }

    const string &GetName() const { return name; }
    const string &GetArg() const { return arg; }
    const Expression *GetQuantity() const { return quantity; }
};

#endif
