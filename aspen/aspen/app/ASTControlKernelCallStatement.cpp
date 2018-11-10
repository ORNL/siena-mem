// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "app/ASTControlKernelCallStatement.h"
#include "walkers/AspenTool.h"

ASTControlKernelCallStatement::ASTControlKernelCallStatement(Identifier *id)
    : ASTControlStatement(), ident(id)
{
}

ASTControlKernelCallStatement::ASTControlKernelCallStatement(Identifier *id, ParseVector<Expression*> args)
    : ASTControlStatement(), ident(id), arguments(args.begin(), args.end())
{
}

ASTControlKernelCallStatement::~ASTControlKernelCallStatement()
{
    for (unsigned int i=0; i<arguments.size(); i++)
        delete arguments[i];
}

void
ASTControlKernelCallStatement::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "Control node '"<<nodeid<<"': kernel call "<<ident->GetName();
    if (arguments.size() > 0)
    {
        out << " with "<<arguments.size()<<" args:" << endl;
        for (unsigned int i=0; i<arguments.size(); i++)
        {
            arguments[i]->Print(out, indent+1);
        }
    }
    else
        out << endl;
}

void
ASTControlKernelCallStatement::Export(ostream &out, int indent) const
{
    out << Indent(indent);
    out << "call " << ident->GetName();
    out << "(";
    if (arguments.size() > 0)
    {
        for (unsigned int i=0; i<arguments.size(); i++)
        {
            out << arguments[i]->GetText(Expression::ASPEN);
            if (i+1<arguments.size())
                out << ", ";
        }
    }
    out << ")";
    out << endl;
}

const ASTKernel*
ASTControlKernelCallStatement::GetKernel(const ASTAppModel *app) const
{
    if (!app)
        return NULL;

    return app->GetKernel(ident->GetName());
}

Expression*
ASTControlKernelCallStatement::GetResourceRequirementExpression(const ASTAppModel *app,
                                         string resource) const
{
    const ASTKernel *k = GetKernel(app);

    // If we can't find what we're looking for, just return the name.
    // (This is intentional; a NULL app means do no expansion.)
    if (!k)
        return ident->Cloned();

    if (k->GetArgs().size() != arguments.size())
        THROW(ModelError, "Argument mismatch for kernel call " + ident->GetName());

    Expression *expr = k->GetResourceRequirementExpression(app,resource);

    if (k->GetArgs().size() > 0)
    {
        NameMap<const Expression*> callsubst;
        for (unsigned int i=0; i<arguments.size(); ++i)
        {
            if (k->GetArgs()[i]->GetType() == "param")
                callsubst[k->GetArgs()[i]->GetName()] = arguments[i];
        }
        if (!callsubst.Empty())
            expr = new CallStackSubstitution(callsubst, expr);
    }

    return expr;
}

set<string>
ASTControlKernelCallStatement::GetInclusiveDataArrays(const ASTAppModel *app) const
{
    set<string> arrays;

    // If we can't find what we're looking for, just return the name.
    // (This is intentional; a NULL app means do no expansion.)
    if (!app)
        return arrays;

    if (app->kernelMap.Contains(ident->GetName()))
    {
        return app->kernelMap[ident->GetName()]->GetInclusiveDataArrays(app);
    }

    return arrays;
}


void
ASTControlKernelCallStatement::Traverse(AspenTool *tool) const
{
    AspenTool::TraversalMode mode = tool->GetTraversalMode(this);

    tool->EnterKernelCall(mode, this);

    const ASTKernel *k = tool->GetKernelForCall(ident->GetName());

    if (!k)
        THROW(ModelError, "Unknown kernel: ", ident->GetName());

    k->Traverse(tool);

    tool->LeaveKernelCall(mode, this);
}


void
ASTControlKernelCallStatement::Visit(ModelVisitor *visitor)
{
    if (visitor->Visit(this))
        return;
    for (unsigned int i=0; i<arguments.size(); ++i)
        arguments[i]->Visit(visitor);
}


bool
ASTControlKernelCallStatement::ContainsRandomSample(ASTAppModel *app) const
{
    const ASTKernel *k = GetKernel(app);

    if (!k)
        THROW(ModelError, "Unknown kernel: ", ident->GetName());

    return k->ContainsRandomSample(app);
}
