// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef FUNCTION_CALL_H
#define FUNCTION_CALL_H

#include "expr/Expression.h"

class FunctionCall : public Expression
{
  public:
    string name;
    vector<Expression*> arguments;
    unsigned int NumArgs() const;
  public:
    FunctionCall(string name, ParseVector<Expression*> args);
    FunctionCall(string name, vector<Expression*> args);
    FunctionCall(string name, Expression *arg0, Expression *arg1=NULL);
    virtual ~FunctionCall();
    virtual int GetDepth() const;
    virtual bool Equivalent(const Expression *e) const;
    virtual void Print(ostream &out, int indent = 0) const;
    virtual Expression *Cloned() const;
    virtual void ExpandInPlace(const ParameterStack &paramMap, set<string> expandedset = set<string>());
    virtual Expression *Expanded(const ParameterStack &paramMap, set<string> expandedset = set<string>()) const;
    virtual Expression *OneStepSimplified() const;
    virtual double Evaluate() const;
    virtual string GetText(TextStyle style = ASPEN) const;
    static double CDFSample(const Expression *e);
    static double LookupTime(const Expression *a1, const Expression *a2, const Expression *n);
    static double UniformSample(double lo, double hi);
    static double NormalSample(double mean, double stddev);
    virtual void Visit(ModelVisitor *visitor);
};

#endif
