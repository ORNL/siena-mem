// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_H
#define AST_H

#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <map>
#include <set>
#include <sstream>
#include <utility>
#include <memory>
#include <algorithm>
#include <iomanip>

using std::string;
using std::vector;
using std::ostream;
using std::ostringstream;
using std::cout;
using std::cerr;
using std::endl;
using std::map;
using std::set;
using std::pair;
using std::auto_ptr;

inline string Indent(int n) { return string(n, ' '); }

template <class T>
string VecPrint(T *const v, unsigned int n, unsigned int nmax=9, unsigned int group=1e9)
{
    ostringstream out;
    if (n <= nmax)
    {
        for (unsigned int i=0 ; i<n; i++)
        {
            out << v[i];
            if (i<n-1)
            {
                if (i%group == group-1)
                    out << ", ";
                out << " ";
            }
        }
        return out.str();
    }

    unsigned int num = nmax/2;
    unsigned int i=0;
    for ( ; i<num; i++)
    {
        out << v[i];
        if (i%group == group-1)
            out << ", ";
        out << " ";
    }
    out << "... ";
    for (i=n - num ; i<n; i++)
    {
        out << v[i];
        if (i%group == group-1)
            out << ", ";
        if (i<n-1)
            out << " ";
    }
    return out.str();
}

#endif
