// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_ASSIGN_ENUM_STATEMENT_H
#define AST_ASSIGN_ENUM_STATEMENT_H

#include "model/ASTAssignStatement.h"

class ASTAssignEnumStatement : public ASTAssignStatement
{
  protected:
    string enumtype;
  public:
    ASTAssignEnumStatement(Identifier *n, Expression *v, string et)
        : ASTAssignStatement(n, v), enumtype(et)
    {
    }
    virtual ~ASTAssignEnumStatement()
    {
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Identifier "<<ident->GetName()<< " = " << endl;
        out << Indent(indent+2) << "Enum type: " << enumtype << endl;
        out << Indent(indent+2) << "Default value:" << endl;
        value->Print(out, indent+3);
    }
    virtual void Export(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "param "<<ident->GetName()<< " = "
            << value->GetText(Expression::ASPEN) << " in "
            << enumtype << endl;
    }
    virtual void Visit(ModelVisitor *visitor)
    {
        ASTAssignStatement::Visit(visitor);
    }
};

#endif
