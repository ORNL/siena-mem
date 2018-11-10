// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "expr/BinaryExpr.h"

#include "expr/ArrayDereference.h"
#include "expr/Identifier.h"
#include "expr/Real.h"
#include "expr/SequenceExpr.h"

BinaryExpr::BinaryExpr(string op, Expression *lhs, Expression *rhs)
    : op(op), lhs(lhs), rhs(rhs)
{
    if (!lhs || !rhs)
    {
        THROW(LogicError, "Null value in binary op constructor");
    }
}

BinaryExpr::~BinaryExpr()
{
#ifdef EXPRTRACE
    cerr << "deleting "<<this<<endl;
#endif
    delete lhs;
    delete rhs;
}

int
BinaryExpr::GetPrecedence() const
{
    if (op == "+") return 1;
    if (op == "-") return 1;
    if (op == "*") return 2;
    if (op == "/") return 2;
    if (op == "^") return 4;
    THROW(GrammaticalError, "Unknown operator : ", op);
}

int
BinaryExpr::GetDepth() const
{
    return 1 + std::max(lhs->GetDepth(), rhs->GetDepth());
}

bool
BinaryExpr::Equivalent(const Expression *e) const
{
    const BinaryExpr *b = dynamic_cast<const BinaryExpr*>(e);
    if (!b)
        return false;
    if (b->op != op)
        return false;
    if (! b->lhs->Equivalent(lhs))
        return false;
    if (! b->rhs->Equivalent(rhs))
        return false;
    return true;
}

Expression*
BinaryExpr::Cloned() const
{
    return new BinaryExpr(op, lhs->Cloned(), rhs->Cloned());
}

void
BinaryExpr::ExpandInPlace(const ParameterStack &paramMap, set<string> expandedset)
{
    if (dynamic_cast<Identifier*>(lhs) ||
        dynamic_cast<ArrayDereference*>(lhs))
    {
        Expression *tmp = lhs;
        lhs = tmp->Expanded(paramMap, expandedset);
        delete tmp;
    }
    else
    {
        lhs->ExpandInPlace(paramMap, expandedset);
    }

    if (dynamic_cast<Identifier*>(rhs) ||
        dynamic_cast<ArrayDereference*>(rhs))
    {
        Expression *tmp = rhs;
        rhs = tmp->Expanded(paramMap, expandedset);
        delete tmp;
    }
    else
    {
        rhs->ExpandInPlace(paramMap, expandedset);
    }
}

Expression*
BinaryExpr::Expanded(const ParameterStack &paramMap, set<string> expandedset) const
{
    return new BinaryExpr(op, lhs->Expanded(paramMap, expandedset), rhs->Expanded(paramMap, expandedset));
}

Expression*
BinaryExpr::OneStepSimplified() const
{
    Value *lv = dynamic_cast<Value*>(lhs);
    Value *rv = dynamic_cast<Value*>(rhs);
    BinaryExpr *lb = dynamic_cast<BinaryExpr*>(lhs);
    BinaryExpr *rb = dynamic_cast<BinaryExpr*>(rhs);
    Value *lblv = lb ? dynamic_cast<Value*>(lb->lhs) : 0;
    Value *lbrv = lb ? dynamic_cast<Value*>(lb->rhs) : 0;
    Value *rblv = rb ? dynamic_cast<Value*>(rb->lhs) : 0;
    Value *rbrv = rb ? dynamic_cast<Value*>(rb->rhs) : 0;
    SequenceExpr *ls = dynamic_cast<SequenceExpr*>(lhs);
    SequenceExpr *rs = dynamic_cast<SequenceExpr*>(rhs);

    // add/subtract identities
    if (op == "+" && lv && lv->GetValue()==0)
        return rhs->OneStepSimplified();
    if ((op == "+" || op == "-") && rv && rv->GetValue()==0)
        return lhs->OneStepSimplified();

    // mul/div identities
    if (op == "*" && lv && lv->GetValue()==1)
        return rhs->OneStepSimplified();
    if ((op == "*" || op == "/") && rv && rv->GetValue()==1)
        return lhs->OneStepSimplified();

    // 0*x or x*0 is 0
    if (op == "*" && ((lv && lv->GetValue()==0) || (rv && rv->GetValue()==0)))
        return new Real(0);

    // 0/x is 0
    if (op == "/" && (lv && lv->GetValue()==0))
        return new Real(0);

    // turn division into multiplication
    if (op == "/" && rv)
    {
        return new BinaryExpr("*",
                                    lhs->OneStepSimplified(),
                                    new Real(1. / rv->GetValue()));
    }

    // distribute * over + if one of the * and one of the + terms is a value
    //  1: lhs is binary add, rhs is value
    if (op == "*" && rv && (lb && lb->op == "+"))
    {
        if (lblv)
            return new BinaryExpr("+",
                                        new Real(lblv->GetValue() *
                                                 rv->GetValue()),
                                        new BinaryExpr("*",
                                                             lb->rhs->OneStepSimplified(),
                                                             rv->Cloned()));
        if (lbrv)
            return new BinaryExpr("+",
                                        new Real(lbrv->GetValue() *
                                                 rv->GetValue()),
                                        new BinaryExpr("*",
                                                             lb->lhs->OneStepSimplified(),
                                                             rv->Cloned()));
    }
    //  2: rhs is binary add, lhs is value
    if (op == "*" && lv && (rb && rb->op == "+"))
    {
        if (rblv)
            return new BinaryExpr("+",
                                        new Real(rblv->GetValue() *
                                                 lv->GetValue()),
                                        new BinaryExpr("*",
                                                             rb->rhs->OneStepSimplified(),
                                                             lv->Cloned()));
        if (rbrv)
            return new BinaryExpr("+",
                                        new Real(rbrv->GetValue() *
                                                 lv->GetValue()),
                                        new BinaryExpr("*",
                                                             rb->lhs->OneStepSimplified(),
                                                             lv->Cloned()));
    }

    // if we have two values, just evaluate them (don't need a paramMap)
    if (lv && rv)
    {
        return new Real(Evaluate());
    }

    // combine chained factors/terms into explicit sequences
    // and convert binary ops with matching op child sequences
    // into single sequences.
    // NOTE!!!  This is the ONLY place sequence expressions are
    // used (as of 5/21/2014).  If we want to get rid of them entirely,
    // simply remove these next few simplifications.
    if ((op == "+" || op == "*") && rb && rb->op == op && lb && lb->op == op)
    {
        return new SequenceExpr(op,
                                      lb->lhs->OneStepSimplified(),
                                      lb->rhs->OneStepSimplified(),
                                      rb->lhs->OneStepSimplified(),
                                      rb->rhs->OneStepSimplified());
    }
    if ((op == "+" || op == "*") && lb && lb->op == op)
    {
        return new SequenceExpr(op,
                                      lb->lhs->OneStepSimplified(),
                                      lb->rhs->OneStepSimplified(),
                                      rhs->OneStepSimplified());
    }
    if ((op == "+" || op == "*") && rb && rb->op == op)
    {
        return new SequenceExpr(op,
                                      lhs->OneStepSimplified(),
                                      rb->lhs->OneStepSimplified(),
                                      rb->rhs->OneStepSimplified());
    }

    if (ls && ls->op == op)
    {
        // currently a safe assumption since we're using cloned instead of simplified
        SequenceExpr *seq = dynamic_cast<SequenceExpr*>(ls->Cloned());
        if (!seq)
            THROW(GrammaticalError, "Cloned sequence wasn't a sequence");
        seq->entries.push_back(rhs->OneStepSimplified());
        return seq;
    }

    if (rs && rs->op == op)
    {
        // currently a safe assumption since we're using cloned instead of simplified
        SequenceExpr *seq = dynamic_cast<SequenceExpr*>(rs->Cloned());
        if (!seq)
            THROW(GrammaticalError, "Cloned sequence wasn't a sequence");
        seq->entries.push_back(lhs->OneStepSimplified());
        return seq;
    }


    // no easy simplification
    return new BinaryExpr(op, lhs->OneStepSimplified(), rhs->OneStepSimplified());
}

