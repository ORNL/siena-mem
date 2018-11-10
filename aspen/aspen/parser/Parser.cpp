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

#include "Parser.h"

#include <iostream>
#include <deque>
#include <cstdio>
#include <map>
#include <cstring>
#include <libgen.h>

#include "parser/NamespacePrefixer.h"

using namespace std;

// flex and bison are funny; you just define a bunch
// of externs instead of having an include file....

extern vector<int> lineno;
extern vector<int> column;
extern vector<int> filepos;
extern vector<string> filename;
extern string parsedir;

extern int yyparse();
extern FILE *yyin;
extern void yyrestart(FILE*);
extern void yy_scan_string(const char*);
extern ASTAppModel *globalapp;
extern ASTMachModel *globalmach;
extern ASTMapModel  *globalmap;

bool ParseSingleModelString(const std::string &str, ASTAppModel *&app, ASTMachModel *&mach)
{
    // We'll return either the app model or the mach model.
    globalapp = NULL;
    globalmach = NULL;

    // set the state we use in the parser for
    // tracking position (for errors, etc.)
    parsedir = "";
    filename.push_back("<in-memory>");
    lineno.push_back(1);
    column.push_back(1);
    filepos.push_back(0);

    yy_scan_string(str.c_str());
    int err = yyparse();
    if (err)
    {
        cerr << "Error parsing string." << endl;
        return false;
    }

    // Set output values.
    app = globalapp;
    mach = globalmach;

    // Success!
    return true;
}

bool ParseSingleFile(const std::string &fn, ASTAppModel *&app, ASTMachModel *&mach)
{
    // We'll return either the app model or the mach model.
    globalapp = NULL;
    globalmach = NULL;

    // set the state we use in the parser for
    // tracking position (for errors, etc.)
    char *tmp = strdup(fn.c_str());
    parsedir = dirname(tmp);
    free(tmp);
    filename.push_back(fn);
    lineno.push_back(1);
    column.push_back(1);
    filepos.push_back(0);

    FILE *f = fopen(fn.c_str(), "r");
    if (!f)
    {
        cerr << "Error: could not open file '"<<fn<<"' for parsing." << endl;
        return false;
    }

    yyin = f;
    // Need yyrestart because of the <<EOF>> manual handling 
    // due to include file parsing in lexer.
    yyrestart(yyin);
    int err = yyparse();
    fclose(f);
    if (err)
    {
        cerr << "Error parsing file '"<<fn<<"'." << endl;
        return false;
    }

    // Set output values.
    app = globalapp;
    mach = globalmach;

    // Success!
    return true;
}

