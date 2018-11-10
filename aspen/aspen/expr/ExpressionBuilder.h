// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef EXPRESSION_BUILDER_H
#define EXPRESSION_BUILDER_H

#include "expr/Expression.h"

class ExpressionBuilder
{
  private:
    Expression *expr;
  public:
    ExpressionBuilder(Expression *expr = NULL) : expr(expr)
    {
    }
    Expression *GetExpression()
    {
        return expr;
    }
    bool Empty()
    {
        return expr == NULL;
    }
    void operator+=(Expression *e)
    {
        ///\todo: could convert binary expr to sequence expr
        SequenceExpr *se = dynamic_cast<SequenceExpr*>(expr);
        if (!expr)
        {
            expr = e;
        }
        else if (se && se->op == "+")
        {
            se->entries.push_back(e);
        }
        else
        {
            expr = new SequenceExpr("+", expr, e);
        }
    }
    void operator*=(Expression *e)
    {
        ///\todo: could convert binary expr to sequence expr
        SequenceExpr *se = dynamic_cast<SequenceExpr*>(expr);
        if (!expr)
        {
            expr = e;
        }
        else if (se && se->op == "*")
        {
            se->entries.push_back(e);
        }
        else
        {
            expr = new SequenceExpr("*", expr, e);
        }
    }
};

#endif
