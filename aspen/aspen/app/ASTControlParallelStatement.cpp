// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "app/ASTControlParallelStatement.h"

#include "walkers/AspenTool.h"

ASTControlParallelStatement::ASTControlParallelStatement(string label, ParseVector<ASTControlStatement*> items)
    : ASTControlStatement(label), items(items.begin(), items.end())
{
    ///\todo: assert items->size() > 0
}

ASTControlParallelStatement::~ASTControlParallelStatement()
{
    for (unsigned int i=0; i<items.size(); i++)
        delete items[i];
}

void
ASTControlParallelStatement::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "Control node '"<<nodeid<<"': parallel:" << endl;
    for (unsigned int i=0; i<items.size(); i++)
    {
        items[i]->Print(out, indent+1);
    }
}

void
ASTControlParallelStatement::Export(ostream &out, int indent) const
{
    out << Indent(indent) << "par ";
    if (label != "")
        out << "\"" << label << "\" ";
    out << "{" << endl;
    for (unsigned int i=0; i<items.size(); i++)
        items[i]->Export(out, indent+1);
    out << Indent(indent) << "}" << endl;
}

Expression*
ASTControlParallelStatement::GetResourceRequirementExpression(const ASTAppModel *app,
                                           string resource) const
{
    SequenceExpr *expr = new SequenceExpr("+");
    for (unsigned int i=0; i<items.size(); ++i)
    {
        expr->entries.push_back(items[i]->GetResourceRequirementExpression(app, resource));
    }
    return expr;
}

set<string>
ASTControlParallelStatement::GetInclusiveDataArrays(const ASTAppModel *app) const
{
    set<string> arrays;
    for (unsigned int i=0; i<items.size(); ++i)
    {
        set<string> newarrays = items[i]->GetInclusiveDataArrays(app);
        arrays.insert(newarrays.begin(), newarrays.end());
    }
    return arrays;
}

void
ASTControlParallelStatement::Traverse(AspenTool *tool) const
{
    AspenTool::TraversalMode mode = tool->GetTraversalMode(this);

    tool->EnterPar(mode, this);

    for (unsigned int i=0; i<items.size(); ++i)
        items[i]->Traverse(tool);

    tool->LeavePar(mode, this);
}

void
ASTControlParallelStatement::Visit(ModelVisitor *visitor)
{
    if (visitor->Visit(this))
        return;

    for (unsigned int i=0; i<items.size(); ++i)
        items[i]->Visit(visitor);
}

bool
ASTControlParallelStatement::ContainsRandomSample(ASTAppModel *app) const
{
    for (unsigned int i=0; i<items.size(); ++i)
        if (items[i]->ContainsRandomSample(app))
            return true;
    return false;
}

