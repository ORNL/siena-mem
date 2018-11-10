// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef ASPEN_TOOL_H
#define ASPEN_TOOL_H

/**
* @file   AspenTool.h
*
* @author Jeremy Meredith
*         jsmeredith@gmail.com
*
* @brief  AspenTool class definition 
*/

/**
 * @mainpage ASPEN
 *
 * @section aspen_intro Introduction
 * Aspen is a language and tool suite for analytical performance modeling. 
 * Aspen is designed to be faster than simulators and hardware prototypes,
 * more accuracte than ad-hoc analytical models, and simpler and more flexbile
 * than either.
 *
 * @section aspen_api Aspen Language Constructs
 *
 * @section aspen_tools Aspen Tools
 *
 */

#include "app/ASTControlStatement.h"
#include "app/ASTControlIfThenStatement.h"
#include "app/ASTControlIterateStatement.h"
#include "app/ASTControlKernelCallStatement.h"
#include "app/ASTControlMapStatement.h"
#include "app/ASTControlParallelStatement.h"
#include "app/ASTControlProbabilityStatement.h"
#include "app/ASTControlSequentialStatement.h"
#include "app/ASTSampleStatement.h"
#include "app/ASTExecutionBlock.h"
#include "model/ASTAppModel.h"
#include "model/ASTMachModel.h"
#include "model/ASTMapModel.h"

class AspenTool
{
  public:
    enum TraversalMode { Implicit, Explicit };
  private:
    vector<AspenTool*> tools;
  protected:
    void AddTool(AspenTool *t) { tools.push_back(t); }
    int level;

    ASTAppModel *app;
    ASTMachModel *mach;
    ASTMapModel  *mapping;
    
  public:
    ParameterStack *paramStack;

  public:
    AspenTool(ASTAppModel *_app, ASTMachModel *_mach = NULL, ASTMapModel *_mapping = NULL)
        : app(_app), mach(_mach), mapping(_mapping),paramStack(NULL)
    {
        level = 0;
    }
    virtual ~AspenTool()
    {/*
        if(paramStack){
            delete paramStack;
	    }*/
    }

    void InitializeTraversal(ParameterStack *ps = NULL)
    {
        if (ps)
        {
            // If we're given a parameter stack, use it
            paramStack = ps;
        }
        else
        {
            // ... otherwise, create and initialize it
            // 
            paramStack = new ParameterStack;
            
            paramStack->GlobalValues.push_back(& app->paramMap);
            if (mach)
                paramStack->GlobalValues.push_back(& mach->paramMap);
        }
//	paramStack->PrintKeys();

        // Initialize sub-tools
        for (unsigned int i=0; i<tools.size(); ++i)
            tools[i]->InitializeTraversal(paramStack);
    }
    void FinalizeTraversal()
    {
        //finalize sub-tools
        for (unsigned int i=0; i<tools.size(); ++i){
            tools[i]->FinalizeTraversal();
            //tools[i]->paramStack = nullptr;
        }
        if(paramStack) delete paramStack;
    }
    void CheckInitialization()
    {
        if (!paramStack)
            THROW(LogicError, "Need to initialize tool before traversal.");
    }

    virtual TraversalMode TraversalModeHint(const ASTControlStatement *here)
    {
        return Implicit;
    }
    TraversalMode GetTraversalMode(const ASTControlStatement *here)
    {
        // if any sub tool tells us to go explict, we do
        for (unsigned int i=0; i<tools.size(); ++i)
            if (tools[i]->TraversalModeHint(here) == Explicit)
                return Explicit;

        // any random sample forces explict
        if (here->ContainsRandomSample(app))
            return Explicit;

        // any loop counter (iterate or map) forces explicit mode
        const ASTControlIterateStatement *citer =
            dynamic_cast<const ASTControlIterateStatement*>(here);
        if (citer && citer->GetCounter())
            return Explicit;

        const ASTControlMapStatement *cmap =
            dynamic_cast<const ASTControlMapStatement*>(here);
        if (cmap && cmap->GetCounter())
            return Explicit;

        // otherwise, do what this tool says
        return this->TraversalModeHint(here);
    }

    ///\todo: should this tool do the Start before its subtools?  I think it should
    /// definitely End after its subtools, but I'm not sure about the Start before/after.
#define DO_ENTER(t, v) {                             \
        CheckInitialization();                       \
        level++;                                     \
        for (unsigned int i=0; i<tools.size(); ++i)  \
            tools[i]->Enter##t(mode, v);             \
        this->Start##t(mode, v);                     \
}

