// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_NODE_H
#define AST_NODE_H

#include <string>
#include <iostream>

// ****************************************************************************
// Class:  ASTNode
//
// Purpose:
///   Base class for all nodes in the Aspen abstract syntax tree.
//
// Programmer:  Jeremy Meredith
// Creation:    May 15, 2013
//
// Modifications:
// ****************************************************************************
class ASTNode
{
  public:
    ASTNode();
    virtual ~ASTNode();
    virtual void Print(std::ostream &out, int indent = 0) const = 0;
    virtual void Export(std::ostream &out, int indent = 0) const { }
};

#endif