bool ResolveImports(ASTAppModel *app, string basedir, const vector<string> origprefixstack)
{
    // If we have an app model, we need to resolve any of is import
    // statements, which in turn may also have import statements.
    // Keep a queue of app models to walk to resolve their imports.

    // collect the import statements first
    vector<const ASTImportStatement*> imports;
    for (unsigned int i=0; i<app->GetGlobals().size(); ++i)
    {
        ASTImportStatement *imp = dynamic_cast<ASTImportStatement*>(app->GetGlobals()[i]);
        if (imp)
            imports.push_back(imp);
    }

    for (unsigned int i=0; i<imports.size(); ++i)
    {
        const ASTImportStatement *imp = imports[i];
        if (imp)
        {
            // We used to only import each imported file once.
            // however; we are lacking a way to clone the imported
            // app models, which means we can't safely override
            // the parameters specified in the import statemtn.
            // As such, re-import each time we encounter a file;

            //cerr << "While parsing "<<app->GetName()<<" (basedir="<<basedir<<") "<<": need to import '"<<imp->GetModel()<<"' from file \""<<imp->GetFile()<<"\"\n";
            ASTAppModel *importedapp;
            ASTMachModel *importedmach;
            string filename = (basedir == "") ? imp->GetFile() : basedir + "/" + imp->GetFile();
            bool success = ParseSingleFile(filename, importedapp, importedmach);

            if (!success)
            {
                cerr << "Error parsing file '"<<filename<<"'.\n";
                return false;
            }
            if (!importedapp)
            {
                cerr << "Could not parse '"<<filename<<"' as an app model.\n";
                return false;
            }
            if (importedapp->GetName() != imp->GetModel())
            {
                cerr << "Error: Expected to find "<<imp->GetModel()<<" in file \""<<filename<<"\"\n";
                return false;
            }

            vector<string> prefixstack(origprefixstack);
            prefixstack.push_back(importedapp->GetName());
            string fullprefix = "";
            for (signed int p=prefixstack.size()-1; p>=0; --p)
            {
                const string &prefix = prefixstack[p];

                NamespacePrefixer *prefixer = new NamespacePrefixer(prefix);
                importedapp->Visit(prefixer);
                delete prefixer;

                fullprefix = prefix + "." + fullprefix;
            }

            // Override the parameters from import statement
            for (unsigned int arg=0; arg<imp->GetArguments().size(); ++arg)
            {
                ///\todo: some of the assignment statements are
                /// for *data* volume idents, not for parameters.....
                const ASTAssignStatement *reassign = imp->GetArguments()[arg];

                for (unsigned int stmt = 0 ; stmt < importedapp->GetGlobals().size(); ++stmt)
                {
                    ASTAssignStatement *assign = dynamic_cast<ASTAssignStatement*>(importedapp->GetGlobals()[stmt]);
                    if (assign && assign->GetName() == fullprefix + reassign->GetName())
                    {
                        assign->ChangeValue( reassign->GetValue()->Cloned() );
                    }
                }
            }

            // Resolve any imports recursively (and pull those things into
            // out imported app's namespace before pulling them into our
            // parent app).
            bool resolved = ResolveImports(importedapp, parsedir, prefixstack);
            if (!resolved)
                return false;

            // Okay, we're done importing and prefixing namespaces, just
            // pull the globals and kernels in.
            for (unsigned int i=0; i<importedapp->GetGlobals().size(); ++i)
            {
                ///\todo: probably don't bother pulling in any import statements.
                /// (but then again, should strip them from the main app, too if we
                /// get rid of them.)  And no, updating the namespace prefixer
                /// to fix these up doesn't work, because it adds the prefix
                /// before trying to resolving any nested imports; we'd have
                /// to fix them up after resolving its imports.
                //if (dynamic_cast<ASTImportStatement*>(importedapp->GetGlobals()[i])) continue;

                app->AddGlobal(importedapp->GetGlobals()[i]);
            }

            for (unsigned int i=0; i<importedapp->GetKernels().size(); ++i)
                app->AddKernel(importedapp->GetKernels()[i]);

            //cerr << " ...adding to model "<<app->GetName()<<" import named "<<imp->GetModel()<<endl;
        }
    }
    return true;
}

bool LoadAppOrMachineModel(const std::string &fn,
                           ASTAppModel *&app, ASTMachModel *&mach)
{
    app = NULL;
    mach = NULL;

    bool success = ParseSingleFile(fn, app, mach);
    if (!success)
        return false;

    if (mach)
    {
        mach->CompleteAndCheck();
        return true;
    }

    if (app)
    {
        // take the directory where the app model was
        // parsed, and use it as the base directory
        // for any import statements.
        success = ResolveImports(app, parsedir);
        app->CompleteAndCheck();
        return success;
    }

    // no machine or app?  logic error....
    return false;
}


ASTAppModel *LoadAppModel(const std::string &fn)
{
    ASTAppModel *app = NULL;
    ASTMachModel *mach = NULL;

    bool success = ParseSingleFile(fn, app, mach);
    if (!success)
        THROW(ModelError, "Unknown error parsing model ",fn);

    if (mach)
        THROW(ModelError, "Expected app model, got machine model");

    if (!ResolveImports(app, parsedir))
        THROW(ModelError, "Error while resolving imports");

    app->CompleteAndCheck();

    return app;
}


ASTMachModel *LoadMachineModel(const std::string &fn)
{
    ASTAppModel *app = NULL;
    ASTMachModel *mach = NULL;

    bool success = ParseSingleFile(fn, app, mach);
    if (!success)
        THROW(ModelError, "Unknown error parsing model ",fn);

    if (app)
        THROW(ModelError, "Expected machine model, got app model");

    mach->CompleteAndCheck();

    return mach;
}

bool LoadAppAndMachineModels(const std::string &fn1,
                             const std::string &fn2,
                             ASTAppModel *&app, ASTMachModel *&mach)
{
    ASTAppModel *app1=NULL, *app2=NULL;
    ASTMachModel *mach1=NULL, *mach2=NULL;

    bool success1 = LoadAppOrMachineModel(fn1, app1, mach1);
    if (!success1)
        return false;

    bool success2 = LoadAppOrMachineModel(fn2, app2, mach2);
    if (!success2)
        return false;

    if ((app1 && app2) || (mach1 && mach2))
    {
        cerr << "Error: expected one app model and one machine model.\n";
        return false;
    }

    if (app1 && mach2)
    {
        app = app1;
        mach = mach2;
    }
    else if (app2 && mach1)
    {
        app = app2;
        mach = mach1;
    }
    else
    {
        cerr << "Logic error: expected an app model from one file and a "
             << "machine model from the other.\n";
        return false;
    }

    return true;
}

