/*=========================================================================
Program:   AssemblerPro
Module:    appVMEProsthesisEdit.cpp
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Gianluigi Crimi, Nicola Vanella
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
#include "appGUI.h"
#include "appVMEProsthesisEdit.h"
#include "appGUIDialogComponent.h"
#include "appGUIDialogMatrix.h"
#include "appGUIDialogProsthesis.h"
#include "albaGUITransformMouseFloatVME.h"
#include "albaInteractorCompositorMouseFloatVME.h"
#include "albaInteractorGenericMouseFloatVME.h"

#include "albaGUI.h"
#include "albaProsthesesDBManager.h"
#include "albaTransform.h"
#include "albaVMEOutput.h"
#include "albaVMEProsthesis.h"
#include "albaVMERefSys.h"
#include "albaVMESurface.h"

#include "mmaMaterial.h"
#include "vtkAppendPolyData.h"
#include "vtkLookupTable.h"
#include "vtkPolyData.h"
#include "vtkSource.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "albaEventBroadcaster.h"

//-------------------------------------------------------------------------
albaCxxTypeMacro(appVMEProsthesisEdit)

//-------------------------------------------------------------------------
appVMEProsthesisEdit::appVMEProsthesisEdit()
{
	m_DBManager = NULL;
	
	m_ComponentRefSys = NULL;

	m_ProsthesisComboBox = NULL;
	m_SelectedProsthesis = 0;

	m_TransformMode = false;

	m_Position[0] = m_Position[1] = m_Position[2] = 0;
	m_Orientation[0] = m_Orientation[1] = m_Orientation[2] = 0;

	m_ComponentsTransformMouse = NULL;
	m_InteractorComponentsGenericMouse = NULL;
}
//-------------------------------------------------------------------------
appVMEProsthesisEdit::~appVMEProsthesisEdit()
{
// 	for (int i = 0; i < m_ComponentsTransform.size(); i++)
// 		cppDEL(m_ComponentsTransform[i]);
// 	m_ComponentsTransform.clear();

	cppDEL(m_ComponentsTransformMouse);
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

/// GUI
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

		m_Gui->Label("");
		m_Gui->Divider(1);
		m_Gui->Label("Transform", true);
		m_Gui->Vector(ID_TRA_TRANSFORM, "Position", m_Position);
		m_Gui->Vector(ID_ROT_TRANSFORM, "Orientation", m_Orientation);
		m_Gui->Button(ID_RESET_TRANSFORM, "Reset");
		m_Gui->HintBox(NULL, "Mouse Right Button = Rotate\nMouse Mid Button = Move\nMouse Right + CTRL = Front Rotate");
		m_Gui->Label("");
		m_Gui->Divider();
		m_Gui->FitGui();
	}

	m_Gui->Enable(ID_TRA_TRANSFORM, false);
	m_Gui->Enable(ID_ROT_TRANSFORM, false);
	m_Gui->Enable(ID_RESET_TRANSFORM, false);

	return m_Gui;
}
//-------------------------------------------------------------------------
void appVMEProsthesisEdit::UpdateGui()
{
	if (m_Gui)
	{
		// Fill Prosthesis Name List
		if (m_ProsthesisComboBox)
		{
			std::vector<albaProDBProsthesis *> DBprosthesis = m_DBManager->GetProstheses();

			m_ProsthesisComboBox->Clear();
			for (int m = 0; m < DBprosthesis.size(); m++)
			{
				wxString name = wxString::Format("%s | %s", DBprosthesis[m]->GetProducer().GetCStr(), DBprosthesis[m]->GetName().GetCStr());
				m_ProsthesisComboBox->Append(name);
			}

			if (DBprosthesis.size() > 0)
				m_ProsthesisComboBox->Select(m_SelectedProsthesis);
		}

		// Update
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
void appVMEProsthesisEdit::EnableGui(bool enable)
{
	if (m_Gui)
	{
		m_Gui->Enable(ID_PROSTHESIS_SELECTION, enable);
		m_Gui->Enable(ID_PROSTHESIS_CHANGE, enable);
		m_Gui->Enable(ID_PROSTHESIS_EDIT, enable);

		for (int g = 0; g < m_Prosthesis->GetCompGroups()->size(); g++)
		{
			EnableComponentGui(g, enable);
		}

		m_Gui->Enable(ID_GROUP_CREATE, enable);

		m_Gui->Enable(ID_TRA_TRANSFORM, enable);
		m_Gui->Enable(ID_ROT_TRANSFORM, enable);
		m_Gui->Enable(ID_RESET_TRANSFORM, enable);
	}
}
//-------------------------------------------------------------------------
void appVMEProsthesisEdit::EnableComponentGui(int compGroup, bool enable)
{
	if (compGroup >= 0 && compGroup < m_Prosthesis->GetCompGroups()->size())
	{
		for (int e = ID_ADD_COMPONENT; e < ID_LAST_COMP_ID; e++)
		{
			int baseID = ID_LAST + (compGroup*ID_LAST_COMP_ID) + e;
			m_ComponentGui[compGroup]->Enable(baseID, enable);
		}
	}
}

/// Events
//-------------------------------------------------------------------------
void appVMEProsthesisEdit::OnEvent(albaEventBase *alba_event)
{
	// events to be sent up or down in the tree are simply forwarded
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{

		if (e->GetSender() == m_ComponentsTransformMouse)
		{
			if (e->GetId() == ID_TRANSFORM)
			{
				if (e->GetArg() == albaInteractorGenericMouse::MOUSE_DOWN)
					PrepareUndoOp();
				else if (e->GetArg() == albaInteractorGenericMouse::MOUSE_MOVE)
					PostMultiplyEventMatrix(e);
				else //albaInteractorGenericMouse::MOUSE_UP
					CompleteAndPushUndoOp();
			}
			else
				Superclass::OnEvent(e);
		}

		albaID eventId = e->GetId();
		int compNum = m_ComponentGui.size();

		switch (e->GetId())
		{
		case ID_START: break;

		case ID_PROSTHESIS_SELECTION: 
			SelectProsthesis(); 
			break;		
		case ID_PROSTHESIS_CHANGE:
			ChangeProsthesis();
			break;
		case ID_PROSTHESIS_EDIT: 
			EditProsthesis(m_Prosthesis);
			m_DBManager->SaveDB();
			break;
		case ID_GROUP_CREATE: 
			CreateNewComponentGroup();
			m_DBManager->SaveDB();
			break;

		case ID_TRA_TRANSFORM: 
		case ID_ROT_TRANSFORM:
			TransformFromGUI(); 
			m_DBManager->SaveDB();
			break;
		case ID_RESET_TRANSFORM: 
			ResetTransform();
			break;

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
	case ID_REM_COMPONENT_GROUP: DeleteComponentGroup(compGroup); break;

	case ID_SHOW_COMPONENT: 
		ShowComponent(compGroup); 
		break;
	case ID_SELECT_COMPONENT: 
		SelectComponent(compGroup);
		break;
	case ID_NAME_COMPONENT:
		RenameComponentGroup(compGroup);			
		m_DBManager->SaveDB();
		break;
	case ID_ADD_COMPONENT:
		AddNewComponent(compGroup);
		m_DBManager->SaveDB();
		break;
	case ID_EDIT_COMPONENT:
		EditComponent(compGroup);
		m_DBManager->SaveDB();
		break;
	case ID_REM_COMPONENT: 
		RemoveComponent(compGroup); 
		m_DBManager->SaveDB();
		break;
	case ID_TRANSFORM_COMPONENT: 
		TransformComponent(compGroup);
		m_DBManager->SaveDB();
		break;
	case ID_MATRIX_COMPONENT:
		EditComponentMatrix(compGroup); 
		m_DBManager->SaveDB();
		break;

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

	GetLogicManager()->VmeVisualModeChanged(this);
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

	compGui->ComponentButton(baseID + ID_SHOW_COMPONENT, baseID + ID_NAME_COMPONENT, baseID + ID_REM_COMPONENT_GROUP, &m_ShowComponents[currGroup], &m_NameComponents[currGroup]);

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

	compGui->TwoButtons(baseID + ID_TRANSFORM_COMPONENT, baseID + ID_MATRIX_COMPONENT, "Transform", "Matrix");
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
	albaProDBCompGroup *group = m_Prosthesis->GetCompGroups()->at(compGroup);

	if (group)
	{
		group->SetName(m_NameComponents[compGroup]);
		UpdateGui();
	}
}
//----------------------------------------------------------------------------
void appVMEProsthesisEdit::DeleteComponentGroup(int compGroup)
{
	std::vector<albaProDBCompGroup *> *compGroups = m_Prosthesis->GetCompGroups();
	albaProDBCompGroup *group = compGroups->at(compGroup);

	if (group)
	{
		for (int c = 0; c < group->GetComponents()->size(); c++)
		{
			RemoveComponent(c);
		}

		compGroups->erase(compGroups->begin() + compGroup);

		//
		ClearComponentGroups();

		for (int g = 0; g < compGroups->size(); g++)
		{
			AddComponentGroup(compGroups->at(g));
		}

		UpdateGui();
		FitParentGui();
		GetLogicManager()->CameraUpdate();
	}
}

/// Components
//----------------------------------------------------------------------------
void appVMEProsthesisEdit::AddNewComponent(int compGroup)
{
	std::vector<albaProDBCompGroup *> * compGroups = m_Prosthesis->GetCompGroups();

	if (compGroups->size() > 0)
	{
		std::vector<albaProDBComponent *> *components = compGroups->at(compGroup)->GetComponents();
		int compId = m_ComponentListBox[compGroup]->GetSelection();

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

		//delete newComponent;
	}
}
//----------------------------------------------------------------------------
void appVMEProsthesisEdit::RemoveComponent(int compGroup)
{
	std::vector<albaProDBCompGroup *> *compGroups = m_Prosthesis->GetCompGroups();

	if (compGroups->size() > 0)
	{
		std::vector<albaProDBComponent *> *components = compGroups->at(compGroup)->GetComponents();
		int compId = m_ComponentListBox[compGroup]->GetSelection();
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

	if (compGroups->size() > 0)
	{
		std::vector<albaProDBComponent *> *components = compGroups->at(compGroup)->GetComponents();
		int compId = m_ComponentListBox[compGroup]->GetSelection();
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
void appVMEProsthesisEdit::TransformComponent(int compGroup)
{
	std::vector<albaProDBCompGroup *> *compGroups = m_Prosthesis->GetCompGroups();

	m_CurrCompGroup = -1;

	if (compGroups->size() > 0)
	{
		// Get RefSys (if exist)
		if (m_ComponentRefSys == NULL)
		{
			m_ComponentRefSys = (albaVMERefSys*)GetLink("Prosthesis Component RefSys");
		}

		if (m_ComponentRefSys == NULL)
		{
			// Create Component RefSys
			albaNEW(m_ComponentRefSys);
			m_ComponentRefSys->SetName("Component RefSys");
			m_ComponentRefSys->ReparentTo(GetRoot());
			SetMandatoryLink("Prosthesis Component RefSys", m_ComponentRefSys);
			//m_ComponentRefSys->GetTagArray()->SetTag(albaTagItem("VISIBLE_IN_THE_TREE", 0.0));
			//m_ComponentRefSys->Delete();

			m_TransformMode = true;
		}
		else
		{
			// Enable/Disable Transform Mode
			m_TransformMode = !m_TransformMode;
		}

		// Update Component RefSys
		std::vector<albaProDBComponent *> *components = compGroups->at(compGroup)->GetComponents();
		int compId = m_ComponentListBox[compGroup]->GetSelection();

		albaMatrix matrix = components->at(compId)->GetMatrix();
		m_ComponentRefSys->SetAbsMatrix(matrix);
		m_OldComponentMatrix = matrix;

		// Calculate Scale Factor
		double b[6];
		GetOutput()->GetVMELocalBounds(b);
		double diffX = fabs(b[1] - b[0]);
		double diffY = fabs(b[3] - b[2]);
		double diffZ = fabs(b[5] - b[4]);
		double mainDiagonal = sqrt(diffX*diffX + diffY*diffY + diffZ*diffZ);
		m_ComponentRefSys->SetScaleFactor(mainDiagonal / 3.0);
		m_ComponentRefSys->Update();

		m_ComponentRefSys->GetOutput()->GetAbsPose(m_Position, m_Orientation);

		// Update Component Gui
		EnableGui(!m_TransformMode);
		m_ComponentGui[compGroup]->Enable(ID_LAST + (compGroup*ID_LAST_COMP_ID) + ID_TRANSFORM_COMPONENT, true);
		m_Gui->Enable(ID_TRA_TRANSFORM, m_TransformMode);
		m_Gui->Enable(ID_ROT_TRANSFORM, m_TransformMode);
		m_Gui->Enable(ID_RESET_TRANSFORM, m_TransformMode);

		// Show/Hide Component RefSys
		GetLogicManager()->VmeShow(m_ComponentRefSys, m_TransformMode);

		if (m_ComponentsTransformMouse == NULL)
		{
			m_ComponentsTransformMouse = new albaGUITransformMouseFloatVME(m_ComponentRefSys, this);
			m_InteractorComponentsGenericMouse = m_ComponentsTransformMouse->CreateBehavior(MOUSE_LEFT_SHIFT);
			m_InteractorComponentsGenericMouse->SetListener(this);
		}
		
		m_InteractorComponentsGenericMouse->SetVME(m_ComponentRefSys);

		if (m_TransformMode)
			m_ComponentsTransformMouse->AttachInteractorToVme();
		else
			m_ComponentsTransformMouse->DetachInteractorFromVme();

		m_CurrCompGroup = compGroup;
	}
}
//----------------------------------------------------------------------------
void appVMEProsthesisEdit::EditComponentMatrix(int compGroup)
{
	std::vector<albaProDBCompGroup *> *compGroups = m_Prosthesis->GetCompGroups();

	if (compGroups->size() > 0)
	{
		std::vector<albaProDBComponent *> *components = compGroups->at(compGroup)->GetComponents();
		int compId = m_ComponentListBox[compGroup]->GetSelection();

		appGUIDialogMatrix cm(_("Edit Matrix"));
		cm.SetComponent(components->at(compId));
		cm.Show();
	}

	m_AppendPolydata->Update();
	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
void appVMEProsthesisEdit::Reset()
{
	ClearComponentGroups();

	vtkDEL(m_AppendPolydata);

	UpdateGui();
}
//----------------------------------------------------------------------------
void appVMEProsthesisEdit::SetSelection(int selection)
{
	vtkDEL(m_AppendPolydata);

	m_SelectedProsthesis = selection;
	SelectProsthesis();
}

//----------------------------------------------------------------------------
void appVMEProsthesisEdit::Resfresh()
{
	SetSelection(m_SelectedProsthesis);
}


//----------------------------------------------------------------------------
void appVMEProsthesisEdit::PrepareUndoOp()
{}
//----------------------------------------------------------------------------
void appVMEProsthesisEdit::CompleteAndPushUndoOp()
{
	m_DBManager->SaveDB();
}
//----------------------------------------------------------------------------
void appVMEProsthesisEdit::PostMultiplyEventMatrix(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		long arg = e->GetArg();

		if (arg == albaInteractorGenericMouse::MOUSE_MOVE)
		{
			// Update Component Matrix
			if (m_CurrCompGroup >= 0 && m_CurrCompGroup < m_Prosthesis->GetCompGroups()->size())
			{
				std::vector<albaProDBComponent *> *components = m_Prosthesis->GetCompGroups()->at(m_CurrCompGroup)->GetComponents();

				int compId = m_ComponentListBox[m_CurrCompGroup]->GetSelection();

				// handle incoming transform events
				vtkTransform *tr = vtkTransform::New();
				tr->PostMultiply();
				tr->SetMatrix(components->at(compId)->GetMatrix().GetVTKMatrix());
				tr->Concatenate(e->GetMatrix()->GetVTKMatrix());
				tr->Update();

				albaMatrix absPose;
				absPose.DeepCopy(tr->GetMatrix());
				absPose.SetTimeStamp(m_CurrentTime);

				components->at(compId)->SetMatrix(absPose);

				m_ComponentRefSys->SetAbsMatrix(components->at(compId)->GetMatrix());
				m_ComponentRefSys->Update();

				SelectComponent(m_CurrCompGroup);

				albaTransform::GetPosition(absPose, m_Position);
				albaTransform::GetOrientation(absPose, m_Orientation);

				m_Gui->Update();

				tr->Delete();
			}
		}
	}
}

//----------------------------------------------------------------------------
void appVMEProsthesisEdit::TransformFromGUI()
{
	if (m_TransformMode && m_CurrCompGroup >= 0 && m_CurrCompGroup < m_Prosthesis->GetCompGroups()->size())
	{
		vtkALBASmartPointer<vtkTransform> t;
		t->PostMultiply();
		double aux_pos[3], aux_rot[3];
		double prosthesisPose[3], prosthesisOrientation[3];
		double diff[3];

		std::vector<albaProDBComponent *> *components = m_Prosthesis->GetCompGroups()->at(m_CurrCompGroup)->GetComponents();
		int compId = m_ComponentListBox[m_CurrCompGroup]->GetSelection();
		albaMatrix absPose = components->at(compId)->GetMatrix();

		albaTransform::GetPosition(absPose, prosthesisPose);
		albaTransform::GetOrientation(absPose, prosthesisOrientation);

		m_ComponentRefSys->GetOutput()->GetAbsPose(aux_pos, aux_rot);

		diff[0] = m_Position[0] - prosthesisPose[0];
		diff[1] = m_Position[1] - prosthesisPose[1];
		diff[2] = m_Position[2] - prosthesisPose[2];

		t->Translate(-aux_pos[0], -aux_pos[1], -aux_pos[2]);
		t->RotateY(m_Orientation[1]);
		t->RotateX(m_Orientation[0]);
		t->RotateZ(m_Orientation[2]);
		t->Translate(aux_pos[0] + diff[0], aux_pos[1] + diff[1], aux_pos[2] + diff[2]);
		t->Update();

		//////////////////////////////////////////////////////////////////////////

		//Apply the orientation to the prosthesis 
		albaSmartPointer<albaMatrix> m;
		m->SetVTKMatrix(t->GetMatrix());
		albaEvent *e = new albaEvent();
		e->SetArg(albaInteractorGenericMouse::MOUSE_MOVE);
		e->SetMatrix(m);
		PostMultiplyEventMatrix((albaEventBase*)e);
		albaDEL(e);
	}
}
//----------------------------------------------------------------------------
void appVMEProsthesisEdit::ResetTransform()
{
	// Update Component Matrix
	if (m_TransformMode && m_CurrCompGroup >= 0 && m_CurrCompGroup < m_Prosthesis->GetCompGroups()->size())
	{
		std::vector<albaProDBComponent *> *components = m_Prosthesis->GetCompGroups()->at(m_CurrCompGroup)->GetComponents();

		int compId = m_ComponentListBox[m_CurrCompGroup]->GetSelection();

		components->at(compId)->SetMatrix(m_OldComponentMatrix);

		m_ComponentRefSys->SetAbsMatrix(m_OldComponentMatrix);
		m_ComponentRefSys->Update();

		SelectComponent(m_CurrCompGroup);

		albaTransform::GetPosition(m_OldComponentMatrix, m_Position);
		albaTransform::GetOrientation(m_OldComponentMatrix, m_Orientation);

		m_Gui->Update();
	}
}