// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "expr/Expression.h"

Expression*
Expression::Simplified() const
{
    int nsteps = 100;
    Expression *expr = Cloned();
    for (int i=0; i<nsteps; ++i)
    {
        //cerr << "\n\n----  STEP ----\n";
        Expression *newexpr = expr->OneStepSimplified();
        delete expr;
        expr = newexpr;
        //if (dynamic_cast<Value*>(expr))
        //    break;
        //expr->Print(cerr);
    }
    return expr;
}

// helper function; do the expansion, evaluate, and free the expansion
double
Expression::EvaluateWithExpansion(const ParameterStack &paramMap) const
{
    Expression *expanded = this->Expanded(paramMap);
    double value = expanded->Evaluate();
    delete expanded;
    return value;
}
