// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_SAMPLE_STATEMENT_H
#define AST_SAMPLE_STATEMENT_H

#include "app/ASTControlStatement.h"

class ASTSampleStatement : public ASTControlStatement
{
    friend class NamespacePrefixer; ///<\todo: better way than this?
  protected:
    Identifier *ident;
    FunctionCall *distribution;
  public:
    ASTSampleStatement(Identifier *n, FunctionCall *d);
    virtual ~ASTSampleStatement();
    virtual void Print(ostream &out, int indent = 0) const;
    virtual void Export(ostream &out, int indent = 0) const;
    virtual Expression *GetResourceRequirementExpression(const ASTAppModel *app,
                                                         string resource) const;
    virtual set<string>    GetInclusiveDataArrays(const ASTAppModel *app) const;
    virtual void Traverse(AspenTool *tool) const;
    virtual void Visit(ModelVisitor *visitor);
    virtual bool ContainsRandomSample(ASTAppModel *app) const;

    const string &GetName() const { return ident->GetName(); }
    const FunctionCall *GetDistribution() const { return distribution; }
};

#endif
