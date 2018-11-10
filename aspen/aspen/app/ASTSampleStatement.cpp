// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "app/ASTSampleStatement.h"

ASTSampleStatement::ASTSampleStatement(Identifier *n, FunctionCall *d)
    : ident(n), distribution(d)
{
    srand(time(NULL));
}

ASTSampleStatement::~ASTSampleStatement()
{
    delete distribution;
}

void
ASTSampleStatement::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "Sample "<<GetName()<< " = " << endl;
    distribution->Print(out, indent+1);
}

void
ASTSampleStatement::Export(ostream &out, int indent) const
{
    out << Indent(indent) << "sample "<<GetName()<< " from " << distribution->GetText(Expression::ASPEN) << endl;
}

Expression*
ASTSampleStatement::GetResourceRequirementExpression(const ASTAppModel *app,
                                                     string resource) const
{
    return new Real(0);
}

set<string>
ASTSampleStatement::GetInclusiveDataArrays(const ASTAppModel *app) const
{
    return set<string>();
}

void
ASTSampleStatement::Traverse(AspenTool *tool) const
{
    ///\todo: implement?  Or is no-op correct?
    // actually, having this be a no-op is causing complexity; 
    // we assume each 'control statement' (e.g. within a Seq)
    // will generate a value to add up for e.g. runtimes/flops, and
    // this isn't true right now unless we have tools support
    // begin/end sample statements.
    //cerr << "Traversing Sample " << name << endl;
    //THROW(LogicError, "UNIMPLEMNENTED");
}

void
ASTSampleStatement::Visit(ModelVisitor *visitor)
{
    if (visitor->Visit(this))
        return;

    distribution->Visit(visitor);
}

bool
ASTSampleStatement::ContainsRandomSample(ASTAppModel *app) const
{
    return true;
}
