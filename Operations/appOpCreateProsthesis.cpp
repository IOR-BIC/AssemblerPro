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

#include "albaAbsLogicManager.h"
#include "albaGUI.h"
#include "albaVME.h"
#include "albaVMEProsthesis.h"
#include "appVMEProsthesisEdit.h"
#include "albaProsthesesDBManager.h"
#include "appGUIDialogProsthesis.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(appOpCreateProsthesis);

//----------------------------------------------------------------------------
appOpCreateProsthesis::appOpCreateProsthesis(wxString label) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;

	m_ProsthesesDBManager = NULL;

	m_VmeProsthesis = NULL;
	m_VMEProsthesisIsCreated = false;
}
//----------------------------------------------------------------------------
appOpCreateProsthesis::~appOpCreateProsthesis()
{
}

//----------------------------------------------------------------------------
bool appOpCreateProsthesis::InternalAccept(albaVME *node)
{
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
	m_ProsthesesDBManager = GetLogicManager()->GetProsthesesDBManager();
	m_VmeProsthesis = GetVMEProsthesisFromTree();

	if (m_VmeProsthesis == NULL)
		m_VmeProsthesis = CreateVMEProshesis();

	if (m_VMEProsthesisIsCreated == false)
		CreateDBProsthesis(m_VmeProsthesis);

	GetLogicManager()->VmeSelect(m_VmeProsthesis);
	GetLogicManager()->VmeShow(m_VmeProsthesis, true);

	OpStop(OP_RUN_OK);
}

//----------------------------------------------------------------------------
appVMEProsthesisEdit *appOpCreateProsthesis::GetVMEProsthesisFromTree()
{
	return appVMEProsthesisEdit::SafeDownCast(m_Input->GetRoot()->GetLink("VMEProsthesis"));
}
//----------------------------------------------------------------------------
appVMEProsthesisEdit * appOpCreateProsthesis::CreateVMEProshesis()
{
	appVMEProsthesisEdit *newVmeProsthesis = NULL;
	albaNEW(newVmeProsthesis);
	newVmeProsthesis->SetName("New Prosthesis");

	newVmeProsthesis->SetProsthesis(m_ProsthesesDBManager->GetProstheses().at(0));
	newVmeProsthesis->ReparentTo(m_Input->GetRoot());

	m_Input->GetRoot()->SetLink("VMEProsthesis", newVmeProsthesis);

	m_VMEProsthesisIsCreated = true;

	return newVmeProsthesis;
}
//----------------------------------------------------------------------------
void appOpCreateProsthesis::CreateDBProsthesis(appVMEProsthesisEdit *VmeProsthesis)
{
	GetLogicManager()->VmeSelect(NULL);
	GetLogicManager()->VmeShow(VmeProsthesis, false);

	// Create New DB Prosthesis
	albaProDBProsthesis *proDB = new albaProDBProsthesis();
	proDB->SetName("New Prosthesis");
	proDB->SetProducer("");
	proDB->SetImgFileName("");
	proDB->SetType("");
	proDB->SetSide(albaProDBProsthesis::PRO_LEFT);

	albaProDBCompGroup *proDBCompGroup = new albaProDBCompGroup();
	proDBCompGroup->SetName("New Group");
	proDB->AddCompGroup(proDBCompGroup);

	albaProDBComponent *proDBComp = new albaProDBComponent();
	proDBComp->SetName("New Component");
	proDBCompGroup->AddComponent(proDBComp);

	// Show dialog
	appGUIDialogProsthesis md(_("Add Prosthesis"));
	md.SetProsthesis(proDB);
	md.ShowModal();

	if (md.OkClosed())
	{
		// Add New Prosthesis to DB
		m_ProsthesesDBManager->GetProstheses().push_back(proDB);
		m_ProsthesesDBManager->SaveDB();

		VmeProsthesis->Reset();
		VmeProsthesis->SetSelection(m_ProsthesesDBManager->GetProstheses().size() - 1);
		//VmeProsthesis->SetProsthesis(proDB);

		wxString message = wxString::Format("Added New Prosthesis!");
		wxMessageBox(message);
	}
	else
	{
		// Reload DB
		m_ProsthesesDBManager->LoadDB();
	}
}