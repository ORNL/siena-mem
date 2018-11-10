#ifndef MULTIPLICITY_TRACKER_H
#define MULTIPLICITY_TRACKER_H

#include "AspenTool.h"

// multiplicity is how many times a kernel is going to be called
// note: in Explicit mode, it never moves beyond 1, because each
// iterate/map call s done explicitly. 
// in Implicit mode, it gets multiplied by iterate/map counts.
class MultiplicityTracker : public AspenTool
{
  public:
    bool debug;
    vector<int> stack;
  public:
    MultiplicityTracker(ASTAppModel *app) : AspenTool(app)
    {
        debug = false;
        stack.push_back(1);
    }
    int GetMultiplicity()
    {
        return stack.back();
    }
  protected:
    virtual TraversalMode TraversalModeHint(const ASTControlStatement *here)
    {
        //return Explicit;
        return Implicit;
    }
    virtual void StartKernel(TraversalMode mode, const ASTKernel *k)
    {
        if (debug) cerr << Indent(level) << "Starting kernel '"<<k->GetName()<<"' (mult="<<GetMultiplicity()<<")\n";
    }

    virtual void StartIterate(TraversalMode mode, const ASTControlIterateStatement *s)
    {
        if (debug) cerr << Indent(level) << "Starting iterate "<<s->GetLabel()<<"\n";
        if (mode == AspenTool::Explicit)
            return;

        int c = s->GetQuantity()->Expanded(app->paramMap)->Evaluate();
        int p = stack.back();
        stack.push_back(c*p);
    }
    virtual void EndIterate(TraversalMode mode, const ASTControlIterateStatement *s)
    {
        if (debug) cerr << Indent(level) << "Ending iterate "<<s->GetLabel()<<"\n";
        if (mode == AspenTool::Explicit)
            return;

        stack.pop_back();
    }
    
    virtual void StartMap(TraversalMode mode, const ASTControlMapStatement *s)
    {
        if (debug) cerr << Indent(level) << "Starting map "<<s->GetLabel()<<"\n";
        if (mode == AspenTool::Explicit)
            return;

        int c = s->GetQuantity()->Expanded(app->paramMap)->Evaluate();
        int p = stack.back();
        stack.push_back(c*p);
    }
    virtual void EndMap(TraversalMode mode, const ASTControlMapStatement *s)
    {
        if (debug) cerr << Indent(level) << "Ending map "<<s->GetLabel()<<"\n";
        if (mode == AspenTool::Explicit)
            return;

        stack.pop_back();
    }
};


#endif
