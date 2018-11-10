// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_MAPPING_RESTRICTION_H
#define AST_MAPPING_RESTRICTION_H

class MappingRestriction
{
  public:
    string type;
    string name;
  public:
    MappingRestriction() : type(""), name("") { }
    MappingRestriction(string type, string name) : type(type), name(name) { }
    bool IsEmpty() { return type=="" || name == ""; }
};

#endif
