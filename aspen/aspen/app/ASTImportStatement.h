// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_IMPORT_STATEMENT_H
#define AST_IMPORT_STATEMENT_H

#include "model/ASTAssignStatement.h"

class ASTImportStatement : public ASTStatement
{
    friend class NamespacePrefixer;
  protected:
    string model;
    string file;
    vector<ASTAssignStatement*> arguments;
  public:
    ASTImportStatement(string model, string file, ParseVector<ASTAssignStatement*> args)
        : model(model), file(file), arguments(args.begin(), args.end())
    {
    }
    ASTImportStatement(string model, string file)
        : model(model), file(file)
    {
    }
    virtual ~ASTImportStatement()
    {
        for (unsigned int i=0; i<arguments.size(); ++i)
        {
            delete arguments[i];
        }
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Import "<<model<< " from ''"<<file<<"'' with "<<arguments.size()<<" args:" << endl;
        for (unsigned int i=0; i<arguments.size(); i++)
        {
            arguments[i]->Print(out, indent+1);
        }
    }
    virtual void Visit(ModelVisitor *visitor)
    {
        if (visitor->Visit(this))
            return;
        for (unsigned int i=0; i<arguments.size(); ++i)
            arguments[i]->Visit(visitor);
    }

    const string &GetModel() const { return model; }
    const string &GetFile() const { return file; }
    const vector<ASTAssignStatement*> &GetArguments() const { return arguments; }
};

#endif
