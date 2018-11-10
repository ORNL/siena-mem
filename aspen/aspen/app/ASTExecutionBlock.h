// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_EXECUTION_BLOCK
#define AST_EXECUTION_BLOCK

#include "common/AST.h"
#include "model/ASTAppModel.h"
#include "expr/Expression.h"
#include "app/ASTControlStatement.h"
#include "app/ASTExecutionStatement.h"

// ****************************************************************************
// Class:  ASTExecutionBlock
//
// Purpose:
///   Encapsulates an execute [n] { ... } block of resource requirements.
//
// Programmer:  Jeremy Meredith
// Creation:    August 20, 2013
//
// Modifications:
// ****************************************************************************
class ASTExecutionBlock : public ASTControlStatement
{
  protected:
    bool isparallel;
    Expression *parallelism;
    vector<ASTExecutionStatement*> statements;
  public:
    ASTExecutionBlock();
    ASTExecutionBlock(string label,
                      Expression *parallelism,
                      ParseVector<ASTExecutionStatement*> statements);
    virtual ~ASTExecutionBlock();
    virtual void Print(ostream &out, int indent = 0) const;
    virtual void Export(ostream &out, int indent = 0) const;
    Expression *GetResourceRequirementExpression(const ASTAppModel *app,
                                                 string resource) const;
    set<string> GetDataArrays() const;
    Expression *GetTimeExpression(ASTAppModel *app,
                                  ASTMachModel *mach,
                                  string sockettype) const;
    virtual set<string> GetInclusiveDataArrays(const ASTAppModel *app) const;
    virtual void Traverse(AspenTool *tool) const;
    virtual void Visit(ModelVisitor *visitor);
    virtual bool ContainsRandomSample(ASTAppModel*) const { return false; }


    const Expression *GetParallelism() const { return parallelism; }
    const vector<ASTExecutionStatement*> &GetStatements() const { return statements; }
};


#endif
