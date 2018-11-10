#include "mach/ASTMachComponent.h"
#include "model/ASTMachModel.h"

ASTMachComponent::ASTMachComponent(string type,
                                   string name,
                                   ParseVector<ASTNode*> contents)
    : type(type), name(name), mach(NULL)
{
    for (unsigned int i=0; i<contents.size(); ++i)
    {
        ASTNode *n = contents[i];
        ASTSubComponent *scomp = dynamic_cast<ASTSubComponent*>(n);
        ASTMachProperty *prop  = dynamic_cast<ASTMachProperty*>(n);
	ASTMachConnectivity *conn  = dynamic_cast<ASTMachConnectivity*>(n);
        ASTMachResource *res   = dynamic_cast<ASTMachResource*>(n);
        ASTMachPower    *pwr   = dynamic_cast<ASTMachPower*>(n);
        ASTResourceConflict *conf = dynamic_cast<ASTResourceConflict*>(n);
        if (scomp)
            subcomponents.push_back(scomp);
        if (prop)
            properties.push_back(prop);
	if (conn)
	    connections.push_back(conn);
        if (res)
            resources.push_back(res);
        if (pwr)
            power.push_back(pwr);
        if (conf)
            conflicts.push_back(conf);
    }
}

ASTMachComponent::~ASTMachComponent()
{
    for (unsigned int i=0; i<subcomponents.size(); ++i)
        delete subcomponents[i];
    for (unsigned int i=0; i<properties.size(); ++i)
        delete properties[i];
    for (unsigned int i=0; i<resources.size(); ++i)
        delete resources[i];
    for (unsigned int i=0; i<power.size(); ++i)
        delete power[i];
    for (unsigned int i=0; i<conflicts.size(); ++i)
        delete conflicts[i];
}

void
ASTMachComponent::Print(ostream &out, int indent) const
{
    out << Indent(indent) << type << ": '" <<name << "'" << endl;
    for (unsigned int i=0; i<subcomponents.size(); ++i)
        subcomponents[i]->Print(out, indent+1);
    for (unsigned int i=0; i<properties.size(); ++i)
        properties[i]->Print(out, indent+1);
    for (unsigned int i=0; i<resources.size(); ++i)
        resources[i]->Print(out, indent+1);
    for (unsigned int i=0; i<power.size(); ++i)
        power[i]->Print(out, indent+1);
    for (unsigned int i=0; i<conflicts.size(); ++i)
        conflicts[i]->Print(out, indent+1);
}

void
ASTMachComponent::CompleteAndCheck(ASTMachModel *machmodel)
{
    mach = machmodel;
    for (auto &sc: subcomponents)
    {
        string sn = sc->GetName();
        string st = sc->GetType();
        if (! mach->compMap.Contains(sn))
        {
            THROW(ModelError, "component ",sn," in ",name,
                  " wasn't defined in the model");
        }
        const ASTMachComponent *mc = mach->compMap[sn];
        if (mc->GetType() != st)
        {
            THROW(ModelError, "component ",sn," in ",name,
                  " wasn't the expected type");
        }
        subcomponentmap[sn] = mc;
        subcomponentmap2[sn] = sc;
    }

    for (auto &r: resources)
    {
        resourceMap[r->GetName()] = r;
    }
}

Expression*
ASTMachComponent::GetSelfTimeExpression(const string &resource,
                                        const vector<ASTTrait*> &traits, 
                                        const Expression *value) const
{
    if (! resourceMap.Contains(resource))
        return NULL;

    const ASTMachResource *r = resourceMap[resource];

    NameMap<const Expression*> valmap(r->GetArg(), value);
    Expression *expr = r->GetQuantity()->Expanded(valmap);
                    
    // each trait applies another expression to our
    // current time calculation
    for (unsigned int j=0; j<traits.size(); ++j)
    {
        string tn = traits[j]->GetName();
        const ASTTraitDefinition *t = r->GetTraitDefinition(tn);
        if (!t)
            continue;
                        
        // we assume traits use "base" as the starting resource value
        NameMap<const Expression*> argmap;
        argmap["base"] = expr;
        // and substitute the argument name for its matching value
        if (t->GetArg() != "")
        {
            if (traits[j]->GetValue() == NULL)
            {
                THROW(ModelError, "Trait '",tn,"' requires an argument");
            }
            argmap[t->GetArg()] = traits[j]->GetValue();
        }
        Expression *newexpr = t->GetQuantity()->Expanded(argmap);
        delete expr; // we just made a copy of this
        expr = newexpr;
    }
                    
    return expr;
}

