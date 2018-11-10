// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
// ****************************************************************************
// Class:     ASTMapStatement
//
// Purpose:   Map Application Data/Kernel to Machine Components
//
// Programmer:  Ivy Bo PENG
// Creation:    Nov, 2017
//
// Modifications:
// ****************************************************************************

#ifndef AST_MAP_STATEMENT_H
#define AST_MAP_STATEMENT_H

#include "model/ASTStatement.h"
#include "expr/Identifier.h"

enum MapType{KERNEL, DATA};

class ASTMapStatement : public ASTStatement
{

 public:
  MapType       type;
  string        app_comp_str;
  Identifier    *app_comp;
  string        mach_comp_str;
  Identifier    *mach_comp;
  

  
  ASTMapStatement(Identifier *_app, Identifier *_mach)
     :app_comp(_app), mach_comp(_mach){  }
  
  ~ASTMapStatement()
    { }

  void Print(ostream &out, int indent = 0)const;
  void Visit(ModelVisitor *visitor){}
};

#endif
