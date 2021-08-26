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
#include "appGUI.h"
#include "appGUIDialogComponent.h"
#include "appGUIDialogProsthesis.h"
#include "appGUIDialogProducer.h"
#include "appLogic.h"
#include "appUtils.h"

#include "albaGUI.h"
#include "albaGUIButton.h"
#include "albaGUIDialog.h"
#include "albaGUILab.h"
#include "albaGUIPicButton.h"
#include "albaGUIValidator.h"
#include "albaProsthesesDBManager.h"
#include "albaServiceClient.h"
#include "albaVME.h"

#include "wx\image.h"
#include "wx\window.h"
#include <vector>

//----------------------------------------------------------------------------
albaCxxTypeMacro(appOpCreateProsthesis);

//----------------------------------------------------------------------------
appOpCreateProsthesis::appOpCreateProsthesis(wxString label) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;
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
	AddProsthesis();

	if (m_CurrentProsthesis.isChanged)
		SaveProsthesis();

	OpStop(OP_RUN_OK);
}
//----------------------------------------------------------------------------
void appOpCreateProsthesis::OpStop(int result)
{
	albaEventMacro(albaEvent(this, result));
}

//////////////////////////////////////////////////////////////////////////
/// Prosthesis Management
//----------------------------------------------------------------------------
void appOpCreateProsthesis::AddProsthesis()
{
	// Create New Prosthesis
	m_CurrentProsthesis.name = "newProsthesis";
	m_CurrentProsthesis.producer = "";
	m_CurrentProsthesis.image = "";
	m_CurrentProsthesis.type = "";
	m_CurrentProsthesis.side = 0; // = LEFT
	m_CurrentProsthesis.isChanged = false;

	appGUIDialogProsthesis md(_("Add Prosthesis"));
	md.SetProsthesis(m_CurrentProsthesis);
	md.ShowModal();

	UpdateProsthesis(md.GetProsthesis());
}
//----------------------------------------------------------------------------
void appOpCreateProsthesis::UpdateProsthesis(AuxProsthesis prosthesis)
{
	// Update Vector Element
	m_CurrentProsthesis.name = prosthesis.name;
	m_CurrentProsthesis.image = prosthesis.image;
	m_CurrentProsthesis.producer = prosthesis.producer;
	m_CurrentProsthesis.type = prosthesis.type;
	m_CurrentProsthesis.side = prosthesis.side;
	m_CurrentProsthesis.isChanged = prosthesis.isChanged;
}
//----------------------------------------------------------------------------
void appOpCreateProsthesis::SaveProsthesis()
{
	albaProDBProshesis *newProsthesis = new albaProDBProshesis();

	newProsthesis->SetName(m_CurrentProsthesis.name);
	newProsthesis->SetImgFileName(m_CurrentProsthesis.image);
	newProsthesis->SetProducer(m_CurrentProsthesis.producer);
	newProsthesis->SetType(m_CurrentProsthesis.type);
	newProsthesis->SetSide((albaProDBProshesis::PRO_SIDES)m_CurrentProsthesis.side);
	
	// Add New Prosthesis to DB
	albaProsthesesDBManager *DBManager = ((appLogic*)GetLogicManager())->GetProsthesesDBManager();
	DBManager->GetProstheses().push_back(newProsthesis);

	std::vector<albaProDBType *> DBTypes = DBManager->GetTypes();

	bool newType = true;

		for (int t = 0; t < DBTypes.size(); t++)
		{
			if (DBTypes[t]->GetName().GetCStr() == m_CurrentProsthesis.type)
			{
				newType = false;
				break;
			}
		}
		if (newType)
		{
			albaProDBType *newType = new albaProDBType();
			newType->SetName(m_CurrentProsthesis.type);
			DBManager->GetTypes().push_back(newType);
		}

	//////////////////////////////////////////////////////////////////////////
	DBManager->SaveDB();

	wxString message = wxString::Format("Added New Prosthesis!");
	wxMessageBox(message);
}
