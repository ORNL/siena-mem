// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef IF_THEN_EXPR_H
#define IF_THEN_EXPR_H

#include "expr/Expression.h"

class IfThenExpr : public Expression
{
  public:
    Expression *iftest;
    Expression *thenexpr;
    Expression *elseexpr;
    IfThenExpr(Expression *iftest, Expression *thenexpr, Expression *elseexpr);
    virtual ~IfThenExpr();
    virtual int GetDepth() const;
    virtual bool Equivalent(const Expression *e) const;
    virtual Expression *Cloned() const;
    virtual void ExpandInPlace(const ParameterStack &paramMap, set<string> expandedset = set<string>());
    virtual Expression *Expanded(const ParameterStack &paramMap, set<string> expandedset = set<string>()) const;
    virtual Expression *OneStepSimplified() const;
    virtual void Print(ostream &out, int indent = 0) const;
    virtual double Evaluate() const;
    virtual string GetText(TextStyle style = ASPEN) const;
    virtual void Visit(ModelVisitor *visitor);
};

#endif
