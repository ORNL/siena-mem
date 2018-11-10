// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "expr/SequenceExpr.h"

#include "expr/ArrayDereference.h"
#include "expr/BinaryExpr.h"
#include "expr/Identifier.h"
#include "expr/Real.h"

SequenceExpr::SequenceExpr(string op,
                           Expression *a,
                           Expression *b,
                           Expression *c,
                           Expression *d)
    : op(op)
{
    if (a)
        entries.push_back(a);
    if (b)
        entries.push_back(b);
    if (c)
        entries.push_back(c);
    if (d)
        entries.push_back(d);
}

SequenceExpr::~SequenceExpr()
{
#ifdef EXPRTRACE
    cerr << "deleting "<<this<<endl;
#endif
    for (unsigned int i=0; i<entries.size(); ++i)
    {
        delete entries[i];
    }
}

int
SequenceExpr::GetDepth() const
{
    int depth = 0;
    for (unsigned int i=0; i<entries.size(); ++i)
        depth = std::max(depth, entries[i]->GetDepth());
    return depth + 1;
}

bool
SequenceExpr::Equivalent(const Expression *e) const
{
    const SequenceExpr *s = dynamic_cast<const SequenceExpr*>(e);
    if (!s)
        return false;
    if (s->entries.size() != entries.size())
        return false;
    if (s->op != op)
        return false;
    for (unsigned int i=0; i<entries.size(); ++i)
    {
        if (! entries[i]->Equivalent(s->entries[i]))
            return false;
    }
    return true;
}

