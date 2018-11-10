// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "expr/TableDereference.h"

#include "expr/Table.h"
#include "expr/Identifier.h"
#include "common/ParameterStack.h"
#include "expr/Real.h"

TableDereference::TableDereference(Expression *table,
                                   Expression *index1,
                                   Expression *index2)
    : table(table), index1(index1), index2(index2)
{
    if (!table || !index1  || !index2)
    {
        THROW(LogicError, "Null value in table dereference constructor!");
    }
}

TableDereference::~TableDereference()
{
    delete table;
    delete index1;
    delete index2;
}

void
TableDereference::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "TableDeref:"<<endl;
    out << Indent(indent+1) << "Table:" << endl;
    table->Print(out, indent+2);
    out << Indent(indent+1) << "Index1:" << endl;
    index1->Print(out, indent+2);
    out << Indent(indent+1) << "Index2:" << endl;
    index2->Print(out, indent+2);
}

Expression*
TableDereference::Cloned() const
{
    return new TableDereference(table->Cloned(),
                                index1->Cloned(),
                                index2->Cloned());
}

bool
TableDereference::Equivalent(const Expression *e) const
{
    const TableDereference *tde =
        dynamic_cast<const TableDereference*>(e);
    if (!tde)
        return false;
    if (! table->Equivalent(tde->table))
        return false;
    return index1->Equivalent(tde->index1) &&
           index2->Equivalent(tde->index1);
}

void
TableDereference::ExpandInPlace(const ParameterStack &paramMap, set<string> expandedset)
{
    THROW(LogicError, "ExpandInPlace should never reach TableDereference");
}

Expression*
TableDereference::Expanded(const ParameterStack &paramMap, set<string> expandedset) const
{
    // start by expanding the indices
    Expression *expanded_index1 = index1->Expanded(paramMap, expandedset);
    Expression *expanded_index2 = index2->Expanded(paramMap, expandedset);

    // see if we have an table expression for our table
    const Table *tableexpr = 
        dynamic_cast<const Table*>(table);

    // if not, see if it's an identifier that expands into an table
    if (!tableexpr)
    {
        // if we don't have an identifier as our table, or if it's not
        // in our param map, then there's not much else we can do,
        // so just expand the index side and defer
        const Identifier *ident = dynamic_cast<const Identifier*>(table);
        if (!ident || !paramMap.Find(ident->GetName()))
            return new TableDereference(table->Expanded(paramMap, expandedset),
                                        expanded_index1,expanded_index2);

        // That identifier had better now expand into an table.
        // In theory, we could support ways of generating an table,
        // but right now the only way to create one is via a
        // named parameter, so throw an error if it's not an table.
        tableexpr = dynamic_cast<const Table*>(paramMap.Find(ident->GetName()));
        if (!table)
            THROW(GrammaticalError, "Expected ", ident->GetName(), " to be an table");
    }

    // okay, we now have an table expression.
    // see if we can generate real integer indices.
    try
    {
        int int_index1 = expanded_index1->Evaluate();
        int int_index2 = expanded_index2->Evaluate();
        // success!
        delete expanded_index1;
        delete expanded_index2;

        if (int_index1<0 || int_index1>=(int)tableexpr->values.size())
            THROW(GrammaticalError, "Index1 out of range dereferencing table");
        if (int_index2<0 || int_index2>=(int)tableexpr->values[int_index1].size())
            THROW(GrammaticalError, "Index2 out of range dereferencing table");
        return new Real(tableexpr->values[int_index1][int_index2]);
    }
    catch (...)
    {
        // nope; just defer expansion until later
        return new TableDereference(tableexpr->Cloned(),
                                    expanded_index1, expanded_index2);
    }
}

double
TableDereference::Evaluate() const
{
    THROW(GrammaticalError, "Can't evaluate; table wasn't expanded");
}

string
TableDereference::GetText(TextStyle style) const
{
    return table->GetText(style)
        + "[" + index1->GetText(style) + "]"
        + "[" + index2->GetText(style) + "]";
}

void
TableDereference::Visit(ModelVisitor *visitor)
{
    visitor->Visit(this);
}