void
BinaryExpr::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "Binary Op "<<op<<endl;
    out << Indent(indent+1) << "LHS"<<endl;
    lhs->Print(out,indent+2);
    out << Indent(indent+1) << "RHS"<<endl;
    rhs->Print(out,indent+2);
}

double
BinaryExpr::Evaluate() const
{
    double l = lhs->Evaluate();
    double r = rhs->Evaluate();
    if (op == "+") return l + r;
    if (op == "-") return l - r;
    if (op == "*") return l * r;
    if (op == "/") return l / r;
    if (op == "^") return pow(l,r);
    THROW(GrammaticalError, "Unknown operator: ", op);
}

string
BinaryExpr::GetText(TextStyle style) const
{
    ///\todo: is this too lenient on parentheses?
    // do some work to try to minimize unnecessary parentheses
    Value *lv = dynamic_cast<Value*>(lhs);
    Value *rv = dynamic_cast<Value*>(rhs);
    Identifier *li = dynamic_cast<Identifier*>(lhs);
    Identifier *ri = dynamic_cast<Identifier*>(rhs);
    BinaryExpr *lb = dynamic_cast<BinaryExpr*>(lhs);
    BinaryExpr *rb = dynamic_cast<BinaryExpr*>(rhs);

    bool commutative = (op=="+" || op=="*");
    bool lneedparens = false;
    bool rneedparens = false;
    if (lb && lb->GetPrecedence()<=GetPrecedence())
        lneedparens = true;
    if (rb && rb->GetPrecedence()<=GetPrecedence())
        rneedparens = true;
    if ((!lv && !li) && !commutative)
        lneedparens = true;
    if ((!rv && !ri) && !commutative)
        rneedparens = true;

    string lt = lhs->GetText(style);
    string rt = rhs->GetText(style);

    if (rneedparens)
        rt = "("+rt+")";
    if (lneedparens)
        lt = "("+lt+")";

    // adjust operator symbols for other systems (e.g. ** vs ^)
    // adjust spacing for visual clarity, too
    if (op == "^")
    {
        switch (style)
        {
          case ASPEN:   return lt + "^" + rt;
          case C:       return string("pow(") + lt + "," + rt + ")";
          case GNUPLOT: return lt + "**" + rt;
          default:
            std::stringstream ss;
            ss << "Unknown enum TextStyle : " << style;
            THROW(LogicError, ss.str());
        }
    }
    else if (op == "*")
    {
        return lt + op + rt;
    }
    else
    {
        return lt + " " + op + " " + rt;
    }
}

void
BinaryExpr::Visit(ModelVisitor *visitor)
{
    if (visitor->Visit(this))
        return;
    lhs->Visit(visitor);
    rhs->Visit(visitor);
}
