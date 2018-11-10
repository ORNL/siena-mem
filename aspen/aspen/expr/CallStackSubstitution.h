// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef CALL_STACK_SUBSTITUTION_H
#define CALL_STACK_SUBSTITUTION_H

#include "expr/Expression.h"

class CallStackSubstitution : public Expression
{
  public:
    NameMap<const Expression*> arguments;
    Expression *expr;
    CallStackSubstitution(const NameMap<const Expression*> arguments, Expression *expr);
    virtual ~CallStackSubstitution();
    virtual int GetDepth() const;
    virtual Expression *Cloned() const;
    virtual void ExpandInPlace(const ParameterStack &paramMap, set<string> expandedset = set<string>());
    virtual Expression *Expanded(const ParameterStack &paramMap, set<string> expandedset = set<string>()) const;
    virtual void Print(ostream &out, int indent = 0) const;
    virtual double Evaluate() const;
    virtual string GetText(TextStyle style = ASPEN) const;
    virtual void Visit(ModelVisitor *visitor);
};

#endif
