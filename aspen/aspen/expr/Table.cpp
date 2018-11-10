// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "expr/Table.h"

#include "common/ParameterStack.h"

Table::Table(const ParseVector<double> &v)
{
    if (v.size() == 0)
        return;
    
    values.push_back(vector<double>());

    unsigned int i = 0;
    while (i < v.size())
    {
        if (i+2 <= v.size() && v[i] == +1e37 && v[i+1] == -1e37)
        {
            values.push_back(vector<double>());
            i += 2;
        }
        else
        {
            values.back().push_back(v[i]);
            i++;
        }
    }
}

Table::Table(const vector<vector<double> > &values)
    : values(values.begin(),values.end())
{
}

void
Table::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "Table"<<"["<<values.size()<<"][]: "
        << endl;
    for (unsigned int i=0; i<values.size(); ++i)
        out << Indent(indent+1) << "{"
            << VecPrint(&(values[i][0]),values[i].size()) << "}" << endl;
}

Expression*
Table::Cloned() const
{
    return new Table(values);
}

bool
Table::Equivalent(const Expression *e) const
{
    return false;
}

void
Table::ExpandInPlace(const ParameterStack &paramMap, set<string> expandedset)
{
    THROW(LogicError, "ExpandInPlace should never reach Table");
}

Expression*
Table::Expanded(const ParameterStack &paramMap, set<string> expandedset) const
{
    // Note: there's at least one case where we want to allow
    // tables to be used directly, so we allow expansions to
    // proceed (as no-ops).
    //THROW(LogicError, "Expanded should never reach Table");
    return Cloned();
}

double
Table::Evaluate() const
{
    THROW(LogicError, "Evaluate should never reach Table");
}

string
Table::GetText(TextStyle style) const
{
    ostringstream out;
    out << "{";
    for (unsigned int i=0; i<values.size(); ++i)
    {
        out << (i>0 ? ", " : "") << "{";
        for (unsigned int j=0; j<values[i].size(); ++j)
        {
            out << (j>0 ? ", " : "") << values[i][j];
        }
        out << "}";
    }
    out << "}";
    return out.str();
    return "";
}

void
Table::Visit(ModelVisitor *visitor)
{
    visitor->Visit(this);
}
