// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef ARRAY_H
#define ARRAY_H

#include "expr/Expression.h"

class Array : public Expression
{
  public:
    vector<double> values;
    vector<Expression*> arguments;
    Array(const ParseVector<double> &values);
    Array(const vector<double> &values);
    Array(const ParseVector<Expression*> &args);
    Array(const vector<Expression*> &args);
    virtual void Print(ostream &out, int indent = 0) const;
    virtual Expression *Cloned() const;
    virtual bool Equivalent(const Expression *e) const;
    virtual void ExpandInPlace(const ParameterStack &paramMap, set<string> expandedset = set<string>());
    virtual Expression *Expanded(const ParameterStack &paramMap, set<string> expandedset = set<string>()) const;
    virtual double Evaluate() const;
    virtual string GetText(TextStyle style = ASPEN) const;
    virtual void Visit(ModelVisitor *visitor);
};

#endif
