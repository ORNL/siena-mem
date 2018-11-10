// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_STATEMENT_H
#define AST_STATEMENT_H

#include "common/AST.h"
#include "expr/Expression.h"
#include "parser/ParseVector.h"
#include "common/ModelVisitor.h"

// ****************************************************************************
// Class:  ASTStatement
//
// Purpose:
///   Base class for all statements.  Major types of statement include
///   globals (param and data declarations and imports),
///   control (kernel calls, execution blocks, and other control flow) and
///   execution (resource requirements within execution blocks).
//
// Programmer:  Jeremy Meredith
// Creation:    May 15, 2013
//
// Modifications:
// ****************************************************************************
class ASTStatement : public ASTNode
{
  public:
    virtual void Visit(ModelVisitor *visitor) = 0;
};

#endif
