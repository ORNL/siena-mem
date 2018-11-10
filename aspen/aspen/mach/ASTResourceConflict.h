// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_RESOURCE_CONFLICT_H
#define AST_RESOURCE_CONFLICT_H

#include "common/AST.h"

class ASTResourceConflict : public ASTNode
{
  protected:
    const string first;
    const string second;
  public:
    ASTResourceConflict(string a, string b)
        : first(a), second(b)
    {
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Conflict: "
            << first << ","
            << second << endl;
    }
    bool Conflicts(const string &a, const string &b) const
    {
        return ((first == a && second == b) ||
                (second == a && first == b));
    }
};

#endif
