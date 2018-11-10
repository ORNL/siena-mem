// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_ENUM_DECLARATION_H
#define AST_ENUM_DECLARATION_H

#include "model/ASTStatement.h"

class ASTEnumDeclaration : public ASTStatement
{
  protected:
    string name;
    vector<string> values;
  public:
    ASTEnumDeclaration(const string &n, ParseVector<string> &values)
        : name(n), values(values.begin(), values.end())
    {
    }
    virtual ~ASTEnumDeclaration()
    {
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Enum '"<<name<< "': {"
            << VecPrint(&values[0], values.size()) << "}" << endl;
    }
    virtual void Export(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "enum " << name << " { ";
        for (unsigned int i=0; i<values.size(); ++i)
            out << (i>0 ? ", " : "") << values[i];
        out << " }" << endl;
    }
    virtual void Visit(ModelVisitor *visitor)
    {
        if (visitor->Visit(this))
            return;
    }

    const string &GetName() const { return name; }
    const unsigned int GetNumValues() const { return values.size(); }
    const string &GetValue(int i) const { return values[i]; }
};

#endif
