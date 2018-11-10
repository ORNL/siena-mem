// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "expr/FunctionCall.h"
#include <fstream>
#include <stdlib.h>

#include "expr/Array.h"
#include "expr/ArrayDereference.h"
#include "expr/Identifier.h"
#include "expr/Real.h"

FunctionCall::FunctionCall(string name, ParseVector<Expression*> args)
    : name(name), arguments(args.begin(),args.end())
{
}

FunctionCall::FunctionCall(string name, vector<Expression*> args)
    : name(name), arguments(args.begin(),args.end())
{
}

FunctionCall::FunctionCall(string name, Expression *arg0, Expression *arg1)
    : name(name)
{
    if (!arg0)
    {
        THROW(LogicError, "Null value in function call constructor!");
    }
    arguments.push_back(arg0);
    if (arg1)
        arguments.push_back(arg1);
}

FunctionCall::~FunctionCall()
{
    for (unsigned int i=0; i<arguments.size(); ++i)
        delete arguments[i];
}

unsigned int
FunctionCall::NumArgs() const
{
    if (name == "log"   ||
        name == "log2"  ||
        name == "floor" ||
        name == "ceil"  ||
        name == "sqrt")
        return 1;
    if (name == "max"   ||
        name == "min")
        return 2;
    // random samples
    if (name == "normal"   ||
        name == "uniform")
        return 2;
    if (name == "cdf")
        return 1;
    if (name == "lookup")
        return 3;
    THROW(GrammaticalError, "Unknown function call : ", name);
}

int
FunctionCall::GetDepth() const
{
    int depth = 0;
    for (unsigned int i=0; i<arguments.size(); ++i)
        depth = std::max(depth, arguments[i]->GetDepth());
    return depth + 1;
}

bool
FunctionCall::Equivalent(const Expression *e) const
{
    const FunctionCall *s = dynamic_cast<const FunctionCall*>(e);
    if (!s)
        return false;
    if (s->arguments.size() != arguments.size())
        return false;
    if (s->name != name)
        return false;
    for (unsigned int i=0; i<arguments.size(); ++i)
    {
        if (! arguments[i]->Equivalent(s->arguments[i]))
            return false;
    }
    return true;
}

void
FunctionCall::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "Call "<<name<< " with "<<arguments.size()<<" args:" << endl;
    for (unsigned int i=0; i<arguments.size(); i++)
    {
        arguments[i]->Print(out, indent+1);
    }
}

Expression*
FunctionCall::Cloned() const
{
    vector<Expression*> clonedargs;
    for (unsigned int i=0; i<arguments.size(); ++i)
        clonedargs.push_back(arguments[i]->Cloned());
    return new FunctionCall(name, clonedargs);
}

void
FunctionCall::ExpandInPlace(const ParameterStack &paramMap, set<string> expandedset)
{
    for (unsigned int i=0; i<arguments.size(); ++i)
    {
        Expression *e = arguments[i];
        if (dynamic_cast<Identifier*>(e) ||
            dynamic_cast<ArrayDereference*>(e))
        {
            arguments[i] = e->Expanded(paramMap, expandedset);
            delete e;
        }
        else
        {
            arguments[i]->ExpandInPlace(paramMap, expandedset);
        }
    }
}

Expression*
FunctionCall::Expanded(const ParameterStack &paramMap, set<string> expandedset) const
{
    vector<Expression*> expandedargs;
    for (unsigned int i=0; i<arguments.size(); ++i)
        expandedargs.push_back(arguments[i]->Expanded(paramMap, expandedset));
    return new FunctionCall(name, expandedargs);
}

