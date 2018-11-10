// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_ASSIGN_RANGE_STATEMENT_H
#define AST_ASSIGN_RANGE_STATEMENT_H

#include "model/ASTAssignStatement.h"

class ASTAssignRangeStatement : public ASTAssignStatement
{
  protected:
    Expression *minval;
    Expression *maxval;
  public:
    ASTAssignRangeStatement(Identifier *n, Expression *v,
                            Expression *minval, Expression *maxval)
        : ASTAssignStatement(n, v), minval(minval), maxval(maxval)
    {
    }
    virtual ~ASTAssignRangeStatement()
    {
        delete minval;
        delete maxval;
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Identifier "<<ident->GetName()<< " = " << endl;
        out << Indent(indent+1) << "Range:" << endl;
        out << Indent(indent+2) << "Default value:" << endl;
        value->Print(out, indent+3);
        out << Indent(indent+2) << "Minimum value:" << endl;
        minval->Print(out, indent+3);
        out << Indent(indent+2) << "Maximum value:" << endl;
        maxval->Print(out, indent+3);
    }
    virtual void Export(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "param "<<ident->GetName()<< " = "
            << value->GetText(Expression::ASPEN) << " in "
            << minval->GetText(Expression::ASPEN) << " .. "
            << maxval->GetText(Expression::ASPEN) << endl;
    }
    virtual void Visit(ModelVisitor *visitor)
    {
        ASTAssignStatement::Visit(visitor);
        minval->Visit(visitor);
        maxval->Visit(visitor);
    }

    const Expression *GetMinVal() const { return minval; }
    const Expression *GetMaxVal() const { return maxval; }
};


#endif
