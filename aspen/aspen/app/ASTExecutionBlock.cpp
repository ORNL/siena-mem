#include "app/ASTExecutionBlock.h"
#include "model/ASTMachModel.h"
#include "app/ASTRequiresStatement.h"
#include "walkers/AspenTool.h"

ASTExecutionBlock::ASTExecutionBlock()
    : ASTControlStatement(""),
      isparallel(false),
      parallelism(NULL)
{
    if (!parallelism)
        parallelism = new Real(1);
}

ASTExecutionBlock::ASTExecutionBlock(string label,
                                     Expression *par,
                                     ParseVector<ASTExecutionStatement*> statements)
    : ASTControlStatement(label),
      isparallel(par != NULL),
      parallelism(par),
      statements(statements.begin(), statements.end())
{
    if (!parallelism)
        parallelism = new Real(1);
}

ASTExecutionBlock::~ASTExecutionBlock()
{
    delete parallelism;
    for (auto it : statements)
        delete it;
}

void
ASTExecutionBlock::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "Execution Block";
    if (label != "")
        out << " (name='"<<label<<"')";
    out <<" (control node '"<<nodeid<<"')"<<endl;
    if (isparallel)
    {
        out << Indent(indent+1) << "Parallelism:"<<endl;
        parallelism->Print(out, indent+2);
    }
    for (unsigned int i=0; i<statements.size(); i++)
    {
        statements[i]->Print(out, indent+1);
    }
}

void
ASTExecutionBlock::Export(ostream &out, int indent) const
{
    out << Indent(indent) << "execute ";
    if (isparallel)
        out << "["<<parallelism->GetText(Expression::ASPEN)<<"] ";
    if (label != "")
        out << "\"" << label << "\" ";
    out << "{" << endl;
    for (unsigned int i=0; i<statements.size(); i++)
    {
        statements[i]->Export(out, indent+1);
    }
    out << Indent(indent) << "}" <<endl;
}

Expression*
ASTExecutionBlock::GetResourceRequirementExpression(const ASTAppModel *app,
                                                    string resource) const
{
    Expression *expr = NULL;

    for (unsigned int i=0; i<statements.size(); ++i)
    {
        const ASTExecutionStatement *s = statements[i];
        const ASTRequiresStatement *req = dynamic_cast<const ASTRequiresStatement*>(s);
        if (req) // requires statement
        {
#if 0
            // see if the resource matches; note that 'bytes' means loads or stores
            if (req->GetResource() == resource || 
                (resource=="bytes" && (req->GetResource()=="loads"||req->GetResource()=="stores")))
            {
                if (!expr)
                    expr = req->GetQuantity()->Cloned();
                else
                    expr = new BinaryExpr("+",expr,req->GetQuantity()->Cloned());
            }
#else
            // see if the resource matches; note that 'bytes' means loads or stores
            //[Modified by Seyong Lee] This version supports resource with traits, such as loads with random (loads_random)
            if (req->GetResource() == resource ||
                (resource=="bytes" && (req->GetResource()=="loads"||req->GetResource()=="stores")) ||
                (req->ContainResource(resource)) )
            {
                if (!expr)
                    expr = req->GetQuantity()->Cloned();
                else
                    expr = new BinaryExpr("+",expr,req->GetQuantity()->Cloned());
            }
#endif
        }
    }

    if (!expr)
        return new Real(0);

    return new BinaryExpr("*", GetParallelism()->Cloned(), expr);
}

set<string>
ASTExecutionBlock::GetInclusiveDataArrays(const ASTAppModel *app) const
{
    return GetDataArrays();
}

set<string>
ASTExecutionBlock::GetDataArrays() const
{
    set<string> arrays;
    for (unsigned int i=0; i<statements.size(); ++i)
    {
        const ASTExecutionStatement *s = statements[i];
        const ASTRequiresStatement *req = dynamic_cast<const ASTRequiresStatement*>(s);
        if (!req)
            continue;
        if (req->GetToFrom() != "")
            arrays.insert(req->GetToFrom());
    }
    return arrays;
}

struct resgrouper
{
    string resource;
    string tofrom;
    vector<string> traits;
    bool operator<(const resgrouper &b) const
    {
        if (resource < b.resource)
            return true;
        if (resource > b.resource)
            return false;
        if (tofrom < b.tofrom)
            return true;
        if (tofrom > b.tofrom)
            return false;
        if (traits.size() < b.traits.size())
            return true;
        if (traits.size() > b.traits.size())
            return false;
        int n = traits.size();
        for (int i=0; i<n; ++i)
        {
            if (traits[i] < b.traits[i])
                return true;
            if (traits[i] > b.traits[i])
                return false;
        }
        return false;
    }
};

