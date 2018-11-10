// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "app/ASTControlProbabilityStatement.h"
#include "walkers/AspenTool.h"

ASTControlProbabilityStatement::ASTControlProbabilityStatement(ParseVector<ASTCaseItem*> items,
                                                               ASTControlStatement *elseitem)
    : ASTControlStatement(), items(items.begin(), items.end()), elseitem(elseitem)
{
    ///\todo: assert items->size() > 0?
}

ASTControlProbabilityStatement::~ASTControlProbabilityStatement()
{
    for (unsigned int i=0; i<items.size(); i++)
        delete items[i];
    if (elseitem)
        delete elseitem;
}

void
ASTControlProbabilityStatement::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "Control node '"<<nodeid<<"': probability:" << endl;
    for (unsigned int i=0; i<items.size(); i++)
        items[i]->Print(out, indent+1);
    if (elseitem)
    {
        out << Indent(indent+1) << "Else do:" << endl;
        elseitem->Print(out, indent+2);
    }
    else
    {
        out << Indent(indent+1) << "No Else for Probability" << endl;
    }
}

void
ASTControlProbabilityStatement::Export(ostream &out, int indent) const
{
    out << Indent(indent) << "probability {" << endl;
    for (unsigned int i=0; i<items.size(); i++)
        items[i]->Export(out, indent+1);
    if (elseitem)
    {
        out << Indent(indent+1) << "else {" << endl;
        elseitem->Export(out, indent+2);
        out << Indent(indent+1) << "}" << endl;
    }
}

Expression*
ASTControlProbabilityStatement::GetResourceRequirementExpression(const ASTAppModel *app,
                                              string resource) const
{
    Expression *sumprob = NULL;
    Expression *expr = NULL;

    for (unsigned int i=0; i<items.size(); ++i)
    {
        const Expression *qi = items[i]->GetQuantity();
        Expression *tmp = new BinaryExpr("*", qi->Cloned(),
                                         items[i]->GetItem()->GetResourceRequirementExpression(app, resource));
        if (expr)
            expr = new BinaryExpr("+", expr, tmp);
        else
            expr = tmp;
        // if we have an "else", we need to add up all the probabilities we encountered so far
        if (elseitem)
        {
            if (sumprob)
                sumprob = new BinaryExpr("+", sumprob, qi->Cloned());
            else
                sumprob = qi->Cloned();
        }
    }

    // if we have an else, add it here
    if (elseitem)
    {
        // the else probability is 1 - the sum of the other probabilities
        Expression *elseprob  = new BinaryExpr("-", new Real(1.0), sumprob);
        // weight the else item by that probability
        Expression *elseexpr = new BinaryExpr("*", elseprob, 
                                                          elseitem->GetResourceRequirementExpression(app, resource));
        // and add it to th final expression
        expr = new BinaryExpr("+", expr, elseexpr);
    }

    if (!expr)
        THROW(ModelError, "Couldn't create valid expression for probability statement");

    return expr;
}

set<string>
ASTControlProbabilityStatement::GetInclusiveDataArrays(const ASTAppModel *app) const
{
    set<string> arrays;
    for (unsigned int i=0; i<items.size(); ++i)
    {
        set<string> newarrays = items[i]->GetItem()->GetInclusiveDataArrays(app);
        arrays.insert(newarrays.begin(), newarrays.end());
    }
    return arrays;
}

void
ASTControlProbabilityStatement::Traverse(AspenTool *tool) const
{
    AspenTool::TraversalMode mode = tool->GetTraversalMode(this);

    tool->EnterProbability(mode, this);

    if (mode == AspenTool::Explicit)
    {
        double r = rand() / double(RAND_MAX);
        double s = 0;
        bool found = false;
        for (auto &i: items)
        {
            double q = i->GetQuantity()->Expanded(*tool->paramStack)->Evaluate();
            s += q;
            if (r <= s)
            {
                found = true;
                i->GetItem()->Traverse(tool);
                break;
            }
        }
        if (!found)
        {
            ///\todo: kind of hackish (here and below)
            if (elseitem)
            {
                elseitem->Traverse(tool);
            }
            else
            {
                ASTExecutionBlock eb;
                eb.Traverse(tool);
            }
        }            
    }
    else if (mode == AspenTool::Implicit)
    {
        for (auto &i: items)
            i->GetItem()->Traverse(tool);
        ///\todo: kind of hackish (here and above)
        if (elseitem)
        {
            elseitem->Traverse(tool);
        }
        else
        {
            ASTExecutionBlock eb;
            eb.Traverse(tool);
        }
    }
    else
    {
        THROW(LogicError, "Expected Implicit or Explicit traversal");
    }

    tool->LeaveProbability(mode, this);
}

void
ASTControlProbabilityStatement::Visit(ModelVisitor *visitor)
{
    if (visitor->Visit(this))
        return;

    for (auto &it: items)
        it->Visit(visitor);

    if (elseitem)
        elseitem->Visit(visitor);
}

bool
ASTControlProbabilityStatement::ContainsRandomSample(ASTAppModel *app) const
{
    ///\todo: I assume we need to check all paths to see if they
    /// contain a random sample?  (Seems like we could which
    /// path we're going to traverse and then only check that one,
    /// at least in Explicit mode.  But maybe not possible, or
    /// maybe not worth the effort in any case.)
    for (auto it: items)
    {
        if (it->GetItem()->ContainsRandomSample(app))
            return true;
    }

    if (elseitem)
        if (elseitem->ContainsRandomSample(app))
            return true;

    return false;
}

