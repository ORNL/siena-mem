// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_NAMESPACE_PREFIXER_H
#define AST_NAMESPACE_PREFIXER_H

#include "common/ModelVisitor.h"

#include "model/ASTAssignStatement.h"
#include "app/ASTSampleStatement.h"
#include "app/ASTImportStatement.h"
#include "app/ASTControlKernelCallStatement.h"

class NamespacePrefixer : public ModelVisitor
{
    string prefix;
  public:
    NamespacePrefixer(string prefix) : prefix(prefix)
    {
    }
    virtual ~NamespacePrefixer()
    {
    }
    virtual bool Visit(ASTNode *astnode)
    {
        Identifier *ident = dynamic_cast<Identifier*>(astnode);
        ASTAssignStatement *assign = dynamic_cast<ASTAssignStatement*>(astnode);
        ASTSampleStatement *sample = dynamic_cast<ASTSampleStatement*>(astnode);
        ASTKernelArgDecl *vardecl = dynamic_cast<ASTKernelArgDecl*>(astnode);
        ASTKernel *kernel = dynamic_cast<ASTKernel*>(astnode);
        ASTControlKernelCallStatement *call = dynamic_cast<ASTControlKernelCallStatement*>(astnode);
        ASTImportStatement *import = dynamic_cast<ASTImportStatement*>(astnode);

        if (ident)
        {
            ident->Prefix(prefix);
        }
        else if (assign)
        {
            assign->ident->Prefix(prefix);
        }
        else if (sample)
        {
            sample->ident->Prefix(prefix);
        }
        else if (vardecl)
        {
            vardecl->ident->Prefix(prefix);
        }
        else if (kernel)
        {
            kernel->ident->Prefix(prefix);
        }
        else if (call)
        {
            call->ident->Prefix(prefix);
        }
        else if (import)
        {
            // Import is special, because it explicitly combines two namespaces;
            // the child is on the left of the assignment statements, and
            // the parent is on the right.  We leave the child alone, because 
            // when we import the child, its name has not yet been prefixed.
            // But the parent has already been prefixed, so we must make
            // e.g. "with n=n" become "with n=thismodel.n".
            // So descend manually into the import, ONLY prefix the RHS, and then
            // return 'true' to stop the automatic descent.
            for (unsigned int i=0; i<import->arguments.size(); ++i)
                import->arguments[i]->value->Visit(this);
            return true;
        }
        
        return false;
    }
};

#endif

