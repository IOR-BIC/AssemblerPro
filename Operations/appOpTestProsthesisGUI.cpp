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
#include "appGUIDialogMatrix.h"
#include "appGUIDialogProsthesis.h"
#include "appGUIDialogProsthesisSelection.h"
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

#include "wx\defs.h"
#include "wx\image.h"
#include "wx\sizer.h"
#include "wx\window.h"
#include <vector>

//----------------------------------------------------------------------------
albaCxxTypeMacro(appOpTestProsthesisGUI);

//----------------------------------------------------------------------------
appOpTestProsthesisGUI::appOpTestProsthesisGUI(wxString label) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;

	m_ContentGui = NULL;
	m_GroupGui = NULL;

	m_DBManager = NULL;

	m_ComponentGroupList.clear();

	m_ProsthesisName = "";
	m_SelectedProsthesis = -1;
}
//----------------------------------------------------------------------------
appOpTestProsthesisGUI::~appOpTestProsthesisGUI()
{
	m_ComponentGroupList.clear();
}

//----------------------------------------------------------------------------
bool appOpTestProsthesisGUI::InternalAccept(albaVME *node)
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

	SelectProsthesis();
}

/// Load/Save Data
//----------------------------------------------------------------------------
void appOpTestProsthesisGUI::LoadInfo()
{
	m_DBManager = GetLogicManager()->GetProsthesesDBManager();
	std::vector<albaProDBProsthesis *> DBprosthesis = m_DBManager->GetProstheses();

	if (DBprosthesis.size() > 0)
		m_SelectedProsthesis = 0;
}
//----------------------------------------------------------------------------
void appOpTestProsthesisGUI::SaveInfo()
{
	// TODO
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
		m_SelectedGroup = GetGroup(e);

		if (m_SelectedGroup >= 0)
			OnEventFromGroup(e);
		else
		{
			switch (e->GetId())
			{
			// Prosthesis Events
			case ID_PROSTHESIS_SELECTION: SelectProsthesis(); break;
			case ID_PROSTHESIS_CHANGE: ChangeProsthesis(); break;
			case ID_PROSTHESIS_EDIT: EditProsthesis(); break;
			case ID_GROUP_CREATE: AddNewGroup(); break;

			// Op Events
			case wxOK: OpStop(OP_RUN_OK); break;
			case wxCANCEL: OpStop(OP_RUN_CANCEL); break;

			default: Superclass::OnEvent(alba_event); break;
			}
		}
	}
}
//----------------------------------------------------------------------------
void appOpTestProsthesisGUI::OnEventFromGroup(albaEventBase *alba_event)
{
	switch (alba_event->GetId())
	{
	case ID_GROUP_SHOW: m_ComponentGroupList[m_SelectedGroup]->ShowGroup(); break;
	case ID_GROUP_NAME: EditGroupName(); break;
	case ID_GROUP_REMOVE: DelGroup(); break;

	case ID_COMPONENT_SELECT: break;
	case ID_COMPONENT_ADD: m_ComponentGroupList[m_SelectedGroup]->AddNewComponent(); break;
	case ID_COMPONENT_DEL: m_ComponentGroupList[m_SelectedGroup]->DelComponent(); break;
	case ID_COMPONENT_EDIT: m_ComponentGroupList[m_SelectedGroup]->EditComponent(); break;
	case ID_COMPONENT_MATRIX: m_ComponentGroupList[m_SelectedGroup]->EditMatrix(); break;
	}
}

//----------------------------------------------------------------------------
void appOpTestProsthesisGUI::CreateGui()
{
	m_Gui = new appGUI(this);

	m_ProsthesisComboBox = m_Gui->Combo(ID_PROSTHESIS_SELECTION, "", &m_SelectedProsthesis);

// 	m_Gui->String(ID_PROSTHESIS_NAME, "", &m_ProsthesisName);
// 	m_Gui->Enable(ID_PROSTHESIS_NAME, false);

	m_Gui->TwoButtons(ID_PROSTHESIS_CHANGE, ID_PROSTHESIS_EDIT, "Change", "Edit");
	m_Gui->Divider(1);

	//////////////////////////////////////////////////////////////////////////

	m_ContentGui = new appGUI(this);
	m_GroupGui = new appGUI(this);

	m_ContentGui->Add(m_GroupGui);
	m_Gui->Add(m_ContentGui);

	m_Gui->Divider(0);
	m_Gui->Button(ID_GROUP_CREATE, "New Component Group");

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
		// Update Prosthesis Name List
		std::vector<albaProDBProsthesis *> DBprosthesis = m_DBManager->GetProstheses();

		m_ProsthesisNameList.clear();
		m_ProsthesisComboBox->Clear();
		for (int m = 0; m < DBprosthesis.size(); m++)
		{
			wxString name = wxString::Format("%s - %s", DBprosthesis[m]->GetProducer().GetCStr(), DBprosthesis[m]->GetName().GetCStr());
			m_ProsthesisNameList.push_back(name);
			m_ProsthesisComboBox->Append(name);
		}
		m_ProsthesisComboBox->Select(m_SelectedProsthesis);

		//
		m_GroupGui->FitGui();
		m_ContentGui->FitGui();
		m_Gui->FitGui();

		m_Gui->Update();
	}
}

