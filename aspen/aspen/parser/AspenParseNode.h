// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef ASPEN_PARSE_NODE_H
#define ASPEN_PARSE_NODE_H

#include "common/AST.h"
#include "model/ASTStatement.h"
#include "app/ASTKernel.h"
#include "model/ASTMapModel.h"
#include "model/ASTAppModel.h"
#include "model/ASTMachModel.h"
#include "map/ASTMapStatement.h"
#include "app/ASTControlStatement.h"
#include "app/ASTControlIfThenStatement.h"
#include "app/ASTControlIterateStatement.h"
#include "app/ASTControlKernelCallStatement.h"
#include "app/ASTControlMapStatement.h"
#include "app/ASTControlParallelStatement.h"
#include "app/ASTControlProbabilityStatement.h"
#include "app/ASTControlSequentialStatement.h"
#include "app/ASTControlSwitchStatement.h"
#include "app/ASTExecutionBlock.h"
#include "app/ASTExecutionStatement.h"
#include "model/ASTAssignStatement.h"
#include "model/ASTAssignRangeStatement.h"
#include "model/ASTAssignEnumStatement.h"
#include "model/ASTEnumDeclaration.h"
#include "model/ASTDataStatement.h"
#include "app/ASTImportStatement.h"
#include "app/ASTRequiresStatement.h"
#include "app/ASTSampleStatement.h"
#include "parser/ParseVector.h"
#include "expr/Array.h"
#include "expr/ArrayDereference.h"
#include "expr/Table.h"
#include "expr/TableDereference.h"
#include "expr/UnaryExpr.h"

// ****************************************************************************
// Struct:  AspenParseNode
//
// Purpose:
///   C++ equivalent of the union type used in Bison parsers.
//
// Programmer:  Jeremy Meredith
// Creation:    May 29, 2013
//
// Modifications:
//   Jeremy Meredith, Wed Nov  6 11:43:04 EST 2013
//   Change to ParseVector instead of vector for much faster parsing speed.
//
//Modifications: 
//   Ivy Bo Peng, Nov 2017
//   Change to add mapping model
// ****************************************************************************

struct AspenParseNode {
    int token;
    ASTAppModel  *app;
    ASTMachModel *mach;

  ASTMapModel *map;
  ASTMap *localmap;
  ASTMapStatement *mapstmt;
  ParseVector<ASTMapStatement*> mapstmtlist;

    ASTStatement *stmt;
    ASTExecutionStatement *execstmt;
    Expression *expr;
    ASTKernel *kernel;
    ASTControlStatement *controlstmt;
    ASTKernelArgDecl *argdecl;
    ASTTrait *trait;
    ASTMachProperty *property;
    ASTMachConnectivity *connect;
    ASTNode *astnode;

    ParseVector<ASTStatement*> stmtlist;
    ParseVector<ASTNode*> astnodelist;
    ParseVector<ASTExecutionStatement*> execstmtlist;
    ParseVector<Expression*> exprlist;
    ParseVector<ASTAssignStatement*> assignlist;
    ParseVector<ASTKernel*> kernellist;
    ParseVector<ASTControlStatement*> controlstmtlist;
    ParseVector<ASTKernelArgDecl*> argdecllist;
    ParseVector<ASTTrait*> traitlist;
    ParseVector<ASTCaseItem*> caselist;
    ParseVector<string> stringlist;
    ParseVector<double> valuelist;

    ASTSubComponent *subcomponent;
    ASTMachComponent *component;
    ASTMachPower *power;
    ASTMachResource *resource;
    ASTResourceConflict *conflict;

    ParseVector<ASTTraitDefinition*> traitdeflist;
    ParseVector<pair<string,string> > conflictlist;

    pair<ParseVector<ASTStatement*>, ParseVector<ASTMachComponent*> > machcontent;

    Identifier *ident;
    FunctionCall *call;
    string str;
};

#define YYSTYPE AspenParseNode


#endif
