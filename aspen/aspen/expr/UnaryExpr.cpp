// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "expr/UnaryExpr.h"

#include "expr/ArrayDereference.h"
#include "expr/Identifier.h"
#include "expr/Real.h"

UnaryExpr::UnaryExpr(string op, Expression *expr)
    : op(op), expr(expr)
{
    if (!expr)
    {
        THROW(LogicError, "Null value in unary op constructor");
    }
}

UnaryExpr::~UnaryExpr()
{
    delete expr;
}

int
UnaryExpr::GetDepth() const
{
    return 1 + expr->GetDepth();
}

bool
UnaryExpr::Equivalent(const Expression *e) const
{
    const UnaryExpr *u = dynamic_cast<const UnaryExpr*>(e);
    if (!u)
        return false;
    if (u->op != op)
        return false;
    if (! u->expr->Equivalent(expr))
        return false;
    return true;
}

Expression*
UnaryExpr::Cloned() const
{
    return new UnaryExpr(op, expr->Cloned());
}

void
UnaryExpr::ExpandInPlace(const ParameterStack &paramMap, set<string> expandedset)
{
    if (dynamic_cast<Identifier*>(expr) ||
        dynamic_cast<ArrayDereference*>(expr))
    {
        Expression *tmp = expr;
        expr = tmp->Expanded(paramMap, expandedset);
        delete tmp;
    }
    else
    {
        expr->ExpandInPlace(paramMap, expandedset);
    }
}

Expression*
UnaryExpr::Expanded(const ParameterStack &paramMap, set<string> expandedset) const
{
    return new UnaryExpr(op, expr->Expanded(paramMap, expandedset));
}

Expression*
UnaryExpr::OneStepSimplified() const
{
    if (dynamic_cast<Value*>(expr))
        return new Real(Evaluate());

    return new UnaryExpr(op, expr->OneStepSimplified());
}

void
UnaryExpr::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "Unary Op "<<op<<endl;
    expr->Print(out,indent+1);
}

double
UnaryExpr::Evaluate() const
{
    if (op == "-") return -expr->Evaluate();
    THROW(LogicError, "A unary expression must be '-'.");
}

string
UnaryExpr::GetText(TextStyle style) const
{
    return "(" + op + " " + expr->GetText(style) + ")";
}

void
UnaryExpr::Visit(ModelVisitor *visitor)
{
    if (visitor->Visit(this))
        return;
    expr->Visit(visitor);
}
