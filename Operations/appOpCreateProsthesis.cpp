/*=========================================================================
Program:   AssemblerPro
Module:    appOpCreateProsthesis.cpp
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

#include "appOpCreateProsthesis.h"
#include "appDecl.h"
#include "appGUIDialogProsthesis.h"
#include "appLogic.h"

#include "albaGUI.h"
#include "albaProsthesesDBManager.h"
#include "albaServiceClient.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(appOpCreateProsthesis);

//----------------------------------------------------------------------------
appOpCreateProsthesis::appOpCreateProsthesis(wxString label) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;

	m_CurrentProsthesis = NULL;
}

//----------------------------------------------------------------------------
appOpCreateProsthesis::~appOpCreateProsthesis()
{
}

//----------------------------------------------------------------------------
bool appOpCreateProsthesis::Accept(albaVME *node)
{
	//return node->IsA("...");
	return true;
}

//----------------------------------------------------------------------------
char** appOpCreateProsthesis::GetIcon()
{
#include "pic/MENU_OP_CREATE_PROSTHESIS.xpm"
	return MENU_OP_CREATE_PROSTHESIS_xpm;
}

//----------------------------------------------------------------------------
albaOp* appOpCreateProsthesis::Copy()
{
	appOpCreateProsthesis *cp = new appOpCreateProsthesis(m_Label);
	return cp;
}
//----------------------------------------------------------------------------
void appOpCreateProsthesis::OpRun()
{
	m_DBManager = ((appLogic*)GetLogicManager())->GetProsthesesDBManager();

	// Create New Prosthesis
	m_CurrentProsthesis = new albaProDBProsthesis();
	m_CurrentProsthesis->SetName("newProsthesis");
	m_CurrentProsthesis->SetProducer("");
	m_CurrentProsthesis->SetImgFileName("");
	m_CurrentProsthesis->SetType("");
	m_CurrentProsthesis->SetSide(albaProDBProsthesis::PRO_LEFT);

	// Show dialog
	appGUIDialogProsthesis md(_("Add Prosthesis"));
	md.SetProsthesis(m_CurrentProsthesis);
	md.ShowModal();

	if (md.OkClosed())
	{
		// Add New Prosthesis to DB
		m_DBManager->GetProstheses().push_back(m_CurrentProsthesis);
		m_DBManager->SaveDB();

		wxString message = wxString::Format("Added New Prosthesis!");
		wxMessageBox(message);
	}
	else
	{
		// Reload DB
		m_DBManager->LoadDB();
	}

	OpStop(OP_RUN_OK);
}
//----------------------------------------------------------------------------
void appOpCreateProsthesis::OpStop(int result)
{
	albaEventMacro(albaEvent(this, result));
}
