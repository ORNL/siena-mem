// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_CONTROL_IF_THEN_STATEMENT_H
#define AST_CONTROL_IF_THEN_STATEMENT_H

#include "app/ASTControlStatement.h"

class ASTControlIfThenStatement : public ASTControlStatement
{
  protected:
    Expression *comparison;
    ASTControlStatement *thenitem;
    ASTControlStatement *elseitem;
  public:
    ASTControlIfThenStatement(Expression *comparison,
                              ASTControlStatement *thenitem, ASTControlStatement *elseitem);
    virtual ~ASTControlIfThenStatement();
    virtual void Print(ostream &out, int indent = 0) const;
    virtual void Export(ostream &out, int indent = 0) const;
    virtual Expression *GetResourceRequirementExpression(const ASTAppModel *app,
                                         string resource) const;
    virtual set<string>    GetInclusiveDataArrays(const ASTAppModel *app) const;
    virtual void Traverse(AspenTool *tool) const;
    virtual void Visit(ModelVisitor *visitor);
    virtual bool ContainsRandomSample(ASTAppModel *app) const;
};

#endif
