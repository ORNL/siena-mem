// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_KERNEL_H
#define AST_KERNEL_H

#include "common/AST.h"
#include "expr/Expression.h"
#include "app/ASTControlStatement.h"
//#include "mach/ASTMachComponent.h"
#include "app/ASTKernelArgDecl.h"
#include "model/ASTStatement.h"

class ASTMachModel;
class ASTAppModel;
class ASTControlSequentialStatement;

// ****************************************************************************
// Class:  ASTKernel
//
// Purpose:
///   Encapsulates a kernel, the main component of control flow and execution
///   within an Aspen application model.
//
// Programmer:  Jeremy Meredith
// Creation:    September 20, 2013
//
// Modifications:
// ****************************************************************************
class ASTKernel : public ASTNode
{
    friend class NamespacePrefixer; ///<\todo: better way than this?
  protected:
    Identifier *ident;
    vector<ASTKernelArgDecl*> args;
    vector<ASTStatement*> vars;
    ASTControlSequentialStatement *statements;

  public:
    NameMap<const Expression*> paramMap;
    ASTKernel(Identifier *id,
              ParseVector<ASTControlStatement*> statements,
              ParseVector<ASTKernelArgDecl*> args,
              ParseVector<ASTStatement*> vars);
    virtual ~ASTKernel();
    virtual void Print(ostream &out, int indent = 0) const;
    virtual void Export(ostream &out, int indent = 0) const;

    Expression *GetResourceRequirementExpression(const ASTAppModel *app,
                                                    string resource) const;

    Expression *GetDataSizeExpression(const ASTAppModel *app, set<string> arrays) const;

    set<string> GetInclusiveDataArrays(const ASTAppModel *app) const;
    set<string> GetExclusiveDataArrays(string blockname = "") const;

    Expression *GetInclusiveDataSizeExpression(const ASTAppModel *app) const;
    Expression *GetExclusiveDataSizeExpression(const ASTAppModel *app, string blockname="") const;

    Expression *GetSingleRegionInclusiveDataSizeExpression(const ASTAppModel *app, string label) const;

    Expression *GetTimeExpression(ASTAppModel *app,
                                     ASTMachModel *mach,
                                     string socket) const;
    Expression *GetSelfTimeExpression(ASTAppModel *app,
                                         ASTMachModel *mach,
                                         string socket) const;

    void Traverse(AspenTool *tool) const;
    bool ContainsRandomSample(ASTAppModel *app) const;
    virtual void Visit(ModelVisitor *visitor);

    const string &GetName() const { return ident->GetName(); }
    const vector<ASTKernelArgDecl*> &GetArgs() const { return args; }
    const ASTControlSequentialStatement *GetStatements() const { return statements; }
};


#endif
