// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef PARAMETER_STACK_H
#define PARAMETER_STACK_H

#include "common/NameMap.h"
#include "expr/Expression.h"

struct ParameterStack
{
    bool localExpansionOnly;

    std::vector<const NameMap<const Expression*> *> GlobalValues;
    std::vector<NameMap<const Expression*> > LocalStack;

    ParameterStack()
        : localExpansionOnly(false)
    {
    }

    ParameterStack(const NameMap<const Expression*> &globals)
        : localExpansionOnly(false)
    {
        GlobalValues.push_back(&globals);
    }

    const Expression *Find(const std::string &name) const
    {
        int nlocal = LocalStack.size();
        for (int i=nlocal - 1; i >= 0 ; --i)
        {
            if (LocalStack[i].Contains(name))
                return LocalStack[i][name];
        }

        if (localExpansionOnly)
            return NULL;

        int nglobal = GlobalValues.size();
        for (int i=nglobal - 1; i >= 0 ; --i)
        {
            if (GlobalValues[i]->Contains(name))
                return (*GlobalValues[i])[name];
        }
        return NULL;
    }
    
};

#endif
