// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
/**
 * @file   parser/Parser.cpp
 * @author Jeremy Meredith
 *         jsmeredith@gmail.com
 * The Aspen parser calls yyparse on specified machine and/or application model
 * file(s) written in the Aspen moeling languages and returns the AST(s) for the 
 * parsed files.
 * 
 * Modification:
 *              Entend to load the mapping model
 * Programmer:  Ivy Peng
 * Date:        Nov, 2017
// *****************************************************************************/



#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <map>

/*class ASTAppModel;
class ASTMachModel;
class ASTMapModel;*/
#include "model/ASTMapModel.h"
#include "model/ASTAppModel.h"
#include "model/ASTMachModel.h"


/// Loads a single (app or machine) model, resolving and
/// parsing any imports recursively.
bool LoadAppOrMachineModel(const std::string &fn,
                           ASTAppModel *&app,
                           ASTMachModel *&mach);

/// Loads a single app model, resolving imports recursively.
ASTAppModel *LoadAppModel(const std::string &fn);

/// Loads a single machine model.
ASTMachModel *LoadMachineModel(const std::string &fn);

/// Loads both an app model and a machine model in either order.
bool LoadAppAndMachineModels(const std::string &fn1,
                             const std::string &fn2,
                             ASTAppModel *&app,
                             ASTMachModel *&mach);

bool LoadAppandMachandMapModel(const std::string &fn1,
			       const std::string &fn2,
			       const std::string &fn3,
			       ASTAppModel  *&app,
			       ASTMachModel *&mach,
			       ASTMapModel  *&mapping);

/// Parses a single file, but leaves any imports unresolved.
/// This will successfully parse include directives.
/// This is primarily used for debugging; you should generally
/// use one of the 'Load' routines above instead of calling this,
/// because if we're given file names, we will be able to resolve
/// imports of file names.
bool ParseSingleFile(const std::string &fn,
                     ASTAppModel *&app,
                     ASTMachModel *&mach);

/// Parses a single string, but leaves any imports unresolved.
/// This will fail if it encounters include directives, but is
/// a viable option for self-contained machine and app models.
bool ParseSingleModelString(const std::string &str,
                            ASTAppModel *&app,
                            ASTMachModel *&mach);

// Recursively parses all imported models in an App model.
/// This is primarily used for debugging; you should generally
/// use one of the 'Load' routines above instead of calling this.
bool ResolveImports(ASTAppModel *app, std::string basedir, const std::vector<std::string> prefixstack = std::vector<std::string>());

#endif
