// Copyright 2013-2018 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_MACH_CONNECTIVITY_H
#define AST_MACH_CONNECTIVITY_H

#include "common/AST.h"
#include "expr/Expression.h"

/// example: "connect L1"
/// example: "connect DDR4,HBM"
class ASTMachConnectivity : public ASTNode
{
  
protected:
  const string name;
  //const Expression *value;
  vector<string> values;

public:
 ASTMachConnectivity(ParseVector<string> &_values) : name("connection"), values(_values.begin(),_values.end())
    {
    }
    virtual ~ASTMachConnectivity()
    {
      //if(value) delete value;
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Connect to "<< endl;
        out << Indent(indent+1);
        for(auto v :values)
             out << v << "  ";
        out <<endl;
        //if (value)
        //value->Print(out, indent+1);
    }
    const string &GetName() const { return name; }
    //const Expression *GetValue() const { return nullptr; }
    const vector<string> &GetValue() const { return values; }
};

#endif
