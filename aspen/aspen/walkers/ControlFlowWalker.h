#ifndef CONTROL_FLOW_WALKER_H
#define CONTROL_FLOW_WALKER_H

#include "AspenTool.h"

class ControlFlowWalker : public AspenTool
{
    TraversalMode mode;
  public:
    ControlFlowWalker(ASTAppModel *app, TraversalMode m=Implicit)
        : AspenTool(app), mode(m)
    {
    }
  protected:
    virtual TraversalMode TraversalModeHint(const ASTControlStatement *here)
    {
        return mode;
    }
    virtual void StartKernel(TraversalMode mode, const ASTKernel *k)
    {
        cerr << Indent(level) << "Starting kernel '"<<k->GetName()<<"'\n";
    }

    virtual void StartKernelCall(TraversalMode mode, const ASTControlKernelCallStatement *s)
    {
        cerr << Indent(level) << "Starting kernel call '"<<s->GetFullyQualifiedName()<<"'\n";
    }

    virtual void StartIterate(TraversalMode mode, const ASTControlIterateStatement *s)
    {
        int c = s->GetQuantity()->Expanded(app->paramMap)->Evaluate();
        cerr << Indent(level) << "Starting iterate "<<s->GetLabel()<<" (count="<<c<<")\n";
    }
    
    virtual void StartMap(TraversalMode mode, const ASTControlMapStatement *s)
    {
        int c = s->GetQuantity()->Expanded(app->paramMap)->Evaluate();
        cerr << Indent(level) << "Starting map "<<s->GetLabel()<<" (count="<<c<<")\n";
    }
    
    virtual void StartSeq(TraversalMode mode, const ASTControlSequentialStatement *s)
    {
        cerr << Indent(level) << "Starting sequential "<<s->GetLabel()<<"\n";
    }
    
    virtual void StartPar(TraversalMode mode, const ASTControlParallelStatement *s)
    {
        cerr << Indent(level) << "Starting parallel "<<s->GetLabel()<<"\n";
    }

    virtual void StartProbability(TraversalMode mode, const ASTControlProbabilityStatement *s)
    {
        cerr << Indent(level) << "Starting probability ("
             << s->GetItems().size() << " + "
             << (s->GetElseItem() ? "1" : "0")
             << ") "
             << s->GetLabel() << "\n";
    }

    virtual void Execute(const ASTExecutionBlock *e)
    {
        cerr << Indent(level) << "Starting execution block "<<e->GetLabel()<<" (nres="<<e->GetStatements().size()<<")\n";
    }
};


#endif
