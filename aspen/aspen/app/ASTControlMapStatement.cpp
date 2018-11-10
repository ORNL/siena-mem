// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "app/ASTControlMapStatement.h"
#include "walkers/AspenTool.h"

ASTControlMapStatement::ASTControlMapStatement(string label, Identifier *counter, Expression *q, ASTControlStatement *item)
    : ASTControlStatement(label), counter(counter), quantity(q), item(item)
{
}

ASTControlMapStatement::~ASTControlMapStatement()
{
    delete quantity;
    delete item;
}

void
ASTControlMapStatement::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "Control node '"<<nodeid<<"': map:" << endl;
    out << Indent(indent+1) << "Quantity:" << endl;
    quantity->Print(out, indent+2);
    item->Print(out, indent+1);
}

void
ASTControlMapStatement::Export(ostream &out, int indent) const
{
    out << Indent(indent) << "map ["<<quantity->GetText(Expression::ASPEN)<<"] ";
    if (label != "")
        out << "\"" << label << "\" ";
    out << "{" << endl;
    item->Export(out, indent+1);
    out << Indent(indent) << "}" << endl;
}

Expression*
ASTControlMapStatement::GetResourceRequirementExpression(const ASTAppModel *app,
                                                     string resource) const
{
    return new BinaryExpr("*", quantity->Cloned(),
                                   item->GetResourceRequirementExpression(app, resource));
}

set<string>
ASTControlMapStatement::GetInclusiveDataArrays(const ASTAppModel *app) const
{
    return item->GetInclusiveDataArrays(app);
}

void
ASTControlMapStatement::Traverse(AspenTool *tool) const
{
    AspenTool::TraversalMode mode = tool->GetTraversalMode(this);

    tool->EnterMap(mode, this);

    if (mode == AspenTool::Explicit)
    {
        double q = quantity->Expanded(*tool->paramStack)->Evaluate();
        for (int i=0; i<q; ++i)
        {
            if (counter)
            {
                ///\todo: Is this the best way to accomplish this?
                tool->paramStack->LocalStack.push_back(NameMap<const Expression*>());
                tool->paramStack->LocalStack.back()[counter->GetName()] = new Real(i);
            }
            item->Traverse(tool);
            if (counter)
            {
                tool->paramStack->LocalStack.pop_back();
            }
        }
    }
    else if (mode == AspenTool::Implicit)
    {
        item->Traverse(tool);
    }
    else
    {
        THROW(LogicError, "Expected Implicit or Explicit traversal");
    }

    tool->LeaveMap(mode, this);
}

void
ASTControlMapStatement::Visit(ModelVisitor *visitor)
{
    if (visitor->Visit(this))
        return;
    quantity->Visit(visitor);
    item->Visit(visitor);
}

bool
ASTControlMapStatement::ContainsRandomSample(ASTAppModel *app) const
{
    return item->ContainsRandomSample(app);
}

