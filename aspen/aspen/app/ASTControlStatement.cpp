// Copyright 2013-2015 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "app/ASTControlStatement.h"

ASTControlStatement::ASTControlStatement(string label) : label(label)
{
    static int nindex = 0;
    char tmp[100];
    sprintf(tmp, "%03d", nindex+100);
    nodeid = tmp;
    ++nindex;
}


