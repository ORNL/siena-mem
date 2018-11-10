#ifndef NAME_MAP_H
#define NAME_MAP_H

#include <map>
#include <string>
#include <vector>

#include "common/Exception.h"

// ****************************************************************************
// Class:  NameMap
//
// Purpose:
///   Like a STL map where the first type is a string (i.e. map<string, T>),
///   except it supports const versions with error checking, and its
///   constructors include a way to start with a single item to the map.
//
// Programmer:  Jeremy Meredith
// Creation:    September 19, 2013
//
// Modifications:
// ****************************************************************************
template <class T>
class NameMap
{
  public:
    typedef typename std::map<std::string, T>::iterator iterator;
    typedef typename std::map<std::string, T>::const_iterator const_iterator;

  protected:
    std::map<std::string, T> items;
  public:
    /// Default constructor.
    NameMap()
    {
    }
    /// Copy constructor.
    NameMap(const NameMap &nm)
    {
        items.insert(nm.items.begin(), nm.items.end());
    }
    /// Convenience constructor so you can create a map with
    /// an item in it in a single step.
    NameMap(const std::string &n, T v)
    {
        items[n] = v;
    }
    /// Insert all items from another map, allowing overwriting
    void Insert(const NameMap &nm)
    {
        for (const_iterator it = nm.begin(); it != nm.end(); ++it)
        {
            items[it->first] = it->second;
        }
    }
    /// Const array subscript operator can't create an entry, so
    /// we assume this item already exists, and error if not.
    const T &operator[](const std::string &n) const
    {
        typename std::map<std::string, T>::const_iterator it = items.find(n);
        if (it == items.end())
            THROW(LogicError, "No item '", n, "' ", "in const NameMap");

        return it->second;
    }
    /// Standard array subscript operator just redirects to map[n],
    /// creating the entry if it doesn't exist yet.
    T& operator[](const std::string &n)
    {
        return items[n];
    }
    /// Erase a single item
    void Erase(const std::string &n)
    {
        items.erase(n);
    }
    void Erase(iterator pos) {
        items.erase(pos);
    }
    void Erase(iterator start, iterator end) {
        items.erase(start, end);
    }
    /// True if map contains no items.
    bool Empty() const
    {
        return items.empty();
    }
    /// True if map contains the given item.
    bool Contains(const std::string &n) const
    {
        return items.count(n) != 0;
    }
    /// Returns a vector of keys of the map
    // NOTE: maybe cache if we see a performance issue.
    std::vector<std::string> Keys() const
    {
        std::vector<std::string> keys;
        typename std::map<std::string, T>::const_iterator it;
        for(it = items.begin(); it != items.end(); it++)
            keys.push_back(it->first);
        return keys;
    }

    iterator begin() { return items.begin(); }
    iterator end()   { return items.end();  }
    const_iterator begin() const { return items.begin(); }
    const_iterator end() const   { return items.end();  }
};

#endif
