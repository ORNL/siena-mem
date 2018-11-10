// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_APP_MODEL_H
#define AST_APP_MODEL_H

#include "common/Exception.h"
#include "common/AST.h"
#include "common/ASTNode.h"
#include "model/ASTStatement.h"
#include "app/ASTKernel.h"
#include "expr/Expression.h"
#include "common/NameMap.h"
#include "common/ModelVisitor.h"

// ****************************************************************************
// Class:  ASTAppModel
//
// Purpose:
///   Encapsulates an Aspen Application Model.
//
// Programmer:  Jeremy Meredith
// Creation:    May 15, 2013
//
// Modifications:
// ****************************************************************************
class ASTAppModel : public ASTNode
{
  protected:
    string name;

    vector<ASTStatement*> globals;
    vector<ASTKernel*> kernels;

  public:
    //
    // Acceleration structures
    //
    const ASTKernel *mainKernel;
    NameMap<const Expression*> paramMap;
    NameMap<const ASTKernel*> kernelMap;
  public:
    ASTAppModel(const string &n, ParseVector<ASTStatement*> globals, ParseVector<ASTKernel*> kernels);
    virtual ~ASTAppModel();
    virtual void CompleteAndCheck();
    virtual void Print(ostream &out, int indent = 0) const;
    virtual void Export(ostream &out, int indent = 0) const;
    int FindParametersWithRanges(vector<const Identifier*> &identifiers,
                                 vector<double> &defvals,
                                 vector<double> &minvals,
                                 vector<double> &maxvals) const;
    Expression *GetSingleArraySize(string array) const;
    Expression *GetGlobalArraySizeExpression() const;
    Expression *GetResourceRequirementExpression(string resource) const;
    Expression *GetControlFlowExpression() const;
    ///\todo: this should probably allow a whole set of substitutions
    double Count(string resource, string withparam="", double withparamvalue=0) const;
    string GetResourceRequirementExpressionText(string resource, bool expand, bool simplify,
                                                const NameMap<const Expression*> &subst=NameMap<const Expression*>()) const;
    void Visit(ModelVisitor *visitor);

    const string &GetName() const { return name; }
    const ASTKernel *GetKernel(const string &name) const { return kernelMap[name]; }
    const ASTKernel *GetMainKernel() const { return mainKernel; }
    const vector<ASTStatement*> &GetGlobals() const { return globals; }
    const vector<ASTKernel*> &GetKernels() const { return kernels; }

    void AddGlobal(ASTStatement *s) { globals.push_back(s); }
    void AddKernel(ASTKernel *k) { kernels.push_back(k); }
    void RedefineParam(string name, Expression* val);
};



#endif
