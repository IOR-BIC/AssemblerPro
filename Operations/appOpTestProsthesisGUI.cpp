/*=========================================================================
Program:   AssemblerPro
Module:    appOpTestProsthesisGUI.cpp
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

#include "appOpTestProsthesisGUI.h"
#include "appDecl.h"
#include "appGUI.h"
#include "appGUIDialogComponent.h"
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
#include "appGUIDialogProsthesisSelection.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(appOpTestProsthesisGUI);

//----------------------------------------------------------------------------
appOpTestProsthesisGUI::appOpTestProsthesisGUI(wxString label) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;

	m_DBManager = NULL;

	m_EditMode = true;
	m_ProsthesisName = "Prosthesis";
	m_NumComponentGroups = 0;
}

//----------------------------------------------------------------------------
appOpTestProsthesisGUI::~appOpTestProsthesisGUI()
{
}

//----------------------------------------------------------------------------
bool appOpTestProsthesisGUI::Accept(albaVME *node)
{
	//return node->IsA("...");
	return true;
}

//----------------------------------------------------------------------------
char** appOpTestProsthesisGUI::GetIcon()
{
#include "pic/MENU_OP_PROSTHESIS.xpm"
	return MENU_OP_PROSTHESIS_xpm;
}

//----------------------------------------------------------------------------
albaOp* appOpTestProsthesisGUI::Copy()
{
	appOpTestProsthesisGUI *cp = new appOpTestProsthesisGUI(m_Label);
	return cp;
}
//----------------------------------------------------------------------------
void appOpTestProsthesisGUI::OpRun()
{
	LoadInfo();
	
	if (!m_TestMode)
	{
		CreateGui();
	}
}

/// Load/Save Data
//----------------------------------------------------------------------------
void appOpTestProsthesisGUI::LoadInfo()
{
	m_DBManager = GetLogicManager()->GetProsthesesDBManager();

	//////////////////////////////////////////////////////////////////////////
	wxString dbFilePath = appUtils::GetConfigDirectory().c_str();

	std::vector<albaProDBProshesis *> DBprosthesis = m_DBManager->GetProstheses();
	
	// Load Prostheses Info
	for (int m = 0; m < DBprosthesis.size(); m++)
	{

	}

	//////////////////////////////////////////////////////////////////////////
	// TEST

	if (DBprosthesis.size() > 0) 
	{
		m_ProsthesisName = DBprosthesis[0]->GetName();	
		m_NumComponentGroups = 3;
	}
}
//----------------------------------------------------------------------------
void appOpTestProsthesisGUI::SaveInfo()
{
	
}

//----------------------------------------------------------------------------
void appOpTestProsthesisGUI::OpStop(int result)
{
	if (!m_TestMode)
	{
		HideGui();
	}

	if (result == OP_RUN_OK)
	{
		SaveInfo();
	}

	albaEventMacro(albaEvent(this, result));
}
//----------------------------------------------------------------------------
void appOpTestProsthesisGUI::OpDo()
{
}

//----------------------------------------------------------------------------
void appOpTestProsthesisGUI::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		//if (e->GetSender() == m_Gui)
		{
			switch (e->GetId())
			{
			case ID_PROSTHESIS_CHANGE:
			{
				appGUIDialogProsthesisSelection pd(_("Select Prosthesis"));
				pd.Show();
			}
			break;

			case ID_COMPONENT_SELECT: break;
			case ID_COMPONENT_EDIT:
			{
				Component component;
				component.name = "Component";

				appGUIDialogComponent cd(_("Edit Component"));
				cd.SetComponent(&component);
				cd.Show();
			}
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
void appOpTestProsthesisGUI::CreateGui()
{
	m_Gui = new appGUI(this);

	m_Gui->String(ID_PROSTHESIS_NAME, "", &m_ProsthesisName);
	m_Gui->Enable(ID_PROSTHESIS_NAME, false);

	m_Gui->TwoButtons(ID_PROSTHESIS_CHANGE, ID_PROSTHESIS_EDIT, "Change", "Edit");
	m_Gui->Divider(1);

	//////////////////////////////////////////////////////////////////////////

	std::vector<int> btnIDs{ ID_COMPONENT_ADD,ID_COMPONENT_DEL,ID_COMPONENT_EDIT };
	std::vector<const char*> btnLabels{ "Add","Del","Edit" };

	appGUI *groupGui = new appGUI(this);

	for (int group = 0; group < m_NumComponentGroups; group++)
	{
		appGUI *componentGui = new appGUI(this);

		wxString val = wxString::Format("ComponentGroup%d", group + 1);
		componentGui->Bool(ID_GROUP_SHOW, val, &m_Show, 1, "Show/Hide");
		wxListBox *listBox = componentGui->ListBox(ID_COMPONENT_SELECT, "");

		for (int comp = 0; comp < 3; comp++)
		{
			wxString val = wxString::Format("Component%d", comp + 1);
			listBox->Append(val);
		}

		if (m_EditMode)
		{
			componentGui->MultipleButtons(3, 3, btnIDs, btnLabels);
			componentGui->Button(NULL, "Matrix");
		}

		componentGui->Divider(1);

		componentGui->FitGui();
		groupGui->Add(componentGui);
	}

	groupGui->FitGui();
	m_Gui->Add(groupGui);

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();
	m_Gui->Label("");

	UpdateGui();
	ShowGui();
}
//----------------------------------------------------------------------------
void appOpTestProsthesisGUI::UpdateGui()
{
	if (m_Gui)
	{
		m_Gui->Update();
	}
}