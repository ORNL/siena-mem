// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "ASTKernel.h"

#include "model/ASTDataStatement.h"
#include "model/ASTMachModel.h"
#include "app/ASTControlStatement.h"
#include "app/ASTControlSequentialStatement.h"
#include "app/ASTExecutionBlock.h"
#include "walkers/AspenTool.h"
#include "walkers/RuntimeExpression.h"

ASTKernel::ASTKernel(Identifier *id,
                     ParseVector<ASTControlStatement*> statements,
                     ParseVector<ASTKernelArgDecl*> args,
                     ParseVector<ASTStatement*> vars)
    : ident(id),
      args(args.begin(), args.end()),
      vars(vars.begin(), vars.end()),
      statements(new ASTControlSequentialStatement("", statements))
{
    if (!paramMap.Empty())
        return;

    for (unsigned int i=0; i<vars.size(); ++i)
    {
        ASTAssignStatement *assign = dynamic_cast<ASTAssignStatement*>(vars[i]);
        if (assign)
        {
            cerr << "Putting" << assign->GetName() << "in kernel paramMap" << endl;
            paramMap[assign->GetName()] = assign->GetValue();
        }
    }

}

ASTKernel::~ASTKernel()
{
    delete statements;
}

void
ASTKernel::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "Kernel "<<ident->GetName()<< ":" << endl;
    if (args.size() > 0)
    {
        out << Indent(indent+1) << "Args:" << endl;
        for (unsigned int i=0; i<args.size(); i++)
        {
            args[i]->Print(out, indent+2);
        }
    }
    if (vars.size() > 0)
    {
        for (unsigned int i=0; i<vars.size(); i++)
        {
            vars[i]->Print(out, indent+2);
        }
    }
    statements->Print(out, indent+1);
    /*
    out << Indent(indent+1) << "Statements:" << endl;
    for (unsigned int i=0; i<statements->GetItems().size(); i++)
    {
        statements->GetItems()[i]->Print(out, indent+2);
    }
    */
}

void
ASTKernel::Export(ostream &out, int indent) const
{
    out << Indent(indent) << "kernel "<<ident->GetName();
    if (args.size() > 0)
    {
        out << "(";
        for (unsigned int i=0; i<args.size(); i++)
        {
            args[i]->Export(out, indent);
            if (i+1<args.size())
                out << ", ";
        }
        out << ")";
    }
    out << " {" << endl;
    for (unsigned int i=0; i<statements->GetItems().size(); i++)
    {
        statements->GetItems()[i]->Export(out, indent+1);
    }
    out << Indent(indent) << "}" << endl;
}

Expression*
ASTKernel::GetResourceRequirementExpression(const ASTAppModel *app,
                                            string resource) const
{
    ExpressionBuilder eb;

    for (unsigned int i=0; i<statements->GetItems().size(); ++i)
    {
        const ASTControlStatement *s = statements->GetItems()[i];
        const ASTExecutionBlock *exec = dynamic_cast<const ASTExecutionBlock*>(s);
        const ASTControlStatement *ctrl = dynamic_cast<const ASTControlStatement*>(s);
        if (exec) // it's a kernel-like requires statement
        {
            ///\todo: now it's the same code as ctrl
            eb += exec->GetResourceRequirementExpression(app,resource);
        }
        else if (ctrl)
        {
            eb += ctrl->GetResourceRequirementExpression(app,resource);
        }
        else
        {
            //cerr << "UNKNOWN STMT in kernel '"<<ident->GetName()<<"': ";s->Print(cerr);
        }
    }

    if (eb.Empty())
        return new Real(0);

    return eb.GetExpression();
}

set<string>
ASTKernel::GetExclusiveDataArrays(string blockname) const
{
    set<string> arrays;

    // collect the list of data arrays
    for (unsigned int i=0; i<statements->GetItems().size(); ++i)
    {
        const ASTControlStatement *s = statements->GetItems()[i];
        const ASTExecutionBlock *exec = dynamic_cast<const ASTExecutionBlock*>(s);
        if (exec) // it's a kernel-like requires statement
        {
            // if they specified an execution block name, only count that block's arrays
            if (blockname == "" || blockname == exec->GetLabel())
            {
                set<string> newarrays = exec->GetDataArrays();
                arrays.insert(newarrays.begin(), newarrays.end());
            }
        }
    }

    return arrays;
}

set<string>
ASTKernel::GetInclusiveDataArrays(const ASTAppModel *app) const
{
    set<string> arrays = GetExclusiveDataArrays();

    // collect the list of data arrays
    for (unsigned int i=0; i<statements->GetItems().size(); ++i)
    {
        const ASTControlStatement *s = statements->GetItems()[i];
        set<string> newarrays = s->GetInclusiveDataArrays(app);
        arrays.insert(newarrays.begin(), newarrays.end());
    }

    return arrays;
}

