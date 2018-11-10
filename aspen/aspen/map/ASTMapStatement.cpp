// Copyright 2013-2017 UT-Battelle, LLC.  See LICENSE.txt for more information.
// ****************************************************************************
// Class:     ASTMapStatement
//
// Purpose:   Map Application Data/Kernel to Machine Components
//
// Programmer:  Ivy Bo PENG
// Creation:    Nov, 2017
//
// Modifications:
// ****************************************************************************

#include "map/ASTMapStatement.h"

void ASTMapStatement::Print(ostream &out, int indent)const
  {
    out << Indent(indent)<< " Mapping: " 
	<< app_comp->GetName() << " to " << mach_comp->GetName()<< endl;
  }
