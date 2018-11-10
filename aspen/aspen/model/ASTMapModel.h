// Copyright 2013-2017 UT-Battelle, LLC.  See LICENSE.txt for more information.
// ****************************************************************************
// Class:     ASTMapModel
//
// Purpose:   A model that descries Application-to-Machine mapping
//
// Programmer:  Ivy Bo PENG
// Creation:    Nov, 2017
//
// Modifications:
// ****************************************************************************

#ifndef AST_MAP_MODEL_H
#define AST_MAP_MODEL_H

#include "common/ASTNode.h"
#include "model/ASTAppModel.h"
#include "model/ASTMachModel.h"
#include "map/ASTMap.h"

class ASTMapModel : public ASTNode
{

  public:
    string name;
    vector<ASTStatement*> globals;
    ASTMap* kernelmap;
    ASTMap* datamap;
    ASTAppModel  *app;
    ASTMachModel *mach;
    
    NameMap<const Expression*>       paramMap;
    /*NameMap<const ASTDataStatement*> dataMap;
    NameMap<const ASTKernel*>        kernelMap;
    NameMap<const ASTMachComponent*> compMap;
    */

  public:
    ASTMapModel(string _name, ParseVector<ASTStatement*> _globals,
                ASTMap* _datamap, ASTMap* _kernelmap)
      :name(_name), globals(_globals.begin(), _globals.end()),
    kernelmap(_kernelmap), datamap(_datamap)
    {

    }

    ~ASTMapModel(){}

    void CompleteAndCheck(ASTAppModel  *_app, ASTMachModel *_mach);
    void Print(ostream &out, int indent = 0) const;
    //void Visit(ModelVisitor *visitor);
    const string &GetName() const { return name; }
    const vector<ASTStatement*> &GetGlobals() const { return globals; }
    
    const ASTMachComponent* get_datamapping(string obj_name)const{
        return datamap->get_mapping(obj_name);
    }
    
    const ASTMachComponent* get_kernelmapping(string obj_name)const{
        return kernelmap->get_mapping(obj_name);
    }
};

#endif
