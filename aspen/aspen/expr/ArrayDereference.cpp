// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "expr/ArrayDereference.h"

#include "expr/Array.h"
#include "expr/Identifier.h"
#include "common/ParameterStack.h"
#include "expr/Real.h"

ArrayDereference::ArrayDereference(Expression *array,
                                   Expression *index)
    : array(array), index(index)
{
    if (!array || !index)
    {
        THROW(LogicError, "Null value in array dereference constructor!");
    }
}

ArrayDereference::~ArrayDereference()
{
    delete array;
    delete index;
}

void
ArrayDereference::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "ArrayDeref:"<<endl;
    out << Indent(indent+1) << "Array:" << endl;
    array->Print(out, indent+2);
    out << Indent(indent+1) << "Index:" << endl;
    index->Print(out, indent+2);
}

Expression*
ArrayDereference::Cloned() const
{
    return new ArrayDereference(array->Cloned(),
                                index->Cloned());
}

bool
ArrayDereference::Equivalent(const Expression *e) const
{
    const ArrayDereference *ade =
        dynamic_cast<const ArrayDereference*>(e);
    if (!ade)
        return false;
    if (! array->Equivalent(ade->array))
        return false;
    return index->Equivalent(ade->index);
}

void
ArrayDereference::ExpandInPlace(const ParameterStack &paramMap, set<string> expandedset)
{
    THROW(LogicError, "ExpandInPlace should never reach ArrayDereference");
}

Expression*
ArrayDereference::Expanded(const ParameterStack &paramMap, set<string> expandedset) const
{
    // start by expanding the index
    Expression *expanded_index = index->Expanded(paramMap, expandedset);

    // see if we have an array expression for our array
    const Array *arrayexpr = 
        dynamic_cast<const Array*>(array);

    // if not, see if it's an identifier that expands into an array
    if (!arrayexpr)
    {
        // if we don't have an identifier as our array, or if it's not
        // in our param map, then there's not much else we can do,
        // so just expand the index side and defer
        const Identifier *ident = dynamic_cast<const Identifier*>(array);
        if (!ident || !paramMap.Find(ident->GetName()))
            return new ArrayDereference(array->Expanded(paramMap, expandedset),
                                        expanded_index);

        // That identifier had better now expand into an array.
        // In theory, we could support ways of generating an array,
        // but right now the only way to create one is via a
        // named parameter, so throw an error if it's not an array.
        arrayexpr = dynamic_cast<const Array*>(paramMap.Find(ident->GetName()));
        if (!array)
            THROW(GrammaticalError, "Expected ", ident->GetName(), " to be an array");
    }

    // okay, we now have an array expression.
    // see if we can generate an real integer index.
    try
    {
        int int_index = expanded_index->Evaluate();
        // success!
        delete expanded_index;

        if (int_index<0 || int_index>=(int)arrayexpr->values.size())
            THROW(GrammaticalError, "Index out of range dereferencing array");
        return new Real(arrayexpr->values[int_index]);
    }
    catch (...)
    {
        // nope; just defer expansion until later
        return new ArrayDereference(arrayexpr->Cloned(),
                                    expanded_index);
    }
}

double
ArrayDereference::Evaluate() const
{
    THROW(GrammaticalError, "Can't evaluate; array wasn't expanded");
}

string
ArrayDereference::GetText(TextStyle style) const
{
    return array->GetText(style) + "[" + index->GetText(style) + "]";
}

void
ArrayDereference::Visit(ModelVisitor *visitor)
{
    visitor->Visit(this);
}
