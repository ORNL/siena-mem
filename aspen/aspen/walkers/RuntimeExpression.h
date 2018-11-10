#ifndef RUNTIME_EXPRESSION_H
#define RUNTIME_EXPRESSION_H

#include "walkers/AspenTool.h"
#include "model/ASTMachModel.h"
#include "app/ASTSampleStatement.h"
#include "expr/ExpressionBuilder.h"

class RuntimeExpression : public AspenTool
{
    string socket;
    bool debug;

    bool cache;
    map<const ASTExecutionBlock*, Expression*> expr_cache;
  public:
    RuntimeExpression(ASTAppModel *app,
                      ASTMachModel *mach,
                      string socket)
        : AspenTool(app, mach), socket(socket)
    {
        cache = false;
        debug = false;
        stack.reserve(1000);
    }
    Expression *GetResult()
    {
        if (stack.size() != 1)
            THROW(LogicError, "Stack size was not 1");
        return stack[0];
    }
    void SetCacheExecutionBlockExpressions(bool c)
    {
        cache = c;
        if (!cache)
            expr_cache.clear();
    }

  protected:
    vector<Expression*> stack;
    void Push(Expression *e)
    {
        stack.push_back(e);
    }
    Expression *Pop()
    {
        Expression *e = stack.back();
        stack.pop_back();
        return e;
    }

  protected:
    virtual void StartKernel(TraversalMode mode, const ASTKernel *k)
    {
        if (debug) cerr << Indent(level) << "Starting kernel '"<<k->GetName()<<"'\n";
    }

    virtual void EndIterate(TraversalMode mode, const ASTControlIterateStatement *s)
    {
        if (mode == Explicit)
        {
            double c = ExprPtr(s->GetQuantity()->Expanded(*paramStack))->Evaluate();
            if (debug) cerr << Indent(level) << "iterate, stack size="<<stack.size()<<", popping "<<c<<" items and pushing 1\n";
            SequenceExpr *e = new SequenceExpr("+");
            for (int i=0; i<c; ++i)
                e->entries.push_back(Pop());
            Push(e);
        }
        else
        {
            if (debug) cerr << Indent(level) << "iterate, stack size="<<stack.size()<<", popping a single item and pushing 1\n";
            Push(new BinaryExpr("*", s->GetQuantity()->Cloned(), Pop()));
        }
    }
    
    virtual void EndMap(TraversalMode mode, const ASTControlMapStatement *s)
    {
        if (mode == Explicit)
        {
            double c = ExprPtr(s->GetQuantity()->Expanded(*paramStack))->Evaluate();
            if (debug) cerr << Indent(level) << "map, stack size="<<stack.size()<<", popping "<<c<<" items and pushing 1\n";
            ///\todo: this is the only usage of ExpressionBuilder so far -- either
            /// update it and use it everywhere, or get rid of it
            ExpressionBuilder eb;
            for (int i=0; i<c; ++i)
                eb += Pop();
            Push(eb.GetExpression());
        }
        else
        {
            if (debug) cerr << Indent(level) << "map, stack size="<<stack.size()<<", popping a single item and pushing 1\n";
            Push(new BinaryExpr("*", s->GetQuantity()->Cloned(), Pop()));
        }
    }
    virtual void EndPar(TraversalMode mode, const ASTControlParallelStatement *s)
    {
        if (debug) cerr << Indent(level) << "par, stack size="<<stack.size()<<", popping "<<s->GetItems().size()<<" items and pushing 1\n";
        SequenceExpr *e = new SequenceExpr("+");
        for (unsigned int i=0; i<s->GetItems().size(); ++i)
            e->entries.push_back(Pop());
        Push(e);
    }
    virtual void EndSeq(TraversalMode mode, const ASTControlSequentialStatement *s)
    {
        if (debug) cerr << Indent(level) << "seq, stack size="<<stack.size()<<", popping "<<s->GetItems().size()<<" items and pushing 1\n";
        SequenceExpr *e = new SequenceExpr("+");
        for (unsigned int i=0; i<s->GetItems().size(); ++i)
        {
            // don't pop sample statements.
            ///\todo: should probably make this test smarter than just "not a sample".
            if (!dynamic_cast<const ASTSampleStatement*>(s->GetItems()[i]))
                e->entries.push_back(Pop());
        }
        Push(e);
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
        }
        else
        {
            // implicit mode, we evaluated all of them

            int n = s->GetItems().size();

            // calculate sum
            ExpressionBuilder sum;
            for (auto &it: s->GetItems())
                sum += it->GetQuantity()->Cloned();

            // NOTE: pushing reversed the order, must pull off in reverse order

            // first is the "else"
            ExpressionBuilder result;
            result += new BinaryExpr("*",
                                     new BinaryExpr("-", new Real(1), sum.GetExpression()),
                                     Pop());

            // then the items in reverse order
            for (int i=n-1; i>=0; --i)
            {
                const Expression *q = s->GetItems()[i]->GetQuantity();
                result += new BinaryExpr("*", q->Cloned(), Pop());
            }

            Push(result.GetExpression());
        }
    }

    virtual void Execute(const ASTExecutionBlock *e)
    {
        if (debug) cerr << Indent(level) << "execute, stack size="<<stack.size()<<", pushing 1 item\n";

        Expression *expr;

        map<const ASTExecutionBlock*, Expression*>::iterator it = expr_cache.end();
        if (cache)
            it = expr_cache.find(e);
        if (it == expr_cache.end())
        {
            //cerr << "calculating!" << endl;
            Expression *expr_tmp = e->GetTimeExpression(app, mach, socket);

            paramStack->localExpansionOnly = true;
            expr = expr_tmp->Expanded(*paramStack);
	//    paramStack->PrintKeys();
            paramStack->localExpansionOnly = false;
            delete expr_tmp;
            
            //cerr << "calculating using value for x = " << paramStack["x"]->Evaluate() << endl;
            if (cache)
                expr_cache[e] = expr;
        }
        else
        {
            expr = it->second;
        }
        Push(expr);
    }
};

#endif
