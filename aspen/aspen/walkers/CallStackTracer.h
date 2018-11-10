#ifndef CALL_STACK_TRACER_H
#define CALL_STACK_TRACER_H

/**
* @file     CallStackTracer.h
*
* @author   Jeremy Meredith
*           jsmeredith@gmail.com
*
* @brief CallStackTracer tool class definition
*
*/

#include "AspenTool.h"

class CallStackTracer : public AspenTool
{
    vector<string> stack;
  public:
    CallStackTracer(ASTAppModel *app) : AspenTool(app)
    {
    }
  protected:
    virtual TraversalMode TraversalModeHint(const ASTControlStatement *here)
    {
        //return Explicit;
        return Implicit;
    }
    virtual void StartKernel(TraversalMode mode, const ASTKernel *k)
    {
        stack.push_back(k->GetName());
    }

    virtual void EndKernel(TraversalMode mode, const ASTKernel *s)
    {
        stack.pop_back();
    }

  public:
    virtual vector<string> GetStackAsStringVector()
    {
        return stack;
    }
    virtual bool EmptyStack()
    {
        return stack.empty();
    }
    virtual string GetTopCaller()
    {
        if (stack.size() < 1)
            return "__START__";
        return stack.back();
    }
    virtual string GetStackAsString()
    {
        string result;
        for (unsigned int i=0; i<stack.size(); i++)
        {
            if (i>0)
                result += "\n";
            result += stack[i];
        }
        return result;
    }
};

#endif
