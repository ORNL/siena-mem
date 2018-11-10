// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_CASE_ITEM_H
#define AST_CASE_ITEM_H

#include "common/AST.h"
#include "expr/Expression.h"
#include "app/ASTControlStatement.h"

class ASTCaseItem : public ASTNode
{
  protected:
    Expression *quantity;
    ASTControlStatement *item;
  public:
    ASTCaseItem(Expression *quantity, ASTControlStatement *item);
    virtual ~ASTCaseItem();
    virtual void Print(ostream &out, int indent = 0) const;
    virtual void Export(ostream &out, int indent = 0) const;

    const Expression *GetQuantity() const { return quantity; }
    const ASTControlStatement *GetItem() const { return item; }

    virtual void Visit(ModelVisitor *visitor);
};

#endif