Expression*
SequenceExpr::OneStepSimplified() const
{
    if (entries.size() == 1)
        return entries[0]->OneStepSimplified();

    // if we contain matching binary/sequence expressions, or
    // if we have more than one value, then we can do some
    // straightforward optimizations.
    bool canSubsume = false;
    int valueCount = 0;
    bool identity = false;
    for (unsigned int i=0; i<entries.size(); ++i)
    {
        Expression *t = entries[i];
        Value *tv = dynamic_cast<Value*>(t);
        BinaryExpr *tb = dynamic_cast<BinaryExpr*>(t);
        SequenceExpr *ts = dynamic_cast<SequenceExpr*>(t);
        if (tv)
        {
            ++valueCount;
            if ((tv->GetValue() == 1 && op == "*") ||
                (tv->GetValue() == 0 && op == "+"))
                identity = true;
        }
        else if (tb && tb->op == op)
            canSubsume = true;
        else if (ts && ts->op == op)
            canSubsume = true;
    }

    // straightforward optimizations            
    if (canSubsume || identity || valueCount > 1)
    {
        SequenceExpr *expr = new SequenceExpr(op);
        double value;
        bool foundvalue = false;
        for (unsigned int i=0; i<entries.size(); ++i)
        {
            Expression *t = entries[i];
            Value *tv = dynamic_cast<Value*>(t);
            BinaryExpr *tb = dynamic_cast<BinaryExpr*>(t);
            SequenceExpr *ts = dynamic_cast<SequenceExpr*>(t);
            if (tv)
            {
                // if we found a value, combine it into our combined value
                double v = tv->GetValue();
                if (!foundvalue)
                {
                    foundvalue = true;
                    value = v;
                }
                else
                {
                    if (op == "*")
                        value *= v;
                    else if (op == "+")
                        value += v;
                    else if (op == "max")
                        value = std::max(value,v);
                    else if (op == "min")
                        value = std::min(value,v);
                    else
                        THROW(GrammaticalError, "Unknown operator : ", op);
                }
            }
            else if (tb && tb->op == op)
            {
                // can merge in a matching binary expression
                expr->entries.push_back(tb->lhs->OneStepSimplified());
                expr->entries.push_back(tb->rhs->OneStepSimplified());
            }
            else if (ts && ts->op == op)
            {
                // can merge in a matching sequence expression
                for (unsigned int i=0; i<ts->entries.size(); ++i)
                    expr->entries.push_back(ts->entries[i]->OneStepSimplified());
            }
            else 
            {
                expr->entries.push_back(t->OneStepSimplified());
            }
        }
        if (foundvalue)
        {
            if (op == "*" && value==1 && expr->entries.size() > 0)
                ; // identity
            else if (op == "+" && value==0 && expr->entries.size() > 0)
                ; // identity
            else
                expr->entries.push_back(new Real(value));
        }

        return expr;
    }

    // no more straightforward simplifications

    // factor out the first identifier we see from terms
    if (op == "+" && entries.size() > 2)
    {
        string id = "";
        for (unsigned int i=0; i<entries.size(); ++i)
        {
            Expression *t = entries[i];
            Identifier *ti = dynamic_cast<Identifier*>(t);
            BinaryExpr *tb = dynamic_cast<BinaryExpr*>(t);
            SequenceExpr *ts = dynamic_cast<SequenceExpr*>(t);
            if (ti)
            {
                id = ti->GetName();
                break;
            }
            if (tb && tb->op=="*" && dynamic_cast<Identifier*>(tb->lhs))
            {
                id = dynamic_cast<Identifier*>(tb->lhs)->GetName();
                break;
            }
            if (tb && tb->op=="*" && dynamic_cast<Identifier*>(tb->rhs))
            {
                id = dynamic_cast<Identifier*>(tb->rhs)->GetName();
                break;
            }
            if (ts && ts->op=="*")
            {
                for (unsigned int j=0; j<ts->entries.size(); ++j)
                {
                    if (dynamic_cast<Identifier*>(ts->entries[j]))
                    {
                        id = dynamic_cast<Identifier*>(ts->entries[j])->GetName();
                        break;
                    }
                }
                if (id != "")
                    break;
            }
        }
        if (id != "")
        {
            SequenceExpr *withID = new SequenceExpr(op);
            SequenceExpr *withoutID = new SequenceExpr(op);

            bool hadRawID = false;
            for (unsigned int i=0; i<entries.size(); ++i)
            {
                Expression *t = entries[i];
                Identifier *ti = dynamic_cast<Identifier*>(t);
                BinaryExpr *tb = dynamic_cast<BinaryExpr*>(t);
                SequenceExpr *ts = dynamic_cast<SequenceExpr*>(t);
                if (ti && ti->GetName() == id)
                {
                    hadRawID = true;
                    withID->entries.push_back(new Real(1));
                }
                else if (tb && tb->op=="*" &&
                    dynamic_cast<Identifier*>(tb->lhs) &&
                    dynamic_cast<Identifier*>(tb->lhs)->GetName() == id)
                {
                    withID->entries.push_back(tb->rhs->OneStepSimplified());
                }
                else if (tb && tb->op=="*" &&
                    dynamic_cast<Identifier*>(tb->rhs) &&
                    dynamic_cast<Identifier*>(tb->rhs)->GetName() == id)
                {
                    withID->entries.push_back(tb->lhs->OneStepSimplified());
                }
                else if (ts && ts->op=="*")
                {
                    int index = -1;
                    for (unsigned int j=0; j<ts->entries.size(); ++j)
                    {
                        if (dynamic_cast<Identifier*>(ts->entries[j]) &&
                            dynamic_cast<Identifier*>(ts->entries[j])->GetName() == id)
                        {
                            index = j;
                            break;
                        }
                    }
                    if (index != -1)
                    {
                        SequenceExpr *newseq = new SequenceExpr(ts->op);
                        for (int j=0; j<index; ++j)
                            newseq->entries.push_back(ts->entries[j]->OneStepSimplified());
                        for (unsigned int j=index+1; j<ts->entries.size(); ++j)
                            newseq->entries.push_back(ts->entries[j]->OneStepSimplified());
                        withID->entries.push_back(newseq);
                    }
                    else
                    {
                        withoutID->entries.push_back(t->OneStepSimplified());
                    }
                }
                else
                {
                    withoutID->entries.push_back(t->OneStepSimplified());
                }
            }

            Expression *newterm = NULL;
            if (withID->entries.size() == 1)
            {
                if (hadRawID)
                {
                    newterm = new Identifier(id);
                }
                else
                {
                    newterm = new BinaryExpr("*",
                                                new Identifier(id),
                                                withID->entries[0]->Cloned());
                }
                delete withID;
            }
            else
            {
                newterm = new BinaryExpr("*",
                                                  new Identifier(id),
                                                  withID);
            }


            // add the new term into the entries that didn't contain the factor
            if (withoutID->entries.size() == 0)
            {
                delete withoutID;
                return newterm;
            }
            else
            {
                withoutID->entries.push_back(newterm);
                return withoutID;
            }
        }
    }

    // no factorization either, just copy
    SequenceExpr *expr = new SequenceExpr(op);
    for (unsigned int i=0; i<entries.size(); ++i)
        expr->entries.push_back(entries[i]->OneStepSimplified());
    return expr;
}

