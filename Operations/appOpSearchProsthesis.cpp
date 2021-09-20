/*=========================================================================
Program:   AssemblerPro
Module:    appOpSearchProsthesis.cpp
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

#include "appOpSearchProsthesis.h"
#include "appDecl.h"
#include "appGUI.h"
#include "appLogic.h"

#include "albaGUI.h"
#include "albaGUICheckListBox.h"
#include "albaVME.h"
#include "albaServiceClient.h"
#include "albaProsthesesDBManager.h"
#include "wx/ctrlsub.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(appOpSearchProsthesis);

//----------------------------------------------------------------------------
appOpSearchProsthesis::appOpSearchProsthesis(wxString label) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;

	m_SearchString = "";
}

//----------------------------------------------------------------------------
appOpSearchProsthesis::~appOpSearchProsthesis()
{
}

//----------------------------------------------------------------------------
bool appOpSearchProsthesis::Accept(albaVME *node)
{
	//return node->IsA("...");
	return true;
}

//----------------------------------------------------------------------------
char** appOpSearchProsthesis::GetIcon()
{
#include "pic/MENU_OP_SEARCH_PROSTHESIS.xpm"
	return MENU_OP_SEARCH_PROSTHESIS_xpm;
}

//----------------------------------------------------------------------------
albaOp* appOpSearchProsthesis::Copy()
{
	appOpSearchProsthesis *cp = new appOpSearchProsthesis(m_Label);
	return cp;
}
//----------------------------------------------------------------------------
void appOpSearchProsthesis::OpRun()
{
	m_DBManager = ((appLogic*)GetLogicManager())->GetProsthesesDBManager();

	if (!m_TestMode)
	{
		CreateGui();
	}
	
	//OpStop(OP_RUN_OK);
}
//----------------------------------------------------------------------------
void appOpSearchProsthesis::OpStop(int result)
{
	if (!m_TestMode)
	{
		HideGui();
	}

	albaEventMacro(albaEvent(this, result));
}
//----------------------------------------------------------------------------
void appOpSearchProsthesis::OpDo()
{
}

//----------------------------------------------------------------------------
void appOpSearchProsthesis::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		m_Gui->Update();
		//if (e->GetSender() == m_Gui)
		{
			switch (e->GetId())
			{
			case ID_SEARCH_PROSTHESIS:
				Find();
				break;

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
void appOpSearchProsthesis::CreateGui()
{
	// Interface:
	m_Gui = new appGUI(this);

	m_Gui->String(ID_SEARCH_PROSTHESIS, "Find", &m_SearchString, "", false, false, true);

	m_Gui->Divider(1);

	albaGUICheckListBox *m_SearchFilterCheckBox = m_Gui->CheckList(NULL, "Filters");
	m_SearchFilterCheckBox->AddItem(NULL, "Producer", true);
	m_SearchFilterCheckBox->AddItem(NULL, "Type", false);
	m_SearchFilterCheckBox->AddItem(NULL, "Side", false);

	m_Gui->Divider(1);

	m_Gui->Label("Results", true);
	m_ResultsListBox = m_Gui->ListBox(NULL, "");

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();
	m_Gui->Label("");

	ShowGui();
}
//----------------------------------------------------------------------------
void appOpSearchProsthesis::UpdateGui()
{
	if (m_Gui)
	{
		m_Gui->Update();
	}
}

//----------------------------------------------------------------------------
void appOpSearchProsthesis::Find()
{
	albaString type = "";
	albaString side = "";

	std::vector<albaProDBProsthesis *> DBprosthesis = m_DBManager->SearchProstheses(m_SearchString, type, side);

	m_ResultsListBox->Clear();

	for (int i = 0; i < DBprosthesis.size(); i++)
	{
		wxString pros = DBprosthesis[i]->GetProducer();
		pros += " - " + DBprosthesis[i]->GetName();

		m_ResultsListBox->Append(pros);
	}

	UpdateGui();
}