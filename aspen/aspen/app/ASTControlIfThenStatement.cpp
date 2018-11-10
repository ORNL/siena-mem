// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "app/ASTControlIfThenStatement.h"

#include "walkers/AspenTool.h"

ASTControlIfThenStatement::ASTControlIfThenStatement(Expression *comparison,
                                                     ASTControlStatement *thenitem,
                                                     ASTControlStatement *elseitem)
    : ASTControlStatement(), comparison(comparison), thenitem(thenitem), elseitem(elseitem)
{
}

ASTControlIfThenStatement::~ASTControlIfThenStatement()
{
    delete comparison;
    delete thenitem;
    if (elseitem)
        delete elseitem;
}

void
ASTControlIfThenStatement::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "Control node '"<<nodeid<<"': ifthen:" << endl;
    out << Indent(indent+1) << "Condition:" << endl;
    comparison->Print(out, indent+2);
    out << Indent(indent+1) << "Then:" << endl;
    thenitem->Print(out, indent+2);
    if (elseitem)
    {
        out << Indent(indent+1) << "Else:" << endl;
        elseitem->Print(out, indent+2);
    }
    else
    {
        out << Indent(indent+1) << "No Else for IfThen" << endl;
    }

}

void
ASTControlIfThenStatement::Export(ostream &out, int indent) const
{
    out << Indent(indent) << "if (" << comparison->GetText(Expression::ASPEN) << ") {" << endl;
    thenitem->Export(out, indent+1);
    out << Indent(indent) << "}" << endl;
    if (elseitem)
    {
        out << Indent(indent) << "else {" << endl;
        elseitem->Export(out, indent+1);
        out << Indent(indent) << "}" << endl;
    }
}

Expression*
ASTControlIfThenStatement::GetResourceRequirementExpression(const ASTAppModel *app,
                                                     string resource) const
{
    return new IfThenExpr(comparison->Cloned(),
                                   thenitem->GetResourceRequirementExpression(app, resource),
                                   elseitem ? elseitem->GetResourceRequirementExpression(app, resource) : new Real(0));
}

set<string>
ASTControlIfThenStatement::GetInclusiveDataArrays(const ASTAppModel *app) const
{
    set<string> arrays = thenitem->GetInclusiveDataArrays(app);
    if (elseitem)
    {
        set<string> newarrays = elseitem->GetInclusiveDataArrays(app);
        arrays.insert(newarrays.begin(), newarrays.end());
    }
    return arrays;
}

void
ASTControlIfThenStatement::Traverse(AspenTool *tool) const
{
    //AspenTool::TraversalMode mode = tool->GetTraversalMode(this);

    cerr << "Traversing IfThen " << label << endl;
    THROW(LogicError, "UNIMPLEMNENTED");
}

void
ASTControlIfThenStatement::Visit(ModelVisitor *visitor)
{
    if (visitor->Visit(this))
        return;

    comparison->Visit(visitor);
    thenitem->Visit(visitor);
    if (elseitem)
        elseitem->Visit(visitor);
}

bool
ASTControlIfThenStatement::ContainsRandomSample(ASTAppModel *app) const
{
    ///\todo: I assume we need to check all paths to see if they
    /// contain a random sample?  (Seems like we could which
    /// path we're going to traverse and then only check that one,
    /// at least in Explicit mode.  But maybe not possible, or
    /// maybe not worth the effort in any case.
    cerr << "Traversing IfThen " << label << endl;
    THROW(LogicError, "UNIMPLEMNENTED");
}

