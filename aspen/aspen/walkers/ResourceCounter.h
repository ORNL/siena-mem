#ifndef RESOURCE_COUNTER_H
#define RESOURCE_COUNTER_H

#include "AspenTool.h"
#include "app/ASTRequiresStatement.h"

class ResourceCounter : public AspenTool
{
    bool debug;
    string resource;
  public:
    ResourceCounter(ASTAppModel *app, string res)
        : AspenTool(app), resource(res)
    {
        debug = true;
    }
    double GetResult()
    {
        if (stack.size() != 1)
            THROW(LogicError, "Stack size was not 1");
        return stack[0];
    }

  protected:
    vector<double> stack;
    void Push(double v)
    {
        stack.push_back(v);
    }
    const double Pop()
    {
        double v = stack.back();
        stack.pop_back();
        return v;
    }

  protected:
    virtual void StartKernel(TraversalMode mode, const ASTKernel *k)
    {
        if (debug) cerr << Indent(level) << "Starting kernel '"<<k->GetName()<<"'\n";
    }

    virtual void EndIterate(TraversalMode mode, const ASTControlIterateStatement *s)
    {
        double c = s->GetQuantity()->Expanded(*paramStack)->Evaluate();
        if (mode == Explicit)
        {
            if (debug) cerr << Indent(level) << "iterate, stack size="<<stack.size()<<", popping "<<c<<" items and pushing 1\n";
            double v = 0;
            for (int i=0; i<c; ++i)
                v += Pop();
            Push(v);
        }
        else
        {
            if (debug) cerr << Indent(level) << "iterate, stack size="<<stack.size()<<", popping a single item and pushing 1\n";
            double v = Pop();
            Push(c*v);
        }
    }
    
    virtual void EndMap(TraversalMode mode, const ASTControlMapStatement *s)
    {
        double c = s->GetQuantity()->Expanded(*paramStack)->Evaluate();
        if (mode == Explicit)
        {
            if (debug) cerr << Indent(level) << "map, stack size="<<stack.size()<<", popping "<<c<<" items and pushing 1\n";
            double v = 0;
            for (int i=0; i<c; ++i)
                v += Pop();
            Push(v);
        }
        else
        {
            if (debug) cerr << Indent(level) << "map, stack size="<<stack.size()<<", popping a single item and pushing 1\n";
            double v = Pop();
            Push(c*v);
        }
    }
    virtual void EndPar(TraversalMode mode, const ASTControlParallelStatement *s)
    {
        if (debug) cerr << Indent(level) << "par, stack size="<<stack.size()<<", popping "<<s->GetItems().size()<<" items and pushing 1\n";
        double v = 0;
        for (unsigned int i=0; i<s->GetItems().size(); ++i)
            v += Pop();
        Push(v);
    }
    virtual void EndSeq(TraversalMode mode, const ASTControlSequentialStatement *s)
    {
        if (debug) cerr << Indent(level) << "seq, stack size="<<stack.size()<<", popping "<<s->GetItems().size()<<" items and pushing 1\n";
        double v = 0;
        for (unsigned int i=0; i<s->GetItems().size(); ++i)
        {
            // don't pop sample statements.
            ///\todo: should probably make this test smarter than just "not a sample".
            if (!dynamic_cast<const ASTSampleStatement*>(s->GetItems()[i]))
            v += Pop();
        }
        Push(v);
    }
    virtual void EndKernel(TraversalMode mode, const ASTKernel *k)
    {
        if (debug) cerr << Indent(level) << "kernel, stack size="<<stack.size()<<"\n";
        // nothing to do for a kernel anymore; it now contains a single seq statement
    }
    virtual void EndProbability(TraversalMode mode, const ASTControlProbabilityStatement *s)
    {
        if (debug) cerr << Indent(level) << "probability, stack size="<<stack.size()<<"\n";
        if (mode == Explicit)
        {
            // explicit mode, we did one item with 100% probability
            // could push, then pull, but no point
        }
        else
        {
            // implicit mode, we evaluated all of them

            int n = s->GetItems().size();

            // calculate sum
            double sum;
            for (auto &it: s->GetItems())
                sum += it->GetQuantity()->Expanded(*paramStack)->Evaluate();

            // NOTE: pushing reversed the order, must pull off in reverse order

            // first is the "else"
            double result = (1.0 - sum) * Pop();

            // then the items in reverse order
            for (int i=n-1; i>=0; --i)
            {
                result += s->GetItems()[i]->GetQuantity()->Expanded(*paramStack)->Evaluate()
                    * Pop();
            }

            Push(result);
        }
    }


    virtual void Execute(const ASTExecutionBlock *e)
    {
        double value = 0;
        double par = e->GetParallelism()->Expanded(*paramStack)->Evaluate();
        for (unsigned int i=0; i<e->GetStatements().size(); ++i)
        {
            const ASTRequiresStatement *rs =
                dynamic_cast<const ASTRequiresStatement*>(e->GetStatements()[i]);
            if (rs && rs->GetResource() == resource)
                value += rs->GetQuantity()->Expanded(*paramStack)->Evaluate();
        }
        if (debug) cerr << Indent(level) << "execute, stack size="<<stack.size()<<", pushing 1 item\n";
        Push(par * value);
    }
};


#endif