//----------------------------------------------------------------------------
void appOpTestProsthesisGUI::SelectProsthesis()
{
	std::vector<albaProDBProsthesis *> DBprosthesis = m_DBManager->GetProstheses();

	m_ProsthesisName = DBprosthesis[m_SelectedProsthesis]->GetName();

	LoadGroups();
}
//----------------------------------------------------------------------------
void appOpTestProsthesisGUI::ChangeProsthesis()
{
	appGUIDialogProsthesisSelection pd(_("Select Prosthesis"));
	std::vector<albaProDBProsthesis *> DBprosthesis = m_DBManager->GetProstheses();
	pd.SetProducer(DBprosthesis[m_SelectedProsthesis]->GetProducer());
	pd.Show();

	if (pd.OkClosed())
	{
		std::vector<albaProDBProsthesis *> DBprosthesis = m_DBManager->GetProstheses();

		albaProDBProsthesis *prosthesis = pd.GetProsthesis();

		for (int m = 0; m < DBprosthesis.size(); m++)
		{
			if (DBprosthesis[m] == prosthesis)
			{
				m_SelectedProsthesis = m;
				SelectProsthesis();
			}
		}
	}
}
//----------------------------------------------------------------------------
void appOpTestProsthesisGUI::EditProsthesis()
{
	std::vector<albaProDBProsthesis *> DBprosthesis = m_DBManager->GetProstheses();
	albaProDBProsthesis *selectedProsthesis = DBprosthesis[m_SelectedProsthesis];

	// Show dialog
	appGUIDialogProsthesis md(_("Edit Prosthesis"));
	md.SetProsthesis(selectedProsthesis);
	md.ShowModal();

	if (md.OkClosed())
	{
		// Save DB
		m_DBManager->SaveDB();
 	}
	else
	{
		// Reload DB
		m_DBManager->LoadDB();
	}
}

//----------------------------------------------------------------------------
int appOpTestProsthesisGUI::GetGroup(albaEvent * e)
{
	for (int c = 0; c < m_ComponentGroupList.size(); c++)
		if (e->GetSender() == m_ComponentGroupList[c]) return c;

	return -1;
}
//----------------------------------------------------------------------------
void appOpTestProsthesisGUI::AddNewGroup()
{
	m_SelectedGroup = m_ComponentGroupList.size();

	ComponentGroupGUI *newGroup = new ComponentGroupGUI();
	newGroup->SetName(wxString::Format("NewComponentGroup(%d)", m_ComponentGroupList.size() + 1));
	m_ComponentGroupList.push_back(newGroup);

	// Add to Gui
	m_GroupGui->Add(newGroup->GetGui());
	newGroup->GetGui()->Update();
	newGroup->SetListener(this);

	std::vector<albaProDBProsthesis *> DBprosthesis = m_DBManager->GetProstheses();
	std::vector<albaProDBCompGroup *> *DBCompGroups = DBprosthesis[m_SelectedProsthesis]->GetCompGroups();
	albaProDBCompGroup *newDBGroup = new albaProDBCompGroup();
	newDBGroup->SetName(newGroup->GetName().c_str());
	DBCompGroups->push_back(newDBGroup);

	UpdateGui();
}
//----------------------------------------------------------------------------
void appOpTestProsthesisGUI::DelGroup()
{
	if (m_ComponentGroupList.size() > 0 && m_SelectedGroup < m_ComponentGroupList.size())
	{
		std::vector<albaProDBProsthesis *> DBprosthesis = m_DBManager->GetProstheses();
		std::vector<albaProDBCompGroup *> *DBCompGroups = DBprosthesis[m_SelectedProsthesis]->GetCompGroups();

		DBCompGroups->erase(DBCompGroups->begin() + m_SelectedGroup);
		m_ComponentGroupList.erase(m_ComponentGroupList.begin() + m_SelectedGroup);
		m_SelectedGroup--;
		
		// Rebuild Gui
		m_GroupGui = NULL;
		m_GroupGui = new appGUI(this);

		for (int c = 0; c < m_ComponentGroupList.size(); c++)
		{
			m_GroupGui->Add(m_ComponentGroupList[c]->GetGui());
		}

		m_ContentGui->DestroyChildren();
		m_ContentGui->Add(m_GroupGui);

		UpdateGui();
	}
}

