// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef IDENTIFIER_H
#define IDENTIFIER_H

#include "expr/Expression.h"

class Identifier : public Expression
{
  protected:
    string fqname;
    string fqname_cstyle;
  public:
    Identifier(const string& name);
    Identifier(const string& fqn, const string& fqc);
    virtual ~Identifier();
    virtual void Print(ostream &out, int indent = 0) const;
    virtual Expression *Cloned() const;
    virtual bool Equivalent(const Expression *e) const;
    virtual void ExpandInPlace(const ParameterStack &paramMap, set<string> expandedset = set<string>());
    virtual Expression *Expanded(const ParameterStack &paramMap, set<string> expandedset = set<string>()) const;
    virtual double Evaluate() const;
    virtual string GetText(TextStyle style = ASPEN) const;
    virtual void Visit(ModelVisitor *visitor);

    const string &GetName() const;
    void Prefix(const string &prefix);
};

#endif
