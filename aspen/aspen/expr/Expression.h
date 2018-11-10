// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <sstream>
#include <cmath>
#include <vector>

#include "common/Exception.h"
#include "common/AST.h"
#include "common/ASTNode.h"
#include "common/NameMap.h"
#include "common/ModelVisitor.h"
#include "parser/ParseVector.h"

// ****************************************************************************
// Class:  Expression
//
// Purpose:
///   Base class for all mathematical expressions.
//
// Programmer:  Jeremy Meredith
// Creation:    May 17, 2013
//
// Modifications:
// ****************************************************************************
struct ParameterStack;

class Expression : public ASTNode
{
  public:
    enum TextStyle { ASPEN, C, GNUPLOT };
  public:
    virtual void Print(ostream &out, int indent = 0) const = 0;
    virtual double Evaluate() const = 0;
    virtual string GetText(TextStyle style = ASPEN) const = 0;
    virtual int    GetDepth() const { return 1; }
    virtual bool        Equivalent(const Expression *e) const { return false; }
    virtual Expression *Cloned() const = 0;
    virtual void        ExpandInPlace(const ParameterStack &paramMap, set<string> expandedset = set<string>()) { }
    virtual Expression *Expanded(const ParameterStack &paramMap, set<string> expandedset = set<string>()) const { return Cloned(); }
    virtual Expression *OneStepSimplified() const { return Cloned(); }
    virtual void Visit(ModelVisitor *visitor) = 0;
    virtual Expression *Simplified() const;
    virtual double EvaluateWithExpansion(const ParameterStack &paramMap) const;
};

// Each operation on an expression creates a new one
// and leaves the old one unmodified.  Using a smart pointer
// greatly simplifies the memory management to
// prevent leaks.
typedef std::unique_ptr<Expression> ExprPtr;


#endif
