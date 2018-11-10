// Copyright 2013-2017 UT-Battelle, LLC.  See LICENSE.txt for more information.
// ****************************************************************************
// Class:     ASTMap
//
// Purpose:   Map Application Data/Kernel to Machine Components
//
// Programmer:  Ivy Bo PENG
// Creation:    Nov, 2017
//
// Modifications:
// ****************************************************************************

#ifndef AST_MAP_H
#define AST_MAP_H

#include "map/ASTMapStatement.h"
#include "mach/ASTMachComponent.h"
#include "model/ASTAppModel.h"
#include "model/ASTMachModel.h"

class ASTMap
{
protected:
    MapType  type;
    string map_name;
    const vector<ASTMapStatement*> mapstmts;
    ASTAppModel  *app;
    ASTMachModel *mach;
    
    NameMap<const ASTMachComponent*> datakernelMap;
    
public:
 
    ASTMap(int _type, ParseVector<ASTMapStatement*> _mapstmts);

    ~ASTMap(){}

    void CompleteAndCheck(ASTAppModel  *_app, ASTMachModel *_mach);
    
    void Print(ostream &out, int indent = 0) const;
    void Export(ostream &out, int indent = 0) const{}
    const string& GetName() const
    {
      return map_name;
    }
    const ASTMachComponent* get_mapping(string name)const{
        if( !datakernelMap.Contains(name) )
            return NULL;
        else
            return datakernelMap[name];
    }

};



#endif
