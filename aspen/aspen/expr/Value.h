// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef VALUE_H
#define VALUE_H

#include "expr/Expression.h"

class Value : public Expression
{
  public:
    virtual double GetValue() const = 0;
};

#endif