#define DO_LEAVE(t, v) {                             \
        for (unsigned int i=0; i<tools.size(); ++i)  \
            tools[i]->Leave##t(mode, v);             \
        this->End##t(mode, v);                       \
        level--;                                     \
}

    void EnterKernel(TraversalMode mode, const ASTKernel *k)
    {
        CheckInitialization();

        paramStack->LocalStack.push_back(k->paramMap);
        for (unsigned int i=0; i<k->GetStatements()->GetItems().size(); ++i)
        {
            const ASTSampleStatement *s = dynamic_cast<const ASTSampleStatement*>(k->GetStatements()->GetItems()[i]);
            if (s)
            {
                double val = s->GetDistribution()->Expanded(*paramStack)->Evaluate();
                //cerr << Indent(level) << "Assigned sample " << val << " to "<<s->GetName()<<endl;
                paramStack->LocalStack.back()[s->GetName()] = new Real(val);
            }
        }

        DO_ENTER(Kernel, k);
    }
    void LeaveKernel(TraversalMode mode, const ASTKernel *k)
    {
        cerr << "LeaveKernel: "<< k->GetName()<<endl;
        
        if(paramStack->LocalStack.size()>0)
            paramStack->LocalStack.pop_back();

        DO_LEAVE(Kernel, k);
    }


    ///\todo: I think we can get rid of kernel calls, right, because they're just
    /// always, simply, going to cause a kernel execution, which is the one we need.
    /// (I mean the callback to the concrete tool, not this method.)
    /// This makes this a bit weird, because really the Tool is doing something
    /// different for a kernel call than other control statements, and
    /// should maybe be named differently.
    void EnterKernelCall(TraversalMode mode, const ASTControlKernelCallStatement *s)
    {
        CheckInitialization();

        paramStack->LocalStack.push_back(NameMap<const Expression*>());

        const ASTKernel *k = s->GetKernel(app);
        if (!k)
            THROW(ModelError, "Can't Find Referenced Kernel");

        for (unsigned int i=0; i<s->GetArguments().size(); ++i)
        {
            ASTKernelArgDecl *argdecl = k->GetArgs()[i];
            if (argdecl->GetType() == "param")
            {
                paramStack->LocalStack.back()[argdecl->GetName()] = s->GetArguments()[i];
            }
        }

        ///\todo: DISABLE THIS, RIGHT?
        DO_ENTER(KernelCall, s);
    }
    void LeaveKernelCall(TraversalMode mode, const ASTControlKernelCallStatement *s)
    {
        paramStack->LocalStack.pop_back();

        ///\todo: DISABLE THIS, RIGHT?
        DO_LEAVE(KernelCall, s);
    }

    const ASTKernel *GetKernelForCall(const string &name)
    {
        return app->GetKernel(name);
    }

    void EnterIterate(TraversalMode mode, const ASTControlIterateStatement *s)
    {
        DO_ENTER(Iterate, s);
    }
    void LeaveIterate(TraversalMode mode, const ASTControlIterateStatement *s)
    {
        DO_LEAVE(Iterate, s);
    }

    
    void EnterMap(TraversalMode mode, const ASTControlMapStatement *s)
    {
        DO_ENTER(Map, s);
    }
    void LeaveMap(TraversalMode mode, const ASTControlMapStatement *s)
    {
        DO_LEAVE(Map, s);
    }

    
    void EnterSeq(TraversalMode mode, const ASTControlSequentialStatement *s)
    {
        DO_ENTER(Seq, s);
    }
    void LeaveSeq(TraversalMode mode, const ASTControlSequentialStatement *s)
    {
        DO_LEAVE(Seq, s);
    }

    
    void EnterPar(TraversalMode mode, const ASTControlParallelStatement *s)
    {
        DO_ENTER(Par, s);
    }
    void LeavePar(TraversalMode mode, const ASTControlParallelStatement *s)
    {
        DO_LEAVE(Par, s);
    }
    
    void EnterProbability(TraversalMode mode, const ASTControlProbabilityStatement *s)
    {
        DO_ENTER(Probability, s);
    }
    void LeaveProbability(TraversalMode mode, const ASTControlProbabilityStatement *s)
    {
        DO_LEAVE(Probability, s);
    }

    
    void PerformExecute(const ASTExecutionBlock *e)
    {
        level++;
        for (unsigned int i=0; i<tools.size(); ++i)
            tools[i]->PerformExecute(e);
        this->Execute(e);
        level--;
    }

#undef DO_ENTER
#undef DO_LEAVE

    //
  protected:
    virtual void StartKernel(TraversalMode mode, const ASTKernel *k)
    {
    }
    virtual void EndKernel(TraversalMode mode, const ASTKernel *k)
    {
    }


    
    virtual void StartKernelCall(TraversalMode mode, const ASTControlKernelCallStatement *s)
    {
    }
    virtual void EndKernelCall(TraversalMode mode, const ASTControlKernelCallStatement *s)
    {
    }
    


    virtual void StartIterate(TraversalMode mode, const ASTControlIterateStatement *s)
    {
    }
    virtual void EndIterate(TraversalMode mode, const ASTControlIterateStatement *s)
    {
    }

    
    virtual void StartMap(TraversalMode mode, const ASTControlMapStatement *s)
    {
    }
    virtual void EndMap(TraversalMode mode, const ASTControlMapStatement *s)
    {
    }

    
    virtual void StartSeq(TraversalMode mode, const ASTControlSequentialStatement *s)
    {
    }
    virtual void EndSeq(TraversalMode mode, const ASTControlSequentialStatement *s)
    {
    }

    
    virtual void StartPar(TraversalMode mode, const ASTControlParallelStatement *s)
    {
    }
    virtual void EndPar(TraversalMode mode, const ASTControlParallelStatement *s)
    {
    }

    virtual void StartProbability(TraversalMode mode, const ASTControlProbabilityStatement *s)
    {
    }
    virtual void EndProbability(TraversalMode mode, const ASTControlProbabilityStatement *s)
    {
    }

    virtual void Execute(const ASTExecutionBlock *e)
    {
    }
};

#endif