//----------------------------------------------------------------------------
void appOpTestProsthesisGUI::LoadGroups()
{
	ClearGroups();

	std::vector<albaProDBProsthesis *> DBprosthesis = m_DBManager->GetProstheses();
	std::vector<albaProDBCompGroup *> *DBCompGroups = DBprosthesis[m_SelectedProsthesis]->GetCompGroups();

	for (int c = 0; c < DBCompGroups->size(); c++)
	{
		ComponentGroupGUI *newGroup = new ComponentGroupGUI();
		newGroup->SetName(DBCompGroups->at(c)->GetName().GetCStr());
		m_ComponentGroupList.push_back(newGroup);

		// Add to Gui
		m_GroupGui->Add(newGroup->GetGui());
		newGroup->GetGui()->Update();
		newGroup->SetListener(this);

		newGroup->SetCompGroup(DBCompGroups->at(c));
	}

	UpdateGui();
}
//----------------------------------------------------------------------------
void appOpTestProsthesisGUI::ClearGroups()
{
	// Rebuild Gui
	m_GroupGui = NULL;
	m_GroupGui = new appGUI(this);

	m_ComponentGroupList.clear();
	m_ContentGui->DestroyChildren();
	m_SelectedGroup = -1;

	m_ContentGui->Add(m_GroupGui);

	UpdateGui();
}
//----------------------------------------------------------------------------
void appOpTestProsthesisGUI::EditGroupName()
{
	if (m_ComponentGroupList.size() > 0 && m_SelectedGroup < m_ComponentGroupList.size())
	{
		std::vector<albaProDBProsthesis *> DBprosthesis = m_DBManager->GetProstheses();
		std::vector<albaProDBCompGroup *> *DBCompGroups = DBprosthesis[m_SelectedProsthesis]->GetCompGroups();

		// Update DB GroupName
		DBCompGroups->at(m_SelectedProsthesis)->SetName(m_ComponentGroupList[m_SelectedGroup]->GetName());
	}
}

//----------------------------------------------------------------------------
/// ComponentGroupGUI
//----------------------------------------------------------------------------
ComponentGroupGUI::ComponentGroupGUI()
{
	m_Listener = NULL;

	m_CurrentCompGroup = NULL;
	m_GroupName = "";
	m_ShowGroup = 1;

	m_ComponentGui = NULL;
	m_ComponentsNameList.clear();

	m_CurrentComponent = NULL;
}
//----------------------------------------------------------------------------
ComponentGroupGUI::~ComponentGroupGUI()
{
}

//----------------------------------------------------------------------------
void ComponentGroupGUI::OnEvent(albaEventBase *alba_event)
{
	if (m_Listener)
		m_Listener->OnEvent(&albaEvent(this, alba_event->GetId()));
}

//----------------------------------------------------------------------------
appGUI* ComponentGroupGUI::GetGui()
{
	if (m_ComponentGui == NULL)
	{
		m_ComponentGui = new appGUI(this);

		m_ComponentGui->ComponentButton(appOpTestProsthesisGUI::ID_GROUP_SHOW, appOpTestProsthesisGUI::ID_GROUP_NAME, appOpTestProsthesisGUI::ID_GROUP_REMOVE, &m_ShowGroup, &m_GroupName);
		//m_ComponentGui->Bool(appOpTestProsthesisGUI::ID_GROUP_SHOW, m_GroupName, &m_ShowGroup, 1, "Show/Hide");

		m_ComponentListBox = m_ComponentGui->ListBox(appOpTestProsthesisGUI::ID_COMPONENT_SELECT, "");

		std::vector<int> btnIDs{ appOpTestProsthesisGUI::ID_COMPONENT_ADD, appOpTestProsthesisGUI::ID_COMPONENT_DEL, appOpTestProsthesisGUI::ID_COMPONENT_EDIT };
		std::vector<const char*> btnLabels{ "Add","Del","Edit" };

		m_ComponentGui->MultipleButtons(3, 3, btnIDs, btnLabels);
		m_ComponentGui->Button(appOpTestProsthesisGUI::ID_COMPONENT_MATRIX, "Matrix");
		m_ComponentGui->Divider(1);

		m_ComponentGui->FitGui();
	}
	return m_ComponentGui;
}
//----------------------------------------------------------------------------
void ComponentGroupGUI::GuiUpdate()
{
	if (m_ComponentGui == NULL)
	{
		m_ComponentGui->Enable(appOpTestProsthesisGUI::ID_COMPONENT_DEL, m_ComponentsNameList.size() > 0);
		m_ComponentGui->Enable(appOpTestProsthesisGUI::ID_COMPONENT_EDIT, m_ComponentsNameList.size() > 0);

		m_ComponentGui->Update();
	}
}

