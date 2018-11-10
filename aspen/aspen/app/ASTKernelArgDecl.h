// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_KERNEL_ARG_DECL_H
#define AST_KERNEL_ARG_DECL_H

#include "common/AST.h"
#include "expr/Expression.h"

/// example: "param x" and "data y" as occurs in kernel arg declarations
class ASTKernelArgDecl : public ASTNode
{
    friend class NamespacePrefixer;
  protected:
    string type;
    Identifier *ident;
  public:
    ASTKernelArgDecl(string type, Identifier *id) : type(type), ident(id) { }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Type="<<type<< " name=" << ident->GetName() << endl;
    }
    virtual void Export(ostream &out, int indent = 0) const
    {
        out << type << " " << ident->GetName();
    }
    virtual void Visit(ModelVisitor *visitor)
    {
        if (visitor->Visit(this))
            return;
    }

    const string &GetType() const { return type; }
    const string &GetName() const { return ident->GetName(); }
};

#endif
