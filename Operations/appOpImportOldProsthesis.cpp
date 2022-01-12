/*=========================================================================
Program:   AssemblerPro
Module:    appOpImportOldProsthesis.cpp
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "appDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "appDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "appOpImportOldProsthesis.h"
#include "appDecl.h"
#include "appGUI.h"

#include "albaGUI.h"
#include "albaVME.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(appOpImportOldProsthesis);

//----------------------------------------------------------------------------
appOpImportOldProsthesis::appOpImportOldProsthesis(wxString label) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = false;
}

//----------------------------------------------------------------------------
appOpImportOldProsthesis::~appOpImportOldProsthesis()
{
}

//----------------------------------------------------------------------------
bool appOpImportOldProsthesis::Accept(albaVME *node)
{
}

//----------------------------------------------------------------------------
albaOp* appOpImportOldProsthesis::Copy()
{
	appOpImportOldProsthesis *cp = new appOpImportOldProsthesis(m_Label);
	return cp;
}
//----------------------------------------------------------------------------
void appOpImportOldProsthesis::OpRun()
{
	albaString wildc = "MSF file (*.msf)|*.msf|Zip file (*.vtk)|*.vtk";
	wxString imagePath = albaGetOpenFile(fileNameFullPath.GetCStr(), wildc, "Select file").c_str();
	
	OpStop(OP_RUN_OK);
}
//----------------------------------------------------------------------------
void appOpImportOldProsthesis::OpStop(int result)
{
	

	albaEventMacro(albaEvent(this, result));
}
//----------------------------------------------------------------------------
void appOpImportOldProsthesis::OpDo()
{
}

