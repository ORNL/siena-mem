// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_CONTROL_ITERATE_STATEMENT_H
#define AST_CONTROL_ITERATE_STATEMENT_H

#include "app/ASTControlStatement.h"

class ASTControlIterateStatement : public ASTControlStatement
{
  protected:
    Identifier *counter;
    Expression *quantity;
    ASTControlStatement *item;
  public:
    ASTControlIterateStatement(string label, Identifier *counter, Expression *q, ASTControlStatement *item);
    virtual ~ASTControlIterateStatement();
    virtual void Print(ostream &out, int indent = 0) const;
    virtual void Export(ostream &out, int indent = 0) const;
    virtual Expression *GetResourceRequirementExpression(const ASTAppModel *app,
                                         string resource) const;
    virtual set<string>    GetInclusiveDataArrays(const ASTAppModel *app) const;
    virtual void Traverse(AspenTool *tool) const;
    virtual void Visit(ModelVisitor *visitor);
    virtual bool ContainsRandomSample(ASTAppModel *app) const;

    const Expression *GetQuantity() const { return quantity; }
    const Identifier *GetCounter() const { return counter; }
};

#endif