bool ParseSingleFileIncMapping(const std::string &fn, 
			       ASTAppModel *&app, 
			       ASTMachModel *&mach,
			       ASTMapModel *&mapping)
{
    globalapp  = NULL;
    globalmach = NULL;
    globalmap  = NULL;

    char *tmp = strdup(fn.c_str());
    parsedir = dirname(tmp); //valgrind complains here
    free(tmp);
    filename.push_back(fn);
    lineno.push_back(1);
    column.push_back(1);
    filepos.push_back(0);

    FILE *f = fopen(fn.c_str(), "r");
    if (!f)
    {
        cerr << "Error: could not open file '"<<fn<<"' for parsing." << endl;
        return false;
    }

    yyin = f;
    // Need yyrestart because of the <<EOF>> manual handling 
    // due to include file parsing in lexer.
    yyrestart(yyin);
    int err = yyparse();
    fclose(f);
    if (err)
    {
        cerr << "Error parsing file '"<<fn<<"'." << endl;
        return false;
    }

    // Set output values.
    app = globalapp;
    mach = globalmach;
    mapping = globalmap;

    // Success!
    return true;
}

bool LoadAppOrMachOrMapModel(const std::string &fn,
			     ASTAppModel *&app, ASTMachModel *&mach, ASTMapModel *&mapping)
{
    app = NULL;
    mach = NULL;
    mapping = NULL;

    bool success = ParseSingleFileIncMapping(fn, app, mach, mapping);
    if (!success){
        cerr << "Error: failed in ParseSingleFileIncMapping: "<< fn <<endl;
        return false;
    }

    if (mach)
    {
        mach->CompleteAndCheck();
        return true;
    }

    if (app)
    {
        // take the directory where the app model was
        // parsed, and use it as the base directory
        // for any import statements.
        success = ResolveImports(app, parsedir);
        app->CompleteAndCheck();
        return success;
    }
    

    //Checking Mapping after machine and application models are established
    if (mapping)
    {
          //postpone the checking later when app and mach models are established
          return true;
    }

    cerr << "Error: cannot parse file "<< fn<< ": not app, machine or mapping model.\n";
    return false;
}


bool LoadAppandMachandMapModel(const std::string &fn1,
			       const std::string &fn2,
			       const std::string &fn3,
			       ASTAppModel  *&app, 
			       ASTMachModel *&mach,
			       ASTMapModel  *&mapping)
{
  ASTAppModel  *apps[3]={NULL};
  ASTMachModel *machs[3]={NULL};
  ASTMapModel  *mappings[3]={NULL};
  string files[]={fn1, fn2, fn3};

  ASTAppModel  *app0 = NULL;
  ASTMachModel *mach0= NULL;
  ASTMapModel  *mapping0 = NULL;
  app = NULL;
  mach=NULL;
  mapping=NULL;

  for(size_t i=0;i<3;i++){
 
    bool success = LoadAppOrMachOrMapModel(files[i], apps[i], machs[i], mappings[i]);
    if (!success)
      return false;
    
    if(apps[i] && app0){
      cerr << "Error: more than one app model is found.\n";
      return false;
    }else if(apps[i])
      app0 = apps[i] ;
    
    if(machs[i] && mach0){
      cerr << "Error: more than one machine model is found.\n";
      return false;
    }else if(machs[i])
      mach0 = machs[i];

    if(mappings[i] && mapping0){
      cerr << "Error: more than one mapping model is found.\n";
      return false;
    }else if(mappings[i]){
      mapping0 = mappings[i] ;}
  }
  

  if(app0){
    //app0->Print(cout);  
    app = app0;
  }else{
      cerr << "Error: app model is missing.\n";
      return false;
  }
    
  if(mach0){
    //mach0->Print(cout);
    mach = mach0;
  }else{
      cerr << "Error: mach model is missing.\n";
      return false;
  }
    
  if(mapping0){
      mapping = mapping0;
      mapping->CompleteAndCheck(app, mach);
  }

  if (app && mach && mapping){
    return true;
  }else
    {
        cerr << "Error: expected one app model, one machine model and mapping model.\n";
        return false;
    }

}

