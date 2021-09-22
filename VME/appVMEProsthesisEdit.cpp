/*=========================================================================
Program:   AssemblerPro
Module:    appVMEProsthesisEdit.cpp
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Gianluigi Crimi
==========================================================================
Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "appDecl.h"
#include "appVMEProsthesisEdit.h"

#include "albaGUI.h"
#include "albaProsthesesDBManager.h"
#include "albaProsthesesDBManager.h"
#include "albaTransform.h"
#include "albaVMEOutput.h"
#include "albaVMEProsthesis.h"
#include "albaVMESurface.h"
#include "appGUIDialogComponent.h"
#include "appGUIDialogMatrix.h"
#include "appGUIDialogProsthesis.h"
#include "mmaMaterial.h"
#include "vtkAppendPolyData.h"
#include "vtkLookupTable.h"
#include "vtkLookupTable.h"
#include "vtkPolyData.h"
#include "vtkSource.h"
#include "vtkTransform.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransformPolyDataFilter.h"

//-------------------------------------------------------------------------
albaCxxTypeMacro(appVMEProsthesisEdit)

//-------------------------------------------------------------------------
appVMEProsthesisEdit::appVMEProsthesisEdit()
{
	m_DBManager = NULL;
	
	m_ProsthesisComboBox = NULL;
	m_SelectedProsthesis = 0;
}
//-------------------------------------------------------------------------
appVMEProsthesisEdit::~appVMEProsthesisEdit()
{
}

//-------------------------------------------------------------------------
char** appVMEProsthesisEdit::GetIcon()
{
#include "albaVMEProsthesis.xpm"
	return albaVMEProsthesis_xpm;
}

//-------------------------------------------------------------------------
int appVMEProsthesisEdit::InternalInitialize()
{
	if (Superclass::InternalInitialize() == ALBA_OK)
	{
		m_DBManager = GetLogicManager()->GetProsthesesDBManager();

		UpdateGui();

		return ALBA_OK;
	}

	return ALBA_ERROR;
}

//-------------------------------------------------------------------------
albaGUI* appVMEProsthesisEdit::CreateGui()
{
	if (m_Gui == NULL)
	{
		m_Gui = new albaGUI(this);

		m_ProsthesisComboBox = m_Gui->Combo(ID_PROSTHESIS_SELECTION, "", &m_SelectedProsthesis);

		m_Gui->TwoButtons(ID_PROSTHESIS_CHANGE, ID_PROSTHESIS_EDIT, "Change", "Edit");
		m_Gui->Divider(1);

		m_ContentGui = new appGUI(this);
		m_GroupGui = new appGUI(this);

		m_ContentGui->Add(m_GroupGui);
		m_Gui->Add(m_ContentGui);

		m_Gui->Divider(0);
		m_Gui->Button(ID_GROUP_CREATE, "New Component Group");

		m_Gui->Divider();
		m_Gui->FitGui();
	}
	return m_Gui;
}
//-------------------------------------------------------------------------
void appVMEProsthesisEdit::UpdateGui()
{
	if (m_Gui)
	{
		std::vector<albaProDBProsthesis *> DBprosthesis = m_DBManager->GetProstheses();

		//
		if (m_ProsthesisComboBox)
		{
			m_ProsthesisComboBox->Clear();
			for (int m = 0; m < DBprosthesis.size(); m++)
			{
				wxString name = wxString::Format("%s | %s", DBprosthesis[m]->GetProducer().GetCStr(), DBprosthesis[m]->GetName().GetCStr());
				m_ProsthesisComboBox->Append(name);
			}

			if (DBprosthesis.size() > 0)
				m_ProsthesisComboBox->Select(m_SelectedProsthesis);
		}

		//
		m_GroupGui->FitGui();
		m_GroupGui->FitInside();
		m_ContentGui->FitGui();
		m_GroupGui->FitInside();

		m_Gui->FitGui();
		m_Gui->FitInside();

		m_Gui->Update();
	}
}

//-------------------------------------------------------------------------
void appVMEProsthesisEdit::OnEvent(albaEventBase *alba_event)
{
	// events to be sent up or down in the tree are simply forwarded
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		albaID eventId = e->GetId();
		int compNum = m_ComponentGui.size();

		switch (e->GetId())
		{
		case ID_START: break;

		case ID_PROSTHESIS_SELECTION: SelectProsthesis(); break;		
		case ID_PROSTHESIS_CHANGE: ChangeProsthesis(); break;
		case ID_PROSTHESIS_EDIT: EditProsthesis(m_Prosthesis); break;

		case ID_GROUP_CREATE: CreateNewComponentGroup(); break;

		default:
			if (eventId >= ID_LAST && eventId < ID_LAST + ID_LAST_COMP_ID*compNum)
			{
				int baseId = eventId - ID_LAST;
				int comp = baseId / ID_LAST_COMP_ID;
				int id = baseId % ID_LAST_COMP_ID;
				OnComponentEvent(comp, id);
			}
			break;
		}
	}
	else
	{
		Superclass::OnEvent(alba_event);
	}
}
//----------------------------------------------------------------------------
void appVMEProsthesisEdit::OnComponentEvent(int compGroup, int id)
{
	Superclass::OnComponentEvent(compGroup, id);

	switch (id)
	{
	case ID_NAME_COMPONENT: RenameComponentGroup(compGroup); break;
	case ID_ADD_COMPONENT: AddNewComponent(compGroup); break;
	case ID_EDIT_COMPONENT: EditComponent(compGroup); break;
	case ID_REM_COMPONENT: RemoveComponent(compGroup); break;
	case ID_MATRIX_COMPONENT: EditComponentMatrix(compGroup); break;

	default:
		break;
	}
}

/// Prosthesis
//----------------------------------------------------------------------------
void appVMEProsthesisEdit::SelectProsthesis()
{
	std::vector<albaProDBProsthesis *> DBprosthesis = m_DBManager->GetProstheses();
	SetProsthesis(DBprosthesis[m_SelectedProsthesis]);
	UpdateGui();
	FitParentGui();
}
//----------------------------------------------------------------------------
void appVMEProsthesisEdit::EditProsthesis(albaProDBProsthesis *prosthesis)
{
	if (prosthesis)
	{
		// Show dialog
		appGUIDialogProsthesis md(_("Edit Prosthesis"));
		md.SetProsthesis(prosthesis);
		md.ShowModal();

		if (md.OkClosed())
		{
			// Save DB
			m_DBManager->SaveDB();
			UpdateGui();
		}
		else
		{
			// Reload DB
			m_DBManager->LoadDB();
		}
	}
}

/// Groups
//----------------------------------------------------------------------------
void appVMEProsthesisEdit::CreateNewComponentGroup()
{
	wxString name = wxString::Format("NewComponentGroup(%d)", m_ComponentGui.size() + 1);

	std::vector<albaProDBProsthesis *> DBprosthesis = m_DBManager->GetProstheses();
	std::vector<albaProDBCompGroup *> *DBCompGroups = DBprosthesis[m_SelectedProsthesis]->GetCompGroups();

	albaProDBCompGroup *gaux = DBCompGroups->at(0);
	albaProDBComponent *caux = gaux->GetComponents()->at(0);

	albaProDBCompGroup *newDBGroup = new albaProDBCompGroup();
	newDBGroup->SetName(name);

	albaProDBComponent *newComponent = new albaProDBComponent();
	newComponent->SetName("New Comp");
	newComponent->SetMatrix(caux->GetMatrix());
	newComponent->SetVTKData(caux->GetVTKData());
	newDBGroup->GetComponents()->push_back(newComponent);

	DBCompGroups->push_back(newDBGroup);

	AddComponentGroup(newDBGroup);

	UpdateGui();
	FitParentGui();
}
//----------------------------------------------------------------------------
void appVMEProsthesisEdit::CreateComponentGui(int currGroup, albaProDBCompGroup * componentGroup)
{
	//GUI Stuffs
	int baseID = ID_LAST + m_ComponentGui.size()*ID_LAST_COMP_ID;

	std::vector<albaProDBComponent *> *components = componentGroup->GetComponents();

	appGUI *compGui = new appGUI(this);

	//show all by default
	m_ShowComponents[currGroup] = true;
	m_NameComponents[currGroup] = componentGroup->GetName();
	//compGui->Bool(baseID + ID_SHOW_COMPONENT, componentGroup->GetName(), &m_ShowComponents[currGroup], 1, "Show/Hide");

	compGui->ComponentButton(baseID + ID_SHOW_COMPONENT, baseID + ID_NAME_COMPONENT, baseID + ID_REM_COMPONENT, &m_ShowComponents[currGroup], &m_NameComponents[currGroup]);

	wxListBox *listBox = compGui->ListBox(baseID + ID_SELECT_COMPONENT, "");

	for (int comp = 0; comp < components->size(); comp++)
	{
		listBox->Append(components->at(comp)->GetName().GetCStr());
	}
	compGui->Divider(1);

	listBox->SetSelection(0);

	m_ComponentListBox.push_back(listBox);
	m_ComponentGui.push_back(compGui);

	std::vector<int> btnIDs{ baseID + ID_ADD_COMPONENT, baseID + ID_REM_COMPONENT, baseID + ID_EDIT_COMPONENT };
	std::vector<const char*> btnLabels{ "Add","Del","Edit" };

	compGui->MultipleButtons(3, 3, btnIDs, btnLabels);
	compGui->Button(baseID + ID_MATRIX_COMPONENT, "Matrix");
	compGui->Divider(1);


	// Add to Gui
	m_GroupGui->Add(compGui);
	compGui->FitGui();
	compGui->Update();

	//UpdateGui();
}
//----------------------------------------------------------------------------
void appVMEProsthesisEdit::RenameComponentGroup(int compGroup)
{
	std::vector<albaProDBCompGroup *> *compGroups = m_Prosthesis->GetCompGroups();
	albaProDBCompGroup *group = compGroups->at(compGroup);

	if (group)
	{
		group->SetName(m_NameComponents[compGroup]);
		UpdateGui();
	}
}

/// Components
//----------------------------------------------------------------------------
void appVMEProsthesisEdit::AddNewComponent(int compGroup)
{
	std::vector<albaProDBCompGroup *> * compGroups = m_Prosthesis->GetCompGroups();
	albaProDBCompGroup *group = compGroups->at(compGroup);

	std::vector<albaProDBComponent *> *components = group->GetComponents();
	int compId = m_ComponentListBox[compGroup]->GetSelection();

	if (compGroups->size() > 0)
	{
		albaProDBComponent *newComponent = new albaProDBComponent();
		newComponent->SetName(wxString::Format("NewComponent(%d)", components->size() + 1));

		appGUIDialogComponent cd(_("Add Component"));
		cd.SetComponent(newComponent);
		cd.Show();

		if (cd.OkClosed())
		{
			wxString name = newComponent->GetName().GetCStr();
			m_ComponentListBox[compGroup]->Append(name);

			components->push_back(newComponent);
		}

		UpdateGui();

		delete newComponent;
	}
}
//----------------------------------------------------------------------------
void appVMEProsthesisEdit::RemoveComponent(int compGroup)
{
	std::vector<albaProDBCompGroup *> *compGroups = m_Prosthesis->GetCompGroups();
	albaProDBCompGroup *group = compGroups->at(compGroup);

	std::vector<albaProDBComponent *> *components = group->GetComponents();
	int compId = m_ComponentListBox[compGroup]->GetSelection();

	if (compGroups->size() > 0)
	{
		albaProDBComponent *component = components->at(compId);

		components->erase(components->begin() + compId);
		m_ComponentListBox[compGroup]->Delete(compId);

		UpdateGui();
	}
}
//----------------------------------------------------------------------------
void appVMEProsthesisEdit::EditComponent(int compGroup)
{
	std::vector<albaProDBCompGroup *> *compGroups = m_Prosthesis->GetCompGroups();
	albaProDBCompGroup *group = compGroups->at(compGroup);

	std::vector<albaProDBComponent *> *components = group->GetComponents();
	int compId = m_ComponentListBox[compGroup]->GetSelection();

	if (compGroups->size() > 0)
	{
		albaProDBComponent *component = components->at(compId);

		appGUIDialogComponent cd(_("Edit Component"));
		cd.SetComponent(component);
		cd.Show();

		if (cd.OkClosed())
		{
			m_ComponentListBox[compGroup]->Clear();
			for (int i = 0; i < components->size(); i++)
				m_ComponentListBox[compGroup]->Append(components->at(i)->GetName().GetCStr());

			m_ComponentListBox[compGroup]->Select(compId);
		}

		UpdateGui();
	}
}
//----------------------------------------------------------------------------
void appVMEProsthesisEdit::EditComponentMatrix(int compGroup)
{
	std::vector<albaProDBCompGroup *> *compGroups = m_Prosthesis->GetCompGroups();
	albaProDBCompGroup *group = compGroups->at(compGroup);

	std::vector<albaProDBComponent *> *components = group->GetComponents();
	int compId = m_ComponentListBox[compGroup]->GetSelection();

	if (compGroups->size() > 0)
	{
		appGUIDialogMatrix cm(_("Edit Matrix"));
		cm.SetComponent(components->at(compId));
		cm.Show();
	}

	m_AppendPolydata->Update();
	GetLogicManager()->CameraUpdate();
}
