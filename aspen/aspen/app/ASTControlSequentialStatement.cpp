// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "app/ASTControlSequentialStatement.h"

#include "walkers/AspenTool.h"

ASTControlSequentialStatement::ASTControlSequentialStatement(string label, ParseVector<ASTControlStatement*> items)
    : ASTControlStatement(label), items(items.begin(), items.end())
{
    ///\todo: assert items->size() > 0
}

ASTControlSequentialStatement::~ASTControlSequentialStatement()
{
    for (unsigned int i=0; i<items.size(); i++)
        delete items[i];
}

void
ASTControlSequentialStatement::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "Control node '"<<nodeid<<"': sequential:" << endl;
    for (unsigned int i=0; i<items.size(); i++)
    {
        items[i]->Print(out, indent+1);
    }
}

void
ASTControlSequentialStatement::Export(ostream &out, int indent) const
{
    out << Indent(indent) << "seq ";
    if (label != "")
        out << "\"" << label << "\" ";
    out << "{" << endl;
    for (unsigned int i=0; i<items.size(); i++)
        items[i]->Export(out, indent+1);
    out << Indent(indent) << "}" << endl;
}

Expression*
ASTControlSequentialStatement::GetResourceRequirementExpression(const ASTAppModel *app,
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
ASTControlSequentialStatement::GetInclusiveDataArrays(const ASTAppModel *app) const
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
ASTControlSequentialStatement::Traverse(AspenTool *tool) const
{
    AspenTool::TraversalMode mode = tool->GetTraversalMode(this);

    tool->EnterSeq(mode, this);

    for (unsigned int i=0; i<items.size(); ++i)
        items[i]->Traverse(tool);

    tool->LeaveSeq(mode, this);
}

void
ASTControlSequentialStatement::Visit(ModelVisitor *visitor)
{
    if (visitor->Visit(this))
        return;

    for (unsigned int i=0; i<items.size(); ++i)
        items[i]->Visit(visitor);
}

bool
ASTControlSequentialStatement::ContainsRandomSample(ASTAppModel *app) const
{
    for (unsigned int i=0; i<items.size(); ++i)
        if (items[i]->ContainsRandomSample(app))
            return true;
    return false;
}

