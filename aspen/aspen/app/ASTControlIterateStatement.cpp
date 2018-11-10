// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "app/ASTControlIterateStatement.h"
#include "walkers/AspenTool.h"

ASTControlIterateStatement::ASTControlIterateStatement(string label, Identifier *counter, Expression *q, ASTControlStatement *item)
    : ASTControlStatement(label), counter(counter), quantity(q), item(item)
{
}

ASTControlIterateStatement::~ASTControlIterateStatement()
{
    delete quantity;
    delete item;
}

void
ASTControlIterateStatement::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "Control node '"<<nodeid<<"': iterate:" << endl;
    out << Indent(indent+1) << "Quantity:" << endl;
    quantity->Print(out, indent+2);
    item->Print(out, indent+1);
}

void
ASTControlIterateStatement::Export(ostream &out, int indent) const
{
    out << Indent(indent) << "iterate ["<<quantity->GetText(Expression::ASPEN)<<"] ";
    if (label != "")
        out << "\"" << label << "\" ";
    out << "{" << endl;
    item->Export(out, indent+1);
    out << Indent(indent) << "}" << endl;
}

Expression*
ASTControlIterateStatement::GetResourceRequirementExpression(const ASTAppModel *app,
                                     string resource) const
{
    return new BinaryExpr("*", quantity->Cloned(),
                                   item->GetResourceRequirementExpression(app, resource));
}

set<string>
ASTControlIterateStatement::GetInclusiveDataArrays(const ASTAppModel *app) const
{
    return item->GetInclusiveDataArrays(app);
}

void
ASTControlIterateStatement::Traverse(AspenTool *tool) const
{
    AspenTool::TraversalMode mode = tool->GetTraversalMode(this);
    tool->EnterIterate(mode, this);

    if (mode == AspenTool::Explicit)
    {
        //Leak
        //double q = quantity->Expanded(*tool->paramStack)->Evaluate();
        double q = quantity->EvaluateWithExpansion(*tool->paramStack);

        for (int i=0; i<q; ++i)
        {
            if (counter)
            {
                ///\todo: Is this the best way to accomplish this?
                tool->paramStack->LocalStack.push_back(NameMap<const Expression*>());
                tool->paramStack->LocalStack.back()[counter->GetName()] = new Real(i);
            }

            cout <<"\tEnterIterate "<< i<< "/" << q << endl;

            item->Traverse(tool);
            if (counter)
            {
                delete (tool->paramStack->LocalStack.back()[counter->GetName()]);
                tool->paramStack->LocalStack.pop_back();
            }

	    cout << "\tEndIterate:"<< i<< "/" << q << endl;

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

    tool->LeaveIterate(mode, this);
}

void
ASTControlIterateStatement::Visit(ModelVisitor *visitor)
{
    if (visitor->Visit(this))
        return;
    quantity->Visit(visitor);
    item->Visit(visitor);
}

bool
ASTControlIterateStatement::ContainsRandomSample(ASTAppModel *app) const
{
    return item->ContainsRandomSample(app);
}
