// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_CONTROL_STATEMENT_H
#define AST_CONTROL_STATEMENT_H

#include "model/ASTStatement.h"
#include "expr/BinaryExpr.h"
#include "expr/CallStackSubstitution.h"
#include "expr/Comparison.h"
#include "expr/FunctionCall.h"
#include "expr/IfThenExpr.h"
#include "expr/Identifier.h"
#include "expr/Integer.h"
#include "expr/Real.h"
#include "expr/SequenceExpr.h"
#include "common/ParameterStack.h"

class ASTAppModel;
class ASTMachModel;

class ASTKernel;
class AspenTool;

// ****************************************************************************
// Class:  ASTControlStatement
//
// Purpose:
///   A statement which is a type of control flow, such as iterate, 
///   parallel, kernel-call, or execution-block.
//
// Programmer:  Jeremy Meredith
// Creation:    May 17, 2013
//
// Modifications:
// ****************************************************************************
class ASTControlStatement : public ASTStatement
{
  protected:
    string nodeid;
    string label;

  public:
    ASTControlStatement(string label = "");
    virtual Expression *GetResourceRequirementExpression(const ASTAppModel *app,
                                         string resource) const = 0;
    virtual set<string>    GetInclusiveDataArrays(const ASTAppModel *app) const = 0;
    virtual void Traverse(AspenTool *tool) const = 0;
    virtual void Visit(ModelVisitor *visitor) = 0;
    virtual bool ContainsRandomSample(ASTAppModel *app) const = 0;

    const string &GetLabel() const { return label; }
    const string &GetNodeid() const { return nodeid; }
};


#endif
