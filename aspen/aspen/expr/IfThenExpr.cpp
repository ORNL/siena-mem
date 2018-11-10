// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "expr/IfThenExpr.h"

#include "expr/ArrayDereference.h"
#include "expr/Identifier.h"
#include "common/ParameterStack.h"
#include "expr/Real.h"

IfThenExpr::IfThenExpr(Expression *iftest, Expression *thenexpr, Expression *elseexpr)
    : iftest(iftest), thenexpr(thenexpr), elseexpr(elseexpr)
{
    if (!iftest || !thenexpr) // elseexpr CAN be NULL
    {
        THROW(LogicError, "Null value in binary op constructor!");
    }
}

IfThenExpr::~IfThenExpr()
{
    delete iftest;
    delete thenexpr;
    delete elseexpr;
}

int
IfThenExpr::GetDepth() const
{
    int depth = std::max(iftest->GetDepth(), thenexpr->GetDepth());
    if (elseexpr)
        depth = std::max(depth, elseexpr->GetDepth());
    return depth + 1;
}

bool
IfThenExpr::Equivalent(const Expression *e) const
{
    const IfThenExpr *i = dynamic_cast<const IfThenExpr*>(e);
    if (!i)
        return false;
    if (! i->iftest->Equivalent(iftest))
        return false;
    if (! i->thenexpr->Equivalent(thenexpr))
        return false;
    // neither has an else; equivalent so far, return true
    if (!i->elseexpr && !elseexpr)
        return true;
    // only one does not have an else; can't be equivalent, return false
    if (!i->elseexpr || !elseexpr)
        return false;
    // both have else; test for equivalency
    if (! i->elseexpr->Equivalent(elseexpr))
        return false;
    return true;
}

Expression*
IfThenExpr::Cloned() const
{
    return new IfThenExpr(iftest->Cloned(),
                          thenexpr->Cloned(),
                          elseexpr ? elseexpr->Cloned() : NULL);
}

void
IfThenExpr::ExpandInPlace(const ParameterStack &paramMap, set<string> expandedset)
{
    if (dynamic_cast<Identifier*>(iftest) ||
        dynamic_cast<ArrayDereference*>(iftest))
    {
        Expression *tmp = iftest;
        iftest = tmp->Expanded(paramMap, expandedset);
        delete tmp;
    }
    else
    {
        iftest->ExpandInPlace(paramMap, expandedset);
    }

    if (dynamic_cast<Identifier*>(thenexpr) ||
        dynamic_cast<ArrayDereference*>(thenexpr))
    {
        Expression *tmp = thenexpr;
        thenexpr = tmp->Expanded(paramMap, expandedset);
        delete tmp;
    }
    else
    {
        iftest->ExpandInPlace(paramMap, expandedset);
    }

    if (!elseexpr)
        return;

    if (dynamic_cast<Identifier*>(elseexpr) ||
        dynamic_cast<ArrayDereference*>(elseexpr))
    {
        Expression *tmp = elseexpr;
        elseexpr = tmp->Expanded(paramMap, expandedset);
        delete tmp;
    }
    else
    {
        elseexpr->ExpandInPlace(paramMap, expandedset);
    }

}

Expression*
IfThenExpr::Expanded(const ParameterStack &paramMap, set<string> expandedset) const
{
    return new IfThenExpr(iftest->Expanded(paramMap, expandedset),
                          thenexpr->Expanded(paramMap, expandedset),
                          elseexpr ? elseexpr->Expanded(paramMap, expandedset) : NULL);
}

Expression*
IfThenExpr::OneStepSimplified() const
{
    Expression *default_for_no_else = new Real(0);
    Value *ifval = dynamic_cast<Value*>(iftest);

    // if our conditional is just a value, we know which case to return
    if (ifval)
    {
        if (ifval->Evaluate() != 0)
            return thenexpr->OneStepSimplified();
        else if (elseexpr)
            return elseexpr->OneStepSimplified();
        else
            return default_for_no_else;
    }

    // no easy simplification
    return new IfThenExpr(iftest->OneStepSimplified(),
                          thenexpr->OneStepSimplified(),
                          elseexpr ? elseexpr->OneStepSimplified() : NULL);
}

void
IfThenExpr::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "If test:"<<endl;
    out << Indent(indent+1) << "Condition:"<<endl;
    iftest->Print(out,indent+2);
    out << Indent(indent+1) << "Then:"<<endl;
    thenexpr->Print(out,indent+2);
    if (elseexpr)
    {
        out << Indent(indent+1) << "Else:"<<endl;
        elseexpr->Print(out,indent+2);
    }
    else
    {
        out << Indent(indent+1) << "No Else"<<endl;
    }
}

double
IfThenExpr::Evaluate() const
{
    double condition = iftest->Evaluate();
    double default_for_no_else = 0;
    if (condition)
        return thenexpr->Evaluate();
    else if (elseexpr)
        return elseexpr->Evaluate();
    else
        return default_for_no_else;
}

string
IfThenExpr::GetText(TextStyle style) const
{
    bool ternary = style==GNUPLOT || style==C;
    string default_for_no_else = "0";
    if (ternary)
        return string("(") +
            iftest->GetText(style) + " ? " +
            thenexpr->GetText(style) + " : " +
            (elseexpr ? elseexpr->GetText(style) : default_for_no_else) + ")";
    else
        return string("if(") + 
            iftest->GetText(style) + ", " +
            thenexpr->GetText(style) + ", " +
            (elseexpr ? elseexpr->GetText(style) : default_for_no_else) + ")";
}

void
IfThenExpr::Visit(ModelVisitor *visitor)
{
    if (visitor->Visit(this))
        return;
    iftest->Visit(visitor);
    thenexpr->Visit(visitor);
    if (elseexpr)
        elseexpr->Visit(visitor);
}
