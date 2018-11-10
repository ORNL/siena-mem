// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "expr/Comparison.h"

#include "expr/ArrayDereference.h"
#include "expr/Identifier.h"
#include "expr/Real.h"

Comparison::Comparison(string op, Expression *lhs, Expression *rhs)
    : op(op), lhs(lhs), rhs(rhs)
{
    if (!lhs || !rhs)
    {
        THROW(LogicError, "Null value in comparison constructor!");
    }
}

Comparison::~Comparison()
{
    delete lhs;
    delete rhs;
}

int
Comparison::GetDepth() const
{
    return 1 + std::max(lhs->GetDepth(), rhs->GetDepth());
}

bool
Comparison::Equivalent(const Expression *e) const
{
    const Comparison *c = dynamic_cast<const Comparison*>(e);
    if (!c)
        return false;
    if (c->op != op)
        return false;
    if (! c->lhs->Equivalent(lhs))
        return false;
    if (! c->rhs->Equivalent(rhs))
        return false;
    return true;
}

Expression*
Comparison::Cloned() const
{
    return new Comparison(op, lhs->Cloned(), rhs->Cloned());
}

void
Comparison::ExpandInPlace(const ParameterStack &paramMap, set<string> expandedset)
{
    if (dynamic_cast<Identifier*>(lhs) ||
        dynamic_cast<ArrayDereference*>(lhs))
    {
        Expression *tmp = lhs;
        lhs = tmp->Expanded(paramMap, expandedset);
        delete tmp;
    }
    else
    {
        lhs->ExpandInPlace(paramMap, expandedset);
    }

    if (dynamic_cast<Identifier*>(rhs) ||
        dynamic_cast<ArrayDereference*>(rhs))
    {
        Expression *tmp = rhs;
        rhs = tmp->Expanded(paramMap, expandedset);
        delete tmp;
    }
    else
    {
        rhs->ExpandInPlace(paramMap, expandedset);
    }
}

Expression*
Comparison::Expanded(const ParameterStack &paramMap, set<string> expandedset) const
{
    return new Comparison(op, lhs->Expanded(paramMap, expandedset), rhs->Expanded(paramMap, expandedset));
}

Expression*
Comparison::OneStepSimplified() const
{
    Value *lv = dynamic_cast<Value*>(lhs);
    Value *rv = dynamic_cast<Value*>(rhs);

    // if we have two values, just evaluate them (don't need a paramMap)
    if (lv && rv)
    {
        return new Real(Evaluate());
    }

    // no easy simplification
    return new Comparison(op, lhs->OneStepSimplified(), rhs->OneStepSimplified());
}

void
Comparison::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "Comparison "<<op<<endl;
    out << Indent(indent+1) << "LHS"<<endl;
    lhs->Print(out,indent+2);
    out << Indent(indent+1) << "RHS"<<endl;
    rhs->Print(out,indent+2);
}

double
Comparison::Evaluate() const
{
    double l = lhs->Evaluate();
    double r = rhs->Evaluate();

    if (op == "and") return l && r;
    if (op == "or")  return l || r;

    if (op == "==") return l == r;
    if (op == "!=") return l != r;
    if (op == "<")  return l <  r;
    if (op == ">")  return l >  r;
    if (op == "<=") return l <= r;
    if (op == ">=") return l >= r;
    THROW(GrammaticalError, "Unknown operator : ", op);
}

string
Comparison::GetText(TextStyle style) const
{
    Value *lv = dynamic_cast<Value*>(lhs);
    Value *rv = dynamic_cast<Value*>(rhs);
    Identifier *li = dynamic_cast<Identifier*>(lhs);
    Identifier *ri = dynamic_cast<Identifier*>(rhs);

    bool lneedparens = false;
    bool rneedparens = false;
    if (!lv && !li)
        lneedparens = true;
    if (!rv && !ri)
        rneedparens = true;

    string lt = lhs->GetText(style);
    string rt = rhs->GetText(style);

    if (rneedparens)
        rt = "("+rt+")";
    if (lneedparens)
        lt = "("+lt+")";

    return lt + " " + op + " " + rt;
}

void
Comparison::Visit(ModelVisitor *visitor)
{
    if (visitor->Visit(this))
        return;
    lhs->Visit(visitor);
    rhs->Visit(visitor);
}