Expression*
ASTMachComponent::GetSerialTimeExpression(const string &resource,
                                          const vector<ASTTrait*> &traits, 
                                          const Expression *value,
                                          const MappingRestriction &restriction) const
{
    // Stop descent here if restriction doesn't match
    if (restriction.type == type && restriction.name != name)
        return NULL;

    // Check yourself for this resource
    Expression *expr = GetSelfTimeExpression(resource, traits, value);
    if (expr)
        return expr;

    // We didn't have this resource.  Check children;
    for (unsigned int i=0; i<subcomponents.size(); ++i)
    {
        const ASTSubComponent *sc = subcomponents[i];
        const ASTMachComponent *comp = subcomponentmap[sc->GetName()];
        Expression *expr = comp->GetSerialTimeExpression(resource,traits,value,restriction);
        if (expr)
        {
            return expr;
        }
    }
    return NULL;
}

Expression*
ASTMachComponent::GetIdealizedTimeExpression(const string &resource,
                                             const vector<ASTTrait*> &traits, 
                                             const Expression *value,
                                             const MappingRestriction &restriction) const
{
    // Stop descent here if restriction doesn't match
    if (restriction.type == type && restriction.name != name)
        return NULL;

    // Check yourself for this resource
    Expression *expr = GetSelfTimeExpression(resource, traits, value);
    if (expr)
        return expr;

    // We didn't have this resource.  Check children;
    for (unsigned int i=0; i<subcomponents.size(); ++i)
    {
        const ASTSubComponent *sc = subcomponents[i];
        const ASTMachComponent *comp = subcomponentmap[sc->GetName()];
        ///\todo: here's an opportunity for parallelism by splitting work among e.g. multiple socket types.
        /// specifically, we could loop over all subcomponents and divide the work.  of course, we need
        /// to take into account actual parallelism available to the kernel.
        Expression *expr = comp->GetIdealizedTimeExpression(resource,traits,value,restriction);
        if (expr)
        {
            if (sc->GetQuantity() != NULL)
                return new BinaryExpr("/", expr, sc->GetQuantity()->Cloned());
            else
                return expr;
        }
    }

    return NULL;
}

bool
ASTMachComponent::CheckConflict(string ra, string rb,
                                const MappingRestriction &restriction) const
{
    // Stop descent here if restriction doesn't match
    if (restriction.type == type && restriction.name != name)
        return false;

    // check myself
    for (unsigned int i=0; i<conflicts.size(); ++i)
    {
        if (conflicts[i]->Conflicts(ra,rb))
        {
            return true;
        }
    }

    // check subcomponents
    for (unsigned int i=0; i<subcomponents.size(); ++i)
    {
        const ASTSubComponent *sc = subcomponents[i];
        const ASTMachComponent *comp = subcomponentmap[sc->GetName()];
        if (comp->CheckConflict(ra,rb,restriction))
            return true;
    }

    // no conflicts found
    return false;
}

Expression*
ASTMachComponent::GetTotalQuantityExpression(const string &resource,
                                        const MappingRestriction &restriction) const
{
    // Stop descent here if restriction doesn't match
    if (restriction.type == type && restriction.name != name)
        return NULL;

    // Check yourself for this resource
    if (resourceMap.Contains(resource))
    {
        // yep!
        return new Real(1);
    }

    // We're not a core, so start diving into subcomponents looking for one
    for (unsigned int i=0; i<subcomponents.size(); ++i)
    {
        const ASTSubComponent *sc = subcomponents[i];
        const ASTMachComponent *comp = subcomponentmap[sc->GetName()];
        ///\todo: here's an opportunity for parallelism by splitting work among e.g. multiple socket types.
        /// specifically, we could loop over all subcomponents and divide the work.  of course, we need
        /// to take into account actual parallelism available to the kernel.
        Expression *expr = comp->GetTotalQuantityExpression(resource,restriction);
        if (expr)
        {
            if (sc->GetQuantity() != NULL)
                return new BinaryExpr("*", expr, sc->GetQuantity()->Cloned());
            else
                return expr;
        }
    }

    // No cores with the right socket type in this chain
    return NULL;
}
