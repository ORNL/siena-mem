// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_DATA_STATEMENT_H
#define AST_DATA_STATEMENT_H

#include "model/ASTAssignStatement.h"

class ASTDataStatement : public ASTStatement
{
  protected:
    string name;
    Expression *quantity;
    FunctionCall *as;
  public:
    ASTDataStatement(const string &n, Expression *q, FunctionCall *a)
        : name(n), quantity(q), as(a)
    {
    }
    virtual ~ASTDataStatement()
    {
        delete quantity;
        delete as;
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Data "<<name<<":"<<endl;
        if (quantity)
        {
            out << Indent(indent+1) << "Quantity:"<<endl;
            quantity->Print(out, indent+2);
        }
        if (as)
        {
            out << Indent(indent+1) << "As:"<<endl;
            as->Print(out, indent+2);
        }
    }
    virtual void Export(ostream &out, int indent = 0) const
    {
        if (quantity)
        {
            out << Indent(indent) << "data "<<name<<" ["<<quantity->GetText(Expression::ASPEN)<<"]" << endl;
        }
        if (as)
        {
            out << Indent(indent) << "data "<<name<<" as "<<as->GetText(Expression::ASPEN) << endl;
        }
    }
    virtual void Visit(ModelVisitor *visitor)
    {
        if (visitor->Visit(this))
            return;
        if (quantity)
            quantity->Visit(visitor);
        if (as)
            as->Visit(visitor);
    }

    Expression *GetSizeInBytes() const
    {
        if (quantity)
            return quantity->Cloned();

        if (as)
        {
            ///\todo: This is simplistic; we just assume that
            /// an array or matrix or whatever is just a list
            /// of expressions that get multiplied together.
            /// e.g. "data mat as Matrix(n,n,wordsize)"
            /// has a data set size of n*n*wordsize, and
            /// "Array(n,wordsize)" as a size of n*wordsize.
            Expression *expr = NULL;
            for (unsigned int i=0; i<as->arguments.size(); ++i)
            {
                if (expr)
                    expr = new BinaryExpr("*", expr,
                                          as->arguments[i]->Cloned());
                else
                    expr = as->arguments[i]->Cloned();
            }
            return expr;
        }

        THROW(ModelError, "Data statement needs a quantity or expression for size");
    }

    const string &GetName() const { return name; }
    const Expression *GetQuantity() const { return quantity; }
    const FunctionCall *GetAs() const { return as; }
};

#endif
