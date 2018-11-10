// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "expr/Identifier.h"

#include "common/ParameterStack.h"

Identifier::Identifier(const string& name) : fqname(name), fqname_cstyle(name)
{
}

Identifier::Identifier(const string& fqn, const string& fqnc) : fqname(fqn), fqname_cstyle(fqnc)
{
    ///\todo: this style is only used in Cloned(); if we switch to storing
    /// an identifier as a stack of strings (instead of including the prefix
    /// within each style of name), this can go away.
}

Identifier::~Identifier()
{
#ifdef EXPRTRACE
    cerr << "deleting ident "<<this<< "("<<name<<")" << endl;
#endif
}

void
Identifier::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "Ident: "<<fqname<<endl;
}
    
Expression*
Identifier::Cloned() const
{
#ifdef EXPRTRACE
    cerr << "creating clone of ident "<<this<< "("<<fqname<<")" << endl;
#endif
    return new Identifier(fqname, fqname_cstyle);
}

bool
Identifier::Equivalent(const Expression *e) const
{
    const Identifier *i = dynamic_cast<const Identifier*>(e);
    if (i && i->fqname == fqname)
        return true;
    return false;
}

void
Identifier::ExpandInPlace(const ParameterStack &paramMap, set<string> expandedset)
{
    THROW(LogicError, "ExpandInPlace should never reach Identifier");
}

Expression*
Identifier::Expanded(const ParameterStack &paramMap, set<string> expandedset) const
{
    const Expression *expansion = paramMap.Find(fqname);
    if (expansion)
    {
        if (expandedset.count(fqname) != 0)
            THROW(GrammaticalError, "Recursive expansion of ", fqname, " detected");
        expandedset.insert(fqname);
        return expansion->Expanded(paramMap, expandedset);
    }
    else
        return Cloned();
}

double
Identifier::Evaluate() const
{
    THROW(GrammaticalError, "Can't evaluate; identifier '", fqname, "' wasn't expanded");
}

string
Identifier::GetText(TextStyle style) const
{
    if (style == Expression::C)
        return fqname_cstyle;
    else
        return fqname;
}

void
Identifier::Visit(ModelVisitor *visitor)
{
    visitor->Visit(this);
}

const string&
Identifier::GetName() const
{
    return fqname;
}

void
Identifier::Prefix(const string &prefix)
{
    fqname = prefix + "." + fqname;
    fqname_cstyle = prefix + "__" + fqname_cstyle;
}

