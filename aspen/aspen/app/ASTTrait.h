// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_TRAIT_H
#define AST_TRAIT_H

#include "common/AST.h"
#include "expr/Expression.h"

/// example: "as trait1, trait2(value)"
class ASTTrait : public ASTNode
{
  protected:
    string name;
    Expression *value;
    vector<Expression *> valuelist;
    
  public:
    ASTTrait(string name, Expression *value=NULL) : name(name), value(value)
    {
    }
    ASTTrait(string name, ParseVector<Expression*> _valuelist)
    : name(name), valuelist(_valuelist.begin(), _valuelist.end())
    {
        value = NULL;//to make valgrind happy
    }
    virtual ~ASTTrait()
    {
        delete value;
        for (size_t i=0; i<valuelist.size(); i++) {
            if(valuelist[i]) delete(valuelist[i]);
        }
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "As "<<name<< (value ? " with value:" : "") << endl;
        if (value)
            value->Print(out, indent+1);
    }
    virtual void Export(ostream &out, int indent = 0) const
    {
        out << name;
        if (value)
            out << "(" << value->GetText(Expression::ASPEN) << ")";
    }
    virtual void Visit(ModelVisitor *visitor)
    {
        if (visitor->Visit(this))
            return;

        if (value)
            value->Visit(visitor);
    }

    const string &GetName() const { return name; }
    const Expression *GetValue() const { return value; }
    const vector<Expression *>& GetValueList() const{ return valuelist;}
};

#endif
