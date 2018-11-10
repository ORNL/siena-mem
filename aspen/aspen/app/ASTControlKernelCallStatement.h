// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_CONTROL_KERNEL_CALL_STATEMENT_H
#define AST_CONTROL_KERNEL_CALL_STATEMENT_H

#include "app/ASTControlStatement.h"

class ASTControlKernelCallStatement : public ASTControlStatement
{
    friend class NamespacePrefixer; ///<\todo: better way than this?
  protected:
    Identifier *ident;
    vector<Expression*> arguments;
  public:
    ASTControlKernelCallStatement(Identifier *id);
    ASTControlKernelCallStatement(Identifier *id, ParseVector<Expression*> args);
    virtual ~ASTControlKernelCallStatement();
    virtual void Print(ostream &out, int indent = 0) const;
    virtual void Export(ostream &out, int indent = 0) const;
    const ASTKernel *GetKernel(const ASTAppModel *app) const;
    virtual Expression *GetResourceRequirementExpression(const ASTAppModel *app,
                                         string resource) const;
    virtual set<string>    GetInclusiveDataArrays(const ASTAppModel *app) const;
    virtual void Traverse(AspenTool *tool) const;
    virtual void Visit(ModelVisitor *visitor);
    virtual bool ContainsRandomSample(ASTAppModel *app) const;

    const string &GetFullyQualifiedName() const { return ident->GetName(); }
    const vector<Expression*> &GetArguments() const { return arguments; }
};

#endif