Expression*
ASTKernel::GetExclusiveDataSizeExpression(const ASTAppModel *app, 
                                          string blockname) const
{
    set<string> arrays = GetExclusiveDataArrays(blockname);
    return GetDataSizeExpression(app, arrays);
}

Expression*
ASTKernel::GetInclusiveDataSizeExpression(const ASTAppModel *app) const
{
    set<string> arrays = GetInclusiveDataArrays(app);
    return GetDataSizeExpression(app, arrays);
}

///\todo: this whole function is a bit of a hack....
/// it doesn't support regions within other regions (or within other
/// control structures).
Expression*
ASTKernel::GetSingleRegionInclusiveDataSizeExpression(const ASTAppModel *app,
                                                      string label) const
{
    set<string> arrays;

    // collect the list of data arrays
    for (unsigned int i=0; i<statements->GetItems().size(); ++i)
    {
        const ASTControlStatement *s = statements->GetItems()[i];
        if (s->GetLabel() == label)
        {
            set<string> newarrays = s->GetInclusiveDataArrays(app);
            arrays.insert(newarrays.begin(), newarrays.end());
        }
    }

    return GetDataSizeExpression(app, arrays);
}

Expression*
ASTKernel::GetDataSizeExpression(const ASTAppModel *app, set<string> arrays) const
{
    ///\todo: we now have GetSingleArraySise in AppModel we could use.

    // for each data array, find its definition in the app global list
    Expression *expr = NULL;
    for (set<string>::iterator it = arrays.begin(); it != arrays.end(); it++)
    {
        string arrayname = *it;
        bool found = false;
        for (unsigned int i=0; i<app->GetGlobals().size(); ++i)
        {
            const ASTStatement *s = app->GetGlobals()[i];
            const ASTDataStatement *ds = dynamic_cast<const ASTDataStatement*>(s);
            if (!ds || ds->GetName() != arrayname)
                continue;
            found = true;
            // we now have the data array declaration of interest....
            Expression *newexpr = ds->GetSizeInBytes();

            if (expr)
                expr = new BinaryExpr("+", expr, newexpr);
            else
                expr = newexpr;
            break;
        }
        if (!found)
        {
            ///\todo: may need to do parameter substitution on DATA sets;
            /// we can't actually figure out the necessary array size
            /// until some kernels are called.  Otherwise, I'd like to 
            /// throw an error here.
            /// For now, how about returning a sentinel?
            cerr << "never found declaration for array "<<arrayname<<endl;
            return new Real(-9999);
            //throw "Missing array declaration";
        }
    }

    if (!expr)
        return new Real(0);
    else
        return expr;
}

void
ASTKernel::Traverse(AspenTool *tool) const
{
    ///\todo: do we really need this?
    /// I'd suggest EnterKernel (and Leave) don't need a traversal mode anymore
    /// now that they contain only a single item
    AspenTool::TraversalMode mode = tool->GetTraversalMode(statements);

    tool->EnterKernel(mode, this);
    statements->Traverse(tool);
    tool->LeaveKernel(mode, this);
}


bool
ASTKernel::ContainsRandomSample(ASTAppModel *app) const
{
    return statements->ContainsRandomSample(app);
}

void
ASTKernel::Visit(ModelVisitor *visitor)
{
    if (visitor->Visit(this))
        return;
    statements->Visit(visitor);
    for (unsigned int i=0; i<args.size(); ++i)
        args[i]->Visit(visitor);
}

///\todo: this is just a help method, probably can delete it?
Expression*
ASTKernel::GetTimeExpression(ASTAppModel *app,
                             ASTMachModel *mach,
                             string socket) const
{
    RuntimeExpression *re = new RuntimeExpression(app, mach, socket);
    re->SetCacheExecutionBlockExpressions(false);
    re->InitializeTraversal();
    this->Traverse(re);
    Expression *expr = re->GetResult();
    delete re;
    return expr;
}

///\todo: this is broken, because it doesn't descend into control
/// structures, it assumes any self-execution is at the top level,
/// which is NOT true.
///\todo: do we really need this anyway?  it's only used by gprof, so
/// we could probably just modify RuntimeExpression slightly to 
/// not descend into kernel calls.
Expression*
ASTKernel::GetSelfTimeExpression(ASTAppModel *app,
                                 ASTMachModel *mach,
                                 string socket) const
{
    SequenceExpr *expr = NULL;
    for (unsigned int i=0; i<statements->GetItems().size(); ++i)
    {
        const ASTControlStatement *s = statements->GetItems()[i];
        const ASTExecutionBlock *exec = dynamic_cast<const ASTExecutionBlock*>(s);
        if (exec) // it's a kernel-like requires statement
        {
            if (!expr)
                expr = new SequenceExpr("+");
            expr->entries.push_back(exec->GetTimeExpression(app,mach,socket));
        }
    }

    if (!expr)
        return new Real(0);

    return expr;
}
