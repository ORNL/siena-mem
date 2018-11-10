// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "app/ASTControlSwitchStatement.h"

ASTControlSwitchStatement::ASTControlSwitchStatement(Expression *value,
                                                     ParseVector<ASTCaseItem*> items,
                                                     ASTControlStatement *elseitem)
    : ASTControlStatement(), value(value), items(items.begin(), items.end()), elseitem(elseitem)
{
    ///\todo: assert items->size() > 0
    ///\todo (but not in this method): ensure either all enum values are covered or
    /// there must be an "else" clause.  (This would also mean we could use 'else' instead
    /// of the final if test, so there would be no missing 'else's.)
}

ASTControlSwitchStatement::~ASTControlSwitchStatement()
{
    for (unsigned int i=0; i<items.size(); i++)
        delete items[i];
    if (elseitem)
        delete elseitem;
}

void
ASTControlSwitchStatement::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "Control node '"<<nodeid<<"': Switch:" << endl;
    out << Indent(indent+1) << "Value:" << endl;
    value->Print(out, indent+2);
    for (unsigned int i=0; i<items.size(); i++)
        items[i]->Print(out, indent+1);
    if (elseitem)
    {
        out << Indent(indent+1) << "Else do:" << endl;
        elseitem->Print(out, indent+2);
    }
    else
    {
        out << Indent(indent+1) << "No Else for Switch" << endl;
    }
}

void
ASTControlSwitchStatement::Export(ostream &out, int indent) const
{
    out << Indent(indent) << "switch {" << endl;
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
ASTControlSwitchStatement::GetResourceRequirementExpression(const ASTAppModel *app,
                                              string resource) const
{
    Expression *expr = NULL;
    IfThenExpr *last = NULL;

    for (unsigned int i=0; i<items.size(); ++i)
    {
        IfThenExpr *tmp = new IfThenExpr(
            new Comparison("==", value->Cloned(),
                                        items[i]->GetQuantity()->Cloned()),
            items[i]->GetItem()->GetResourceRequirementExpression(app, resource),
            NULL);

        if (last)
            last->elseexpr = tmp;
        else
            expr = tmp;

        last = tmp;
    }

    // if we have an else, add it here
    if (elseitem)
    {
        last->elseexpr = elseitem->GetResourceRequirementExpression(app, resource);
    }

    if (!expr)
        THROW(ModelError, "Couldn't create valid expression for switch statement");

    return expr;
}

set<string>
ASTControlSwitchStatement::GetInclusiveDataArrays(const ASTAppModel *app) const
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
ASTControlSwitchStatement::Traverse(AspenTool *tool) const
{
    //AspenTool::TraversalMode mode = tool->GetTraversalMode(this);

    cerr << "Traversing Switch " << label << endl;
    THROW(LogicError, "UNIMPLEMNENTED");
}

void
ASTControlSwitchStatement::Visit(ModelVisitor *visitor)
{
    if (visitor->Visit(this))
        return;

    cerr << "Traversing Switch " << label << endl;
    THROW(LogicError, "UNIMPLEMNENTED");
}

bool
ASTControlSwitchStatement::ContainsRandomSample(ASTAppModel *app) const
{
    ///\todo: I assume we need to check all paths to see if they
    /// contain a random sample?  (Seems like we could which
    /// path we're going to traverse and then only check that one,
    /// at least in Explicit mode.  But maybe not possible, or
    /// maybe not worth the effort in any case.
    cerr << "Traversing Switch " << label << endl;
    THROW(LogicError, "UNIMPLEMNENTED");
}

