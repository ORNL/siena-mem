// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "app/ASTCaseItem.h"

ASTCaseItem::ASTCaseItem(Expression *quantity, ASTControlStatement *item)
    : quantity(quantity), item(item)
{
}

ASTCaseItem::~ASTCaseItem()
{
}

void
ASTCaseItem::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "For quantity:" << endl;
    quantity->Print(out, indent+1);
    out << Indent(indent) << "do:" << endl;
    item->Print(out, indent+1);
}

void
ASTCaseItem::Export(ostream &out, int indent) const
{
    out << Indent(indent) << "[" << quantity->GetText(Expression::ASPEN) << "] {";
    item->Export(out, indent+1);
    out << Indent(indent) << "}";
}

void
ASTCaseItem::Visit(ModelVisitor *visitor)
{
    if (visitor->Visit(this))
        return;

    quantity->Visit(visitor);
    item->Visit(visitor);
}
