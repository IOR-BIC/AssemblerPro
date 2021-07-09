/*=========================================================================
Program:   AssemblerPro
Module:    appOpExportProsthesisDB.cpp
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

#include "appOpExportProsthesisDB.h"
#include "appDecl.h"
#include "appGUI.h"

#include "albaGUI.h"
#include "albaGUICheckListBox.h"
#include "albaVME.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(appOpExportProsthesisDB);

//----------------------------------------------------------------------------
appOpExportProsthesisDB::appOpExportProsthesisDB(wxString label) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;

	m_DBName = "";
	m_DBVersion = "1.0";

	m_SelectAllProducers = 0;
	m_SelectAllModels = 0;
}

//----------------------------------------------------------------------------
appOpExportProsthesisDB::~appOpExportProsthesisDB()
{
}

//----------------------------------------------------------------------------
bool appOpExportProsthesisDB::Accept(albaVME *node)
{
	//return node->IsA("...");
	return true;
}

//----------------------------------------------------------------------------
char** appOpExportProsthesisDB::GetIcon()
{
#include "pic/MENU_OP_EXPORT_PROSTHESIS.xpm"
	return MENU_OP_EXPORT_PROSTHESIS_xpm;
}

//----------------------------------------------------------------------------
albaOp* appOpExportProsthesisDB::Copy()
{
	appOpExportProsthesisDB *cp = new appOpExportProsthesisDB(m_Label);
	return cp;
}
//----------------------------------------------------------------------------
void appOpExportProsthesisDB::OpRun()
{
	if (!m_TestMode)
	{
		CreateGui();
	}
	
	//OpStop(OP_RUN_OK);
}
//----------------------------------------------------------------------------
void appOpExportProsthesisDB::OpStop(int result)
{
	if (!m_TestMode)
	{
		HideGui();
	}

	albaEventMacro(albaEvent(this, result));
}
//----------------------------------------------------------------------------
void appOpExportProsthesisDB::OpDo()
{
}

//----------------------------------------------------------------------------
void appOpExportProsthesisDB::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		m_Gui->Update();
		//if (e->GetSender() == m_Gui)
		{
			switch (e->GetId())
			{
			case wxOK:
				OpStop(OP_RUN_OK);
				break;

			case wxCANCEL:
				OpStop(OP_RUN_CANCEL);
				break;

			default:
				Superclass::OnEvent(alba_event);
				break;
			}
		}
// 		else
// 		{
// 			Superclass::OnEvent(alba_event);
// 		}
	}
}

//----------------------------------------------------------------------------
void appOpExportProsthesisDB::CreateGui()
{
	// Interface:
	m_Gui = new appGUI(this);

	m_Gui->String(NULL, "Name", &m_DBName);
	m_Gui->String(NULL, "Version", &m_DBVersion);
	m_Gui->Divider(1);

	albaGUICheckListBox *m_ProducerCheckBox = m_Gui->CheckList(NULL, "Producer");
	m_ProducerCheckBox->AddItem(NULL, "Prod1", false);
	m_ProducerCheckBox->AddItem(NULL, "Prod2", false);
	m_ProducerCheckBox->AddItem(NULL, "Prod3", false);
	m_Gui->Bool(NULL, "Select All", &m_SelectAllProducers);

	m_Gui->Divider(1);

	albaGUICheckListBox *m_ModelCheckBox = m_Gui->CheckList(NULL, "Models", 180);
	m_ModelCheckBox->AddItem(NULL, "Mod1", false);
	m_ModelCheckBox->AddItem(NULL, "Mod2", false);
	m_ModelCheckBox->AddItem(NULL, "Mod3", false);
	m_ModelCheckBox->AddItem(NULL, "Mod4", false);
	m_ModelCheckBox->AddItem(NULL, "Mod5", false);
	m_ModelCheckBox->AddItem(NULL, "Mod6", false);
	m_ModelCheckBox->AddItem(NULL, "Mod7", false);
	m_ModelCheckBox->AddItem(NULL, "Mod8", false);
	m_ModelCheckBox->AddItem(NULL, "Mod9", false);
	m_Gui->Bool(NULL, "Select All", &m_SelectAllModels);

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();
	m_Gui->Label("");

	ShowGui();
}