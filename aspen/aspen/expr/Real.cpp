// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "expr/Real.h"

Real::Real(double value)
    : value(value)
{
}

Real::~Real()
{
#ifdef EXPRTRACE
    cerr << "deleting real "<<this<< "("<<value<<")" << endl;
#endif
}


Expression*
Real::Cloned() const
{
#ifdef EXPRTRACE
    cerr << "creating clone of real "<<this<< "("<<value<<")" << endl;
#endif
    return new Real(value);
}

void
Real::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "Real: "<<value<<endl;
}

bool
Real::Equivalent(const Expression *e) const
{
    const Value *v = dynamic_cast<const Value*>(e);
    if (v && v->GetValue() == value)
        return true;
    return false;
}

double
Real::Evaluate() const
{
    return value;
}

string
Real::GetText(TextStyle style) const
{
    ostringstream out;
    out << value;
    return out.str();
}

double
Real::GetValue() const
{
    return value;
}

void
Real::Visit(ModelVisitor *visitor)
{
    visitor->Visit(this);
}
