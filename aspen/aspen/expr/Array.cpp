// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "expr/Array.h"

#include "common/ParameterStack.h"

Array::Array(const ParseVector<double> &values)
    : values(values.begin(),values.end())
{
}

Array::Array(const vector<double> &values)
    : values(values.begin(),values.end())
{

}
Array::Array(const ParseVector<Expression*> &args)
    : arguments(args.begin(),args.end())
{
}

Array::Array(const vector<Expression*> &args)
    : arguments(args.begin(),args.end())
{
}

void
Array::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "Array"<<"["<<values.size()<<"]: "
        << "{" << VecPrint(&values[0],values.size()) << "}" << endl;
}

Expression*
Array::Cloned() const
{
    return new Array(values);
}

bool
Array::Equivalent(const Expression *e) const
{
    const Array *a = dynamic_cast<const Array*>(e);
    if (!a)
        return false;
    if (a->values.size() != values.size())
        return false;
    for (unsigned int i=0; i<values.size(); ++i)
    {
        if (values[i] != a->values[i])
            return false;
    }
    return true;
}

void
Array::ExpandInPlace(const ParameterStack &paramMap, set<string> expandedset)
{
    THROW(LogicError, "ExpandInPlace should never reach Array");
}

Expression*
Array::Expanded(const ParameterStack &paramMap, set<string> expandedset) const
{
    // Note: there's at least one case where we want to allow
    // arrays to be used directly, e.g. sampling from a user-
    // defined cumulative distribution functon defined as an array.
    // E.g., "param cdf = {.5, .9, 1.}" "sample x from cdf(cdf)".
    // So we need to allow expansions to proceed (as no-ops).
    // This also allows hypothetical future use of in-place
    // arrays, like "flops [{10,20,30} @ 1]".
    //THROW(LogicError, "Expanded should never reach Array");
    return Cloned();
}

double
Array::Evaluate() const
{
    THROW(LogicError, "Evaluate should never reach Array");
}

string
Array::GetText(TextStyle style) const
{
    ostringstream out;
    out << "{";
    for (unsigned int i=0; i<values.size(); ++i)
        out << (i>0 ? ", " : "") << values[i];
    out << "}";
    return out.str();
}

void
Array::Visit(ModelVisitor *visitor)
{
    visitor->Visit(this);
}