Expression*
SequenceExpr::Cloned() const
{
    SequenceExpr *expr = new SequenceExpr(op);
    for (unsigned int i=0; i<entries.size(); ++i)
    {
        expr->entries.push_back(entries[i]->Cloned());
    }
    return expr;
}

void
SequenceExpr::ExpandInPlace(const ParameterStack &paramMap, set<string> expandedset)
{
    for (unsigned int i=0; i<entries.size(); ++i)
    {
        Expression *e = entries[i];
        if (dynamic_cast<Identifier*>(e) ||
            dynamic_cast<ArrayDereference*>(e))
        {
            entries[i] = e->Expanded(paramMap, expandedset);
            delete e;
        }
        else
        {
            entries[i]->ExpandInPlace(paramMap, expandedset);
        }
    }
}

Expression*
SequenceExpr::Expanded(const ParameterStack &paramMap, set<string> expandedset) const
{
    SequenceExpr *expr = new SequenceExpr(op);
    for (unsigned int i=0; i<entries.size(); ++i)
    {
        expr->entries.push_back(entries[i]->Expanded(paramMap, expandedset));
    }
    return expr;
}

void
SequenceExpr::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "Sequence "<<op<<endl;
    for (unsigned int i=0; i<entries.size(); ++i)
    {
        out << Indent(indent+1) << "Entry "<<i<<endl;
        entries[i]->Print(out,indent+2);
    }
}

double
SequenceExpr::Evaluate() const
{
    if (entries.size() == 0)
        THROW(GrammaticalError, "No entries in sequence");

    double val = entries[0]->Evaluate();
    if (op == "+") 
    {
        for (unsigned int i=1; i<entries.size(); ++i)
            val += entries[i]->Evaluate();
    }
    else if (op == "*") 
    {
        for (unsigned int i=1; i<entries.size(); ++i)
            val *= entries[i]->Evaluate();
    }
    else if (op == "max")
    {
        for (unsigned int i=1; i<entries.size(); ++i)
        {
            double newval = entries[i]->Evaluate();;
            val = std::max(val, newval);
        }
    }
    else if (op == "min")
    {
        for (unsigned int i=1; i<entries.size(); ++i)
        {
            double newval = entries[i]->Evaluate();;
            val = std::min(val, newval);
        }
    }
    else
        THROW(GrammaticalError, "Unknown operator : ", op);

    return val;
}    

string
SequenceExpr::GetText(TextStyle style) const
{
    if (entries.size() == 0)
        THROW(GrammaticalError, "No entries in sequence");

    if (op == "max" || op == "min")
    {
        string s = op + "(";
        for (unsigned int i=0; i<entries.size(); ++i)
        {
            s += entries[i]->GetText(style);
            if (i < entries.size()-1)
                s += ", ";
        }
        s += ")";
        return s;
    }
    else
    {
        string s = "(";
        for (unsigned int i=0; i<entries.size(); ++i)
        {
            if (i != 0)
                s += " " + op + " ";
            bool needparens = true;
            Expression *e = entries[i];
            if (dynamic_cast<Value*>(e) ||
                dynamic_cast<Identifier*>(e))
                needparens = false;

            if (needparens)
                s += "(" + e->GetText(style) + ")";
            else
                s += e->GetText(style);
        }
        s += ")";
        return s;
    }
}

void
SequenceExpr::Visit(ModelVisitor *visitor)
{
    if (visitor->Visit(this))
        return;
    for (unsigned int i=0; i<entries.size(); ++i)
        entries[i]->Visit(visitor);
}
