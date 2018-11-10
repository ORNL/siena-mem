// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef SEQUENCE_EXPR_H
#define SEQUENCE_EXPR_H

#include "expr/Expression.h"

class SequenceExpr : public Expression
{
  public:
    string op;
    vector<Expression*> entries;
  public:
    SequenceExpr(string op,
                 Expression *a = NULL,
                 Expression *b = NULL,
                 Expression *c = NULL,
                 Expression *d = NULL);
    virtual ~SequenceExpr();
    virtual int GetDepth() const;
    virtual bool Equivalent(const Expression *e) const;
    virtual Expression *OneStepSimplified() const;
    virtual Expression *Cloned() const;
    virtual void ExpandInPlace(const ParameterStack &paramMap, set<string> expandedset = set<string>());
    virtual Expression *Expanded(const ParameterStack &paramMap, set<string> expandedset = set<string>()) const;
    virtual void Print(ostream &out, int indent = 0) const;
    virtual double Evaluate() const;
    virtual string GetText(TextStyle style = ASPEN) const;
    virtual void Visit(ModelVisitor *visitor);
};

#endif
