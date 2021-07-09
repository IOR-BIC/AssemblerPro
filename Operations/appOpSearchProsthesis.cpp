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

#include "albaGUI.h"
#include "albaGUICheckListBox.h"
#include "albaVME.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(appOpSearchProsthesis);

//----------------------------------------------------------------------------
appOpSearchProsthesis::appOpSearchProsthesis(wxString label) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;

	m_SearchString = "1";
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

	m_Gui->String(NULL, "Find", &m_SearchString);

	m_Gui->Divider(1);

	albaGUICheckListBox *m_SearchFilterCheckBox = m_Gui->CheckList(NULL, "Filters");
	m_SearchFilterCheckBox->AddItem(NULL, "Producer", false);
	m_SearchFilterCheckBox->AddItem(NULL, "Model", false);
	m_SearchFilterCheckBox->AddItem(NULL, "Component", false);

	m_Gui->Divider(1);

	m_Gui->Label("Results", true);
	wxListBox *m_ResultsListBox = m_Gui->ListBox(NULL, "");
	m_ResultsListBox->Append("Prod 1 - Mod 1");
	m_ResultsListBox->Append("Prod 2 - Mod 1");
	m_ResultsListBox->Append("Prod 2 - Mod 14");
	m_ResultsListBox->Append("Prod 3 - Mod 12");

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();
	m_Gui->Label("");

	ShowGui();
}