Expression*
FunctionCall::OneStepSimplified() const
{
    // if we only have value contents, we can just return the evaluation
    // as our simplification
    bool allvalues = true;
    for (unsigned int i=0; i<arguments.size(); ++i)
    {
        if (dynamic_cast<Value*>(arguments[i]) == NULL)
            allvalues = false;
    }
    if (allvalues)
    {
        return new Real(Evaluate());
    }

    // otherwise, simplify our contents
    vector<Expression*> simplifiedargs;
    for (unsigned int i=0; i<arguments.size(); ++i)
        simplifiedargs.push_back(arguments[i]->OneStepSimplified());
    return new FunctionCall(name, simplifiedargs);
}
double
FunctionCall::LookupTime(const Expression *a1, const Expression *a2, const Expression *idx)
{
    const Array *vals = dynamic_cast<const Array*>(a1);
    const Array *sizes = dynamic_cast<const Array*>(a2);
    const Real *sz = dynamic_cast<const Real*>(idx);
    if (!vals || !sizes || !sz)
        THROW(GrammaticalError, "Expected two sorted arrays and a value as input to time lookup function");
    const vector<double> &V = vals->values;
    const vector<double> &S = sizes->values;
    int nvalues = S.size();
    if (V.size() != S.size())
        THROW(GrammaticalError, "Expected arrays to lookup to be of same length");
    double n = sz->value;
    double x1 = 0, y1 = 0, x2 = 0, y2 = 0, m = 0;
  
    for(int i = 0; i<nvalues-1; i++)
    {
        //if n lies between the two points
        if ((i==0 || n >= S[i]) &&
            (i==nvalues-2 || n <= S[i+1]))
        {
            y1 = V[i];
            y2 = V[i+1];
            x1 = S[i];
            x2 = S[i+1];
            m = (y2-y1)/(x2-x1);
            return y1 + m * (n-x1);
        }
    }
    THROW(LogicError, "Interpolation failed");
}

double
FunctionCall::Evaluate() const
{
    if (arguments.size() != NumArgs())
    {
        std::stringstream msg;
        msg << "Unexpected number of arguments to function call: "
            << arguments.size() << " for " << NumArgs() << " in function " << name;
        THROW(GrammaticalError, msg.str());
    }

    // special case random sample: cdf takes an array, not a single value
    if (name == "cdf") return CDFSample(arguments[0]);
    if (name == "lookup") return LookupTime(arguments[0], arguments[1], arguments[2]); 
        
    vector<double> v;
    for (unsigned int i=0; i<arguments.size(); ++i)
    {
        v.push_back(arguments[i]->Evaluate());
    }

    if (name == "log")    return log(v[0]);
    if (name == "log2")   return log(v[0]) / log(2);
    if (name == "max")    return v[0] > v[1] ? v[0] : v[1];
    if (name == "min")    return v[0] < v[1] ? v[0] : v[1];
    if (name == "floor")  return floor(v[0]);
    if (name == "ceil")   return ceil(v[0]);
    if (name == "sqrt")   return sqrt(v[0]);
    // random sampling functions
    if (name == "normal") return NormalSample(v[0],v[1]);
    if (name == "uniform") return UniformSample(v[0],v[1]);

    THROW(GrammaticalError, "Unknown function call: ", name);
}

string
FunctionCall::GetText(TextStyle style) const
{
    string ret = name;
    if (name == "log2")
        ret = "(log";

    ret += "(";
    for (unsigned int i=0; i<arguments.size(); ++i)
    {
        ret += arguments[i]->GetText(style);
        if (i<arguments.size()-1)
            ret += ", ";
    }
    ret += ")";

    if (name == "log2")
        ret += "/log(2))";

    return ret;
}

double
FunctionCall::CDFSample(const Expression *e)
{
    const Array *array = 
        dynamic_cast<const Array*>(e);
    if (!array)
        THROW(GrammaticalError, "Expected an array input to CDF sampler");
        
    double r = rand() / double(RAND_MAX);
    int index = 0;
    int n = array->values.size();
    while (index < n-1 && r > array->values[index])
        index++;
    return index;
}

double
FunctionCall::UniformSample(double lo, double hi)
{
    double r = rand() / double(RAND_MAX);
    return lo + r*(hi-lo);
}

double
FunctionCall::NormalSample(double mean, double stddev)
{
    double r1 = rand() / double(RAND_MAX);
    double r2 = rand() / double(RAND_MAX);
    return mean + stddev * sqrt(-2. * log(r1)) * cos(r2 * 2. * M_PI);
}

void
FunctionCall::Visit(ModelVisitor *visitor)
{
    if (visitor->Visit(this))
        return;
    for (unsigned int i=0; i<arguments.size(); ++i)
        arguments[i]->Visit(visitor);
}
