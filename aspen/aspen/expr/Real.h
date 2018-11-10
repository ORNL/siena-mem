// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef REAL_H
#define REAL_H

#include "expr/Value.h"

class Real : public Value
{
  public:
    double value;
    Real(double value);
    virtual ~Real();
    virtual Expression *Cloned() const;
    virtual void Print(ostream &out, int indent = 0) const;
    virtual bool Equivalent(const Expression *e) const;
    virtual double Evaluate() const;
    virtual string GetText(TextStyle style = ASPEN) const;
    virtual double GetValue() const;
    virtual void Visit(ModelVisitor *visitor);
};

#endif