//----------------------------------------------------------------------------
void ComponentGroupGUI::SetCompGroup(albaProDBCompGroup *compGroup)
{
	if (compGroup != NULL)
	{
		m_CurrentCompGroup = compGroup;
		m_GroupName = m_CurrentCompGroup->GetName();

		m_ComponentsNameList.clear();
		m_ComponentListBox->Clear();
		for (int c=0; c< m_CurrentCompGroup->GetComponents()->size(); c++)
		{
			albaProDBComponent *comp = (albaProDBComponent*)m_CurrentCompGroup->GetComponents()->at(c);
			m_ComponentsNameList.push_back(comp->GetName().GetCStr());
			m_ComponentListBox->Append(comp->GetName().GetCStr());
		}
		
		int last = m_ComponentsNameList.size() - 1;
		m_ComponentListBox->Select(last);
	}

	GuiUpdate();
}

//----------------------------------------------------------------------------
void ComponentGroupGUI::ShowGroup()
{
	int show = m_ShowGroup;
	GuiUpdate();
}

//----------------------------------------------------------------------------
void ComponentGroupGUI::AddNewComponent()
{
	albaProDBComponent *component = new albaProDBComponent();
	component->SetName(wxString::Format("NewComponent(%d)", m_ComponentsNameList.size() + 1));

	appGUIDialogComponent cd(_("Add Component"));
	cd.SetComponent(component);
	cd.Show();

	if (cd.OkClosed())
	{
		wxString name = component->GetName().GetCStr();
		m_ComponentsNameList.push_back(name);
		m_ComponentListBox->Append(name);

		int last = m_ComponentsNameList.size() - 1;
		m_ComponentListBox->Select(last);
		m_SelectedComponent = last;
	}

	GuiUpdate();

	delete component;	
}
//----------------------------------------------------------------------------
void ComponentGroupGUI::DelComponent()
{
	int index = m_ComponentListBox->GetSelection();

	if (index >= 0 && index < m_ComponentsNameList.size())
	{
		m_SelectedComponent = index;

		m_ComponentsNameList.erase(m_ComponentsNameList.begin() + index);
		m_ComponentListBox->Delete(index);
		m_SelectedComponent--;
		GuiUpdate();
	}
}
//----------------------------------------------------------------------------
void ComponentGroupGUI::EditComponent()
{
	int index = m_ComponentListBox->GetSelection();

	if (index >= 0 && index < m_ComponentsNameList.size())
	{
		m_SelectedComponent = index;

		albaProDBComponent *component = m_CurrentCompGroup->GetComponents()->at(m_SelectedComponent);

		appGUIDialogComponent cd(_("Edit Component"));
		cd.SetComponent(component);
		cd.Show();

		if (cd.OkClosed())
		{
			m_ComponentsNameList[index] = component->GetName();

			m_ComponentListBox->Clear();
			for (int i = 0; i < m_ComponentsNameList.size(); i++)
				m_ComponentListBox->Append(m_ComponentsNameList[i]);

			m_ComponentListBox->Select(index);
		}

		GuiUpdate();

		delete component;
	}
}

//----------------------------------------------------------------------------
void ComponentGroupGUI::EditMatrix()
{
	int index = m_ComponentListBox->GetSelection();
	if (index >= 0 && index < m_ComponentsNameList.size())
	{
		m_SelectedComponent = index;

		albaProDBComponent *component = (albaProDBComponent*)m_CurrentCompGroup->GetComponents()->at(m_SelectedComponent);

		appGUIDialogMatrix cm(_("Edit Matrix"));
		cm.SetComponent(component);
		cm.Show();
	}
}