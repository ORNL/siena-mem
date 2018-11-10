#ifndef PARSE_VECTOR_H
#define PARSE_VECTOR_H

#include "common/Exception.h"
#include <vector>

// ****************************************************************************
// Class:  ParseVector
//
// Purpose:
///   STL vector pointer container with deferred creation.
///   When using a union as a Bison parse tree node, you can't
///   use real vectors, because a union doesn't support anything
///   but POD (i.e. no constructors).  If you use a struct instead
///   of a union, you can, but then you incur significant expenses;
///   each construction allocates memory and copies are expensive,
///   while Bison allocates and throws these around making copies 
///   as if they were free.  Ideally you could just use raw pointers,
///   allocating and deleting as needed in the parse grammar code,
///   but that is ugly and hard to maintain.  So this class gives
///   you the best compromise; it acts like a pointer but with
///   the calling semantics of a vector object.
///   \todo: We're leaking the data.  Either add a method to free the vector
///          and call it from the grammar after we've created the AST object
///          using its contents, or add every constructed vector to a global
///          list for later garbage collection.
//
// Programmer:  Jeremy Meredith
// Creation:    November  6, 2013
//
// Modifications:
// ****************************************************************************
template<class T>
class ParseVector
{
  protected:
    std::vector<T> *data;
    void CreateData()
    {
        data = new std::vector<T>;
    }
  public:
    ParseVector()
    {
        data = NULL;
    }
    ParseVector(const ParseVector<T> &rhs)
    {
        data = rhs.data;
    }
    void operator=(const ParseVector<T> &rhs)
    {
        data = rhs.data;
    }
    void push_back(const T &t)
    {
        if (!data)
            THROW(LogicError, "Push back element before construction");
        data->push_back(t);
    }
    void append(const ParseVector<T> &rhs)
    {
        if (!data)
            THROW(LogicError, "Append before construction");
        data->insert(data->end(), rhs.begin(), rhs.end());
    }
    typename std::vector<T>::iterator begin()
    {
        if (!data)
            CreateData();
        return data->begin();
    }
    typename std::vector<T>::iterator end()
    {
        if (!data)
            CreateData();
        return data->end();
    }
    typename std::vector<T>::const_iterator begin() const
    {
        if (!data)
            THROW(LogicError, "Cannot create const iterator on empty array");
        return data->begin();
    }
    typename std::vector<T>::const_iterator end() const
    {
        if (!data)
            THROW(LogicError, "Cannot create const iterator on empty array");
        return data->end();
    }
    const T &operator[](int i) const
    {
        if (!data)
            THROW(LogicError, "Accessing array element (const) before construction");
        return data->operator[](i);
    }
    T &operator[](int i)
    {
        if (!data)
            THROW(LogicError, "Accessing array element (non-const) before construction");
        return data->operator[](i);
    }
    size_t size() const
    {
        if (!data)
            return 0;
        return data->size();
    }
    void clear()
    {
        CreateData();
    }
    void release()
    {
        if (data)
            delete data;
        data = NULL;
    }
};

#endif