Expression*
ASTExecutionBlock::GetTimeExpression(ASTAppModel *app,
                                     ASTMachModel *mach,
                                     string sockettype) const
{
    if (statements.empty())
        return new Real(0);

    MappingRestriction restr("socket", sockettype);

    /// ---------
    /// Experimental (new): Group resource usage (and limit it appropriately) (plus some "smart" things like assuming stride-0 expressions are constant, etc.)
    /// ---------
#if 0


    map<resgrouper, Expression*> resource_usage;
    for (unsigned int i=0; i<statements.size(); ++i)
    {
        const ASTExecutionStatement *s = statements[i];
        const ASTRequiresStatement *req = dynamic_cast<const ASTRequiresStatement*>(s);
        if (!req)
            continue;
        resgrouper rg;
        rg.resource = req->GetResource();
        rg.tofrom   = req->GetToFrom();

        //// hack, skip intracomm!
        //if (rg.resource == "intracomm")
        //{
        //    resource_usage[rg] = new Real(0);
        //    continue;
        //}

        bool skip = false;
        for (unsigned t = 0; t < req->GetNumTraits(); ++t)
        {
            if (req->GetTrait(t)->GetName() == "stride")
            {
                try
                {
                    double stride = req->GetTrait(t)->GetValue()->Evaluate();
                    if (stride == 0)
                        skip = true; // skip this resource requirement entirely; a stride of 0 means assume register usage
                    else if (stride <= 1)
                        continue; // skip this trait; we ignore the value for traits, but removing the stride trait means it's assumed to be continuous
                }
                catch (...)
                {
                    // ignore errors from evaluating the trait quantity
                }
            }
            rg.traits.push_back(req->GetTrait(t)->GetName());
        }
        if (skip)
            continue;
        if (resource_usage.count(rg))
            resource_usage[rg] = new BinaryExpr("+", resource_usage[rg], req->GetQuantity()->Cloned());
        else
            resource_usage[rg] = req->GetQuantity()->Cloned();
    }

#if 1
    // clamp every let of loads/stores to max total size of a given array
    ///\todo: even this modification should probably be limited to stride-0 accesses
    for (map<resgrouper, Expression*>::iterator it = resource_usage.begin();
         it != resource_usage.end(); ++it)
    {
        const resgrouper &rg = it->first;
        if (rg.tofrom != "")
        {
            Expression *size = app->GetSingleArraySize(rg.tofrom);
            if (size)
            {
                // simple option ignored parallelism
                //it->second = new FunctionCall("min", it->second, size);

                // complex option; only read a total of 'size' bytes across ALL parallel instances
                const Expression *par = GetParallelism();
                it->second = new FunctionCall("min", it->second, new BinaryExpr("/",size,par->Cloned()));
            }
        }
    }
#endif

    // DEBUG
#if 1
    for (map<resgrouper, Expression*>::iterator it = resource_usage.begin();
         it != resource_usage.end(); ++it)
    {
        const resgrouper &rg = it->first;
        cerr << "rg=" << rg.resource << " tofrom=" << rg.tofrom << " traits=";
        for (unsigned int i=0; i<rg.traits.size(); ++i)
            cerr << rg.traits[i] << ",";
        cerr << " ::: " << it->second->GetText() << endl;
    }
    cerr << endl;
#endif

    // calculate times for each requirement and add them up
    map<string,Expression*> requirements;
    for (map<resgrouper, Expression*>::iterator it = resource_usage.begin();
         it != resource_usage.end(); ++it)
    {
        const resgrouper &rg = it->first;
        const Expression *quantity = it->second;

        // get the "serial" time expression for this resource
        Expression *sertime = mach->GetMachine()->
            GetSerialTimeExpression(rg.resource, rg.traits, quantity, restr);
        Expression *quant = mach->GetMachine()->
            GetTotalQuantityExpression(rg.resource, restr);
        Expression *depth = new FunctionCall("ceil",
                                 new BinaryExpr("/", parallelism, quant));
        Expression *time = new BinaryExpr("*", depth, sertime);
        //cerr << "   ---- time for resource '"<<resource<<"': "<<time->GetText() << endl;
        if (requirements[rg.resource])
            requirements[rg.resource] = new BinaryExpr("+",requirements[rg.resource],time);
        else
            requirements[rg.resource] = time;
    }


#else

    /// ---------
    /// Standard (old): Calculate runtime for each resource statement independently
    /// ---------


    // Get the raw requirements for each type of resource
    map<string,Expression*> requirements;
    for (unsigned int i=0; i<statements.size(); ++i)
    {
        const ASTExecutionStatement *s = statements[i];
        const ASTRequiresStatement *req = dynamic_cast<const ASTRequiresStatement*>(s);
        if (!req)
            continue;

        // get the resource and traits
        string resource = req->GetResource();
        const vector<ASTTrait*> &traits = req->GetTraits();

        // get the "serial" time expression for this resource
        Expression *sertime = mach->GetMachine()->GetSerialTimeExpression(resource, traits, req->GetQuantity(), restr);
        Expression *quant = mach->GetMachine()->GetTotalQuantityExpression(resource, restr);
        Expression *depth = new FunctionCall("ceil",new BinaryExpr("/", parallelism->Cloned(), quant));
        Expression *time  = new BinaryExpr("*", depth, sertime);
            
        if (!time)
            cerr << "ERROR getting time for '"<<resource<<"' with quantity "<<req->GetQuantity()->GetText()<<endl;
        else
        {
            //cerr << "   ---- time for resource '"<<resource<<"': "<<time->GetText() << endl;
            if (requirements[resource])
                requirements[resource] = new BinaryExpr("+",requirements[resource],time);
            else
                requirements[resource] = time;
        }
    }
#endif

    // Fold any that conflict using "+"
    map<set<string>, Expression*> conflicting;
    bool foundnewconflict = true;
    while (foundnewconflict)
    {
        foundnewconflict = false;

        // first, look for conflicts with an already-folded resource
        //cerr << "looking for conflicts with an already-folded resource\n";
        for (map<string,Expression*>::iterator req = requirements.begin(); req != requirements.end(); req++)
        {
            string reqres = req->first;
            for (map<set<string>,Expression*>::iterator conf = conflicting.begin(); conf != conflicting.end(); conf++)
            {
                for (set<string>::iterator confitem = conf->first.begin(); confitem != conf->first.end(); confitem++)
                {
                    if (mach->GetMachine()->CheckConflict(reqres, *confitem, restr))
                    {
                        //cerr << "   -- Found conflict between so-far-not-conflicting-item "<<reqres<<" and already-conflicting-item "<<*confitem<<endl;
                        foundnewconflict = true;
                        break;
                    }
                }
                if (foundnewconflict)
                {
                    set<string> newset(conf->first);
                    newset.insert(reqres);
                    conflicting[newset] = new BinaryExpr("+", conf->second, req->second);
                    conflicting.erase(conf);
                    requirements.erase(req);
                }
                if (foundnewconflict)
                    break;
            }
            if (foundnewconflict)
                break;
        }

        // next, look for conflicts between iterms that have not yet been folded
        //cerr << "no conflicts with existing conflicting items; looking at unfolded pairs\n";
        for (map<string,Expression*>::iterator req1 = requirements.begin(); req1 != requirements.end(); req1++)
        {
            string reqres1 = req1->first;
            //cerr << " (1) reqres1 = " << reqres1 << endl;
            for (map<string,Expression*>::iterator req2 = requirements.begin(); req2 != requirements.end(); req2++)
            {
                string reqres2 = req2->first;
                //cerr << "    (2) reqres2 = " << reqres2 << endl;
                if (reqres1 != reqres2)
                {
                    if (mach->GetMachine()->CheckConflict(reqres1, reqres2, restr))
                    {
                        //cerr << "   -- Found conflict between so-far-not-conflicting-items "<<reqres1<<" and "<<reqres2<<endl;
                        set<string> newset;
                        newset.insert(reqres1);
                        newset.insert(reqres2);
                        conflicting[newset] = new BinaryExpr("+", req1->second, req2->second);
                        foundnewconflict = true;
                        requirements.erase(req1);
                        requirements.erase(req2);
                    }
                }
                if (foundnewconflict)
                    break;
            }
            if (foundnewconflict)
                break;
        }

        //cerr << "foundnewconflict="<<foundnewconflict<<endl;
    }

    // We should have found all conflicting requirements
    // Whatever's left (from the individual or previously-folded
    // requirements) can be folded using Max()
    Expression *runtime = NULL;
    for (map<string,Expression*>::iterator req = requirements.begin(); req != requirements.end(); req++)
    {
        //cerr << "for "<<req->first<<", serial runtime expr = "<<req->second->GetText()<<endl;
        if (!runtime)
            runtime = req->second;
        else
            runtime = new FunctionCall("max", runtime, req->second);
    }
    for (map<set<string>,Expression*>::iterator conf = conflicting.begin(); conf != conflicting.end(); conf++)
    {
        //cerr << "for (conflicting set) serial runtime expr = "<<conf->second->GetText()<<endl;
        if (!runtime)
            runtime = conf->second;
        else
            runtime = new FunctionCall("max", runtime, conf->second);
    }
    if (!runtime)
        THROW(ModelError, "Couldn't get serial time");
    return runtime;
}

void
ASTExecutionBlock::Traverse(AspenTool *tool) const
{
    tool->PerformExecute(this);
}


void
ASTExecutionBlock::Visit(ModelVisitor *visitor)
{
    if (visitor->Visit(this))
        return;

    parallelism->Visit(visitor);
    for (unsigned int i=0; i<statements.size(); ++i)
        statements[i]->Visit(visitor);
}
