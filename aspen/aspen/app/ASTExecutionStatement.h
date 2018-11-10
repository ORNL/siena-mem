// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_EXECUTION_STATEMENT_H
#define AST_EXECUTION_STATEMENT_H

#include "common/AST.h"
#include "app/ASTTrait.h"
#include "expr/Expression.h"

// ****************************************************************************
// Class:  ASTExecutionStatement
//
// Purpose:
///   Base class for any statements that might exist within an execution
///   block.  At the moment, that's only a resource requirement statement.
//
// Programmer:  Jeremy Meredith
// Creation:    September 19, 2013
//
// Modifications:
// ****************************************************************************

class ASTExecutionStatement : public ASTNode
{
  public:
    virtual void Visit(ModelVisitor *visitor) = 0;
};


#endif
