// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "model/ASTAppModel.h"

#include "model/ASTAssignStatement.h"
#include "model/ASTAssignRangeStatement.h"
#include "model/ASTEnumDeclaration.h"
#include "model/ASTDataStatement.h"
#include "expr/ExpressionBuilder.h"

ASTAppModel::ASTAppModel(const string &n, ParseVector<ASTStatement*> globals, ParseVector<ASTKernel*> kernels)
    : name(n), globals(globals.begin(),globals.end()), kernels(kernels.begin(),kernels.end())
{
    mainKernel = NULL;
}

ASTAppModel::~ASTAppModel()
{
    for (unsigned int i=0; i<globals.size(); i++)
        delete globals[i];
    for (unsigned int i=0; i<kernels.size(); i++)
        delete kernels[i];
}

void
ASTAppModel::CompleteAndCheck()
{
    // ensure we don't do this call more than once
    if (!paramMap.Empty())
        return;

    for (unsigned int i=0; i<globals.size(); ++i)
    {
        ASTAssignStatement *assign = dynamic_cast<ASTAssignStatement*>(globals[i]);
        if (assign)
        {
            paramMap[assign->GetName()] = assign->GetValue();
        }
        ASTEnumDeclaration *enumdecl = dynamic_cast<ASTEnumDeclaration*>(globals[i]);
        if (enumdecl)
        {
            for (unsigned int i=0; i<enumdecl->GetNumValues(); ++i)
            {
                paramMap[enumdecl->GetValue(i)] = new Integer(i);
            }
        }
    }

    for (unsigned int i=0; i<kernels.size(); ++i)
    {
        kernelMap[kernels[i]->GetName()] = kernels[i];
    }

    // Find the main kernel
    mainKernel = kernelMap["main"];
}

void
ASTAppModel::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "App model "<<name<< ":" << endl;
    out << Indent(indent+1) << "Globals:" << endl;
    for (unsigned int i=0; i<globals.size(); i++)
    {
        globals[i]->Print(out, indent+2);
    }
    out << Indent(indent+1) << "Kernels:" << endl;
    for (unsigned int i=0; i<kernels.size(); i++)
    {
        kernels[i]->Print(out, indent+2);
    }
}

void
ASTAppModel::Export(ostream &out, int indent) const
{
    out << "model " <<name<<" {" << endl;
    out << endl;
    for (unsigned int i=0; i<globals.size(); i++)
    {
        globals[i]->Export(out, indent+1);
    }
    out << endl;
    for (unsigned int i=0; i<kernels.size(); i++)
    {
        kernels[i]->Export(out, indent+1);
    }
    out << endl;
    out << "}" << endl;
}

int
ASTAppModel::FindParametersWithRanges(vector<const Identifier*> &identifiers,
                                      vector<double> &defvals,
                                      vector<double> &minvals,
                                      vector<double> &maxvals) const
{
    ///\todo: we currently allow ranges to have expressions for their default and limit values.
    /// While convenient, it adds some pain (like having to create a paramMap to evaluate
    /// these values here) which may not be worth the effort.  We might want to restrict these to
    /// explicit values only, not expressions.
    for (unsigned int i=0; i<globals.size(); ++i)
    {
        const ASTAssignRangeStatement *range = dynamic_cast<const ASTAssignRangeStatement*>(globals[i]);
        if (!range)
            continue;
        identifiers.push_back(range->GetIdentifier());
        defvals.push_back(range->GetValue()->EvaluateWithExpansion(paramMap));
        minvals.push_back(range->GetMinVal()->EvaluateWithExpansion(paramMap));
        maxvals.push_back(range->GetMaxVal()->EvaluateWithExpansion(paramMap));
    }        
    return identifiers.size();
}

void
ASTAppModel::RedefineParam(string name, Expression* val)
{
    int a = 0;
    for (auto &i : globals)
    {
      ASTAssignStatement* stmt = dynamic_cast<ASTAssignStatement*>(i);
      if(!stmt)
      {
        a++;
        continue;
      }
      ///\todo: do we need to erase the old one, or just reset its value?
      if (stmt->GetName() == name)
        globals.erase(globals.begin() + a);
      a++;
    }
    globals.push_back(new ASTAssignStatement(new Identifier(name),val));
}

Expression*
ASTAppModel::GetResourceRequirementExpression(string resource) const
{
    if (!mainKernel)
        THROW(ModelError, "No main() kernel defined");

    return mainKernel->GetResourceRequirementExpression(this, resource);
}

Expression*
ASTAppModel::GetControlFlowExpression() const
{
    if (!mainKernel)
        THROW(ModelError, "No main() kernel defined");

    ///\todo: A null AppModel gives it nothing to substitute.  This
    /// is a little ugly, though.
    return mainKernel->GetResourceRequirementExpression(NULL, "");
}

double
ASTAppModel::Count(string resource, string withparam, double withparamvalue) const
{
    ExprPtr expr(GetResourceRequirementExpression(resource));

    NameMap<const Expression*> fullsubst(paramMap);

    if (withparam != "")
        fullsubst[withparam] = new Real(withparamvalue);

    double count = expr->EvaluateWithExpansion(fullsubst);

    if (withparam != "")
    {
        delete fullsubst[withparam];
        fullsubst[withparam] = NULL;
    }

    return count;
}

string
ASTAppModel::GetResourceRequirementExpressionText(string resource,
                                                  bool expand,
                                                  bool simplify,
                           const NameMap<const Expression*> &subst) const
{
    ExprPtr expr(GetResourceRequirementExpression(resource));


    // note: the old way of doing substitution was to expand the 
    // substitutions first, then expand everything else.
    // but if p=10 and q=p, and we substitute x for p,
    // anything that references q will get q==>p==>10,
    // whereas we really want q==>p==>x.  So the substitution
    // isn't a single-time thing; we need to redefine p
    // in the global parameter map.

    if (expand)
    {
        NameMap<const Expression*> fullsubst(paramMap);
        fullsubst.Insert(subst);
        expr = ExprPtr(expr->Expanded(fullsubst));
    }
    else if (!subst.Empty())
    {
        expr = ExprPtr(expr->Expanded(subst));
    }

    // and simplify if needed
    if (simplify)
    {
        int passes = 10;
        for (int i=0; i<passes; ++i)
            expr = ExprPtr(expr->Simplified());
    }
    return expr->GetText();
}

Expression*
ASTAppModel::GetSingleArraySize(string array) const
{
    for (unsigned int i=0; i<globals.size(); ++i)
    {
        const ASTStatement *s = globals[i];
        const ASTDataStatement *ds = dynamic_cast<const ASTDataStatement*>(s);
        //if (!ds)
	//  continue;
	if( !ds || ds->GetName().compare(array)!=0)
	  continue;


        return ds->GetSizeInBytes();
    }

    return NULL;
}


Expression*
ASTAppModel::GetGlobalArraySizeExpression() const
{
    ExpressionBuilder eb;
    for (unsigned int i=0; i<globals.size(); ++i)
    {
        const ASTStatement *s = globals[i];
        const ASTDataStatement *ds = dynamic_cast<const ASTDataStatement*>(s);
        if (!ds)
            continue;

        eb += ds->GetSizeInBytes();
    }

    if (eb.Empty())
        return new Real(0);
    else
        return eb.GetExpression();
}


void
ASTAppModel::Visit(ModelVisitor *visitor)
{
    if (visitor->Visit(this))
        return;
    for (unsigned int i=0; i<globals.size(); ++i)
        globals[i]->Visit(visitor);
    for (unsigned int i=0; i<kernels.size(); ++i)
        kernels[i]->Visit(visitor);
}
