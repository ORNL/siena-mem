// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "model/ASTMachModel.h"

#include "model/ASTAssignStatement.h"

ASTMachModel::ASTMachModel(ParseVector<ASTStatement*> globals,
                           ParseVector<ASTMachComponent*> components)
    : globals(globals.begin(),globals.end()), components(components.begin(),components.end())
{
}

ASTMachModel::~ASTMachModel()
{
    for (unsigned int i=0; i<globals.size(); i++)
        delete globals[i];
    for (unsigned int i=0; i<components.size(); i++)
        delete components[i];
}

void
ASTMachModel::CompleteAndCheck()
{
    // ensure we don't do this call more than once
    if (!paramMap.Empty())
        return;

    // build the maps
    for (unsigned int i=0; i<globals.size(); ++i)
    {
        ASTAssignStatement *assign = dynamic_cast<ASTAssignStatement*>(globals[i]);
        if (assign)
            paramMap[assign->GetName()] = assign->GetValue();
    }
    for (unsigned int i=0; i<components.size(); ++i)
    {
        compMap[components[i]->GetName()] = components[i];
    }

    // check all the components
    for (unsigned int i=0; i<components.size(); ++i)
    {
        components[i]->CompleteAndCheck(this);
        if (components[i]->GetType() == "machine")
            machine = components[i];
        if (components[i]->GetType() == "socket")
            socketlist.push_back(components[i]->GetName());
    }
}

void
ASTMachModel::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "Machine model:" << endl;
    out << Indent(indent+1) << "Globals:" << endl;
    for (unsigned int i=0; i<globals.size(); i++)
    {
        globals[i]->Print(out, indent+2);
    }
    out << Indent(indent+1) << "Components:" << endl;
    for (unsigned int i=0; i<components.size(); i++)
    {
        components[i]->Print(out, indent+2);
    }
}

const ASTMachComponent*
ASTMachModel::GetMachine() const
{
    ///\todo: should this be a specific type?
    /// or just search for the top-most component?
    return machine;
}
