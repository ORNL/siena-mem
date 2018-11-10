// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "expr/Integer.h"

Integer::Integer(long long value)
    : value(value)
{
}

Integer::~Integer()
{
#ifdef EXPRTRACE
    cerr << "deleting integer "<<this<< "("<<value<<")" << endl;
#endif
}

Expression*
Integer::Cloned() const
{
#ifdef EXPRTRACE
    cerr << "creating clone of integer "<<this<< "("<<value<<")" << endl;
#endif
    return new Integer(value);
}

void
Integer::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "Int: "<<value<<endl;
}

bool
Integer::Equivalent(const Expression *e) const
{
    const Value *v = dynamic_cast<const Value*>(e);
    if (v && v->GetValue() == value)
        return true;
    return false;
}

double
Integer::Evaluate() const
{
    return value;
}

string
Integer::GetText(TextStyle style) const
{
    ostringstream out;
    out << value;
    return out.str();
}

double
Integer::GetValue() const
{
    return value;
}

void
Integer::Visit(ModelVisitor *visitor)
{
    visitor->Visit(this);
}

