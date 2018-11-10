// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "expr/CallStackSubstitution.h"

#include "expr/ArrayDereference.h"
#include "expr/Identifier.h"
#include "common/ParameterStack.h"

CallStackSubstitution::CallStackSubstitution(const NameMap<const Expression*> arguments,
                                             Expression *expr)
    : arguments(arguments), expr(expr)
{
    for (NameMap<const Expression*>::const_iterator it = arguments.begin(); it != arguments.end(); ++it)
    {
        if (!it->second)
            THROW(LogicError, "Null value in argument to call stack expression");
    }
    if (!expr)
    {
        THROW(LogicError, "Null value in call stack constructor");
    }
}

CallStackSubstitution::~CallStackSubstitution()
{
    delete expr;
}

int
CallStackSubstitution::GetDepth() const
{
    return 1 + expr->GetDepth();
}

Expression*
CallStackSubstitution::Cloned() const
{
    return new CallStackSubstitution(arguments, expr->Cloned());
}

void
CallStackSubstitution::ExpandInPlace(const ParameterStack &paramMap, set<string> expandedset)
{
    if (dynamic_cast<Identifier*>(expr) ||
        dynamic_cast<ArrayDereference*>(expr))
    {
        Expression *tmp1 = expr;
        Expression *tmp2 = tmp1->Expanded(arguments);
        expr = tmp2->Expanded(paramMap, expandedset);
        delete tmp2;
        delete tmp1;
    }
    else
    {
        expr->ExpandInPlace(arguments, expandedset);
        expr->ExpandInPlace(paramMap, expandedset);
    }
}

Expression*
CallStackSubstitution::Expanded(const ParameterStack &paramMap, set<string> expandedset) const
{
    Expression *tmp = expr->Expanded(arguments);
    Expression *ret = tmp->Expanded(paramMap, expandedset);
    delete tmp;
    return ret;
}

void
CallStackSubstitution::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "CallStack"<<endl;
    vector<string> keys = arguments.Keys();
    for (unsigned int i=0; i<keys.size(); ++i)
    {
        out << Indent(indent+1) << "Argument \""<<keys[i]<<"\":" << endl;
        arguments[keys[i]]->Print(out,indent+2);
    }
    out << Indent(indent+1) << "Expression:" << endl;
    expr->Print(out,indent+2);
}

double
CallStackSubstitution::Evaluate() const
{
    return expr->Evaluate();
}

string
CallStackSubstitution::GetText(TextStyle style) const
{
#if 0 // debug
    return "SUBST(" + expr->GetText(style) + ")";
#else
    Expression *tmp = expr->Expanded(arguments);
    string retval = tmp->GetText(style);
    delete tmp;
    return retval;
#endif
}

void
CallStackSubstitution::Visit(ModelVisitor *visitor)
{
    if (visitor->Visit(this))
        return;
    expr->Visit(visitor);
}
