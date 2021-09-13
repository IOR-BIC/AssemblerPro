/*=========================================================================
Program:   AssemblerPro
Module:    appOpCreateProsthesisVME.cpp
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

#include "appOpCreateProsthesisVME.h"
#include "appDecl.h"

#include "albaAbsLogicManager.h"
#include "albaGUI.h"
#include "albaVME.h"
#include "albaVMEProsthesis.h"
#include "appVMEProsthesisEdit.h"
#include "albaProsthesesDBManager.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(appOpCreateProsthesisVME);

//----------------------------------------------------------------------------
appOpCreateProsthesisVME::appOpCreateProsthesisVME(wxString label) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;
}
//----------------------------------------------------------------------------
appOpCreateProsthesisVME::~appOpCreateProsthesisVME()
{
}

//----------------------------------------------------------------------------
bool appOpCreateProsthesisVME::Accept(albaVME *node)
{
	return true;
}

//----------------------------------------------------------------------------
albaOp* appOpCreateProsthesisVME::Copy()
{
	appOpCreateProsthesisVME *cp = new appOpCreateProsthesisVME(m_Label);
	return cp;
}
//----------------------------------------------------------------------------
void appOpCreateProsthesisVME::OpRun()
{
	//albaVMEProsthesis *pro;
	appVMEProsthesisEdit *pro;
	albaNEW(pro);
	pro->SetName("New Prosthesis");
	albaProsthesesDBManager * prosthesesDBManager = GetLogicManager()->GetProsthesesDBManager();
	pro->SetProsthesis(prosthesesDBManager->GetProstheses().at(0));
	pro->ReparentTo(m_Input);

	GetLogicManager()->VmeShow(pro, true);
	
	OpStop(OP_RUN_OK);
}