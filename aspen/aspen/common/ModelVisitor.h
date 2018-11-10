// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef MODEL_VISITOR_H
#define MODEL_VISITOR_H

#include "common/AST.h"

class ModelVisitor
{
  public:
    // return true to stop traversing this subtree
    virtual bool Visit(ASTNode *astnode) = 0;
};


#endif
