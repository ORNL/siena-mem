// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_ASSIGN_STATEMENT_H
#define AST_ASSIGN_STATEMENT_H

#include "model/ASTStatement.h"
#include "expr/Identifier.h"

class ASTAssignStatement : public ASTStatement
{
    friend class NamespacePrefixer; ///<\todo: better way than this?
  protected:
    Identifier *ident;
    Expression *value;
  public:
    ASTAssignStatement(Identifier *n, Expression *v)
        : ident(n), value(v)
    {
    }
    virtual ~ASTAssignStatement()
    {
        delete ident;
        delete value;
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Identifier "<<ident->GetName()<< " = " << endl;
        value->Print(out, indent+1);
    }
    virtual void Export(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "param "<<ident->GetName()<< " = " << value->GetText(Expression::ASPEN) << endl;
    }
    virtual void Visit(ModelVisitor *visitor)
    {
        if (visitor->Visit(this))
            return;
        value->Visit(visitor);
    }
    
    const string &GetName() const { return ident->GetName(); }
    const Identifier *GetIdentifier() const { return ident; }
    const Expression *GetValue() const { return value; }
    void ChangeValue(Expression *expr) { delete value; value = expr; }
};

#endif
