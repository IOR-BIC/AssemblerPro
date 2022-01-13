/*=========================================================================
Program:   AssemblerPro
Module:    albaVMEProsthesis.cpp
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
#include "appGUIDialogProsthesisSelection.h"

#include "albaGUI.h"
#include "albaProsthesesDBManager.h"
#include "albaTransform.h"
#include "albaVMEOutput.h"
#include "albaVMEProsthesis.h"
#include "albaVMESurface.h"

#include "mmaMaterial.h"
#include "vtkAppendPolyData.h"
#include "vtkLookupTable.h"
#include "vtkPolyData.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "wx/listctrl.h"


//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEProsthesis)

//-------------------------------------------------------------------------
albaVMEProsthesis::albaVMEProsthesis()
{
	m_AppendPolydata = NULL;
	m_ProsthesisName = "";

	m_ContentGui = NULL;
	m_GroupGui = NULL;
}
//-------------------------------------------------------------------------
albaVMEProsthesis::~albaVMEProsthesis()
{
	vtkDEL(m_AppendPolydata);
}

//-------------------------------------------------------------------------
char** albaVMEProsthesis::GetIcon()
{
#include "albaVMEProsthesis.xpm"
	return albaVMEProsthesis_xpm;
}

//-------------------------------------------------------------------------
int albaVMEProsthesis::InternalInitialize()
{
	if (Superclass::InternalInitialize() == ALBA_OK)
	{
		// force material allocation
		GetMaterial();

		return ALBA_OK;
	}

	return ALBA_ERROR;
}
//-------------------------------------------------------------------------
mmaMaterial *albaVMEProsthesis::GetMaterial()
{
	mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
	if (material == NULL)
	{
		material = mmaMaterial::New();
		SetAttribute("MaterialAttributes", material);
	}
	return material;
}
//-------------------------------------------------------------------------
bool albaVMEProsthesis::IsAnimated()
{
	return false;
}
//-------------------------------------------------------------------------
void albaVMEProsthesis::GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes)
{
	kframes.clear(); // no timestamps
}

//-------------------------------------------------------------------------
albaGUI* albaVMEProsthesis::CreateGui()
{
	if (m_Gui == NULL)
	{
		m_Gui = new albaGUI(this);
		
		m_Gui->String(ID_PROSTHESIS_NAME, "", &m_ProsthesisName);
		m_Gui->Enable(ID_PROSTHESIS_NAME, false);

		m_Gui->Button(ID_PROSTHESIS_CHANGE, "Change Prosthesis");
		m_Gui->Divider(1);

		m_ContentGui = new appGUI(this);
		m_GroupGui = new appGUI(this);

		m_ContentGui->Add(m_GroupGui);
		m_Gui->Add(m_ContentGui);

		m_Gui->Divider();
		m_Gui->FitGui();
	}
	return m_Gui;
}
//-------------------------------------------------------------------------
void albaVMEProsthesis::UpdateGui()
{
	if (m_Gui)
	{
		m_GroupGui->FitGui();
		m_ContentGui->FitGui();
		m_Gui->FitGui();

		m_Gui->FitInside();
		m_Gui->Update();
	}
}
//----------------------------------------------------------------------------
void albaVMEProsthesis::FitParentGui()
{
	albaGUI* parent = (albaGUI*)m_Gui->GetParent();
	if (parent)
	{
		parent->FitGui();
		parent->Update();
	}

	albaGUI* grandParent = (albaGUI*)m_Gui->GetParent();
	if (grandParent)
	{
		grandParent->FitGui();
		grandParent->Update();
	}
}

//----------------------------------------------------------------------------
void albaVMEProsthesis::AddComponentGroup(albaProDBCompGroup *componentGroup)
{
	//VTK Stuffs
	vtkTransformPolyDataFilter *compTraFilter;
	vtkNEW(compTraFilter);

	m_TransformFilters.push_back(compTraFilter);

	vtkTransform *compTra;
	vtkNEW(compTra);
	compTra->PreMultiply();
	m_Transforms.push_back(compTra);

	int currGroup = m_Transforms.size()-1;
	if (currGroup > 1)
	{
		compTra->SetMatrix(m_Transforms[currGroup - 1]->GetMatrix());
	}

	std::vector<albaProDBComponent *> *components = componentGroup->GetComponents();

	if (currGroup >= 0)
	{		
		if (currGroup >= 1)
		{
			albaProDBCompGroup *prevGroup = m_Prosthesis->GetCompGroups()->at(currGroup - 1);
			int prevSelComp = m_ComponentListBox[currGroup - 1]->GetSelection();
			albaProDBComponent *prevComp = prevGroup->GetComponents()->at(prevSelComp);
			
			compTra->Concatenate(prevComp->GetMatrix().GetVTKMatrix());
		}
		albaProDBComponent *currentComp = components->at(0);
		vtkPolyData * compVTKData = currentComp->GetVTKData();
		if (compVTKData)
		{
			compTraFilter->SetInput(compVTKData);
			compVTKData->UnRegister(NULL);
		}
	}
	compTra->Update();

	compTraFilter->SetTransform(compTra);
	m_AppendPolydata->AddInput(compTraFilter->GetOutput());

	CreateComponentGui(currGroup, componentGroup);
}
//----------------------------------------------------------------------------
void albaVMEProsthesis::CreateComponentGui(int currGroup, albaProDBCompGroup * componentGroup)
{
	//GUI Stuffs
	int baseID = ID_LAST + m_ComponentGui.size()*ID_LAST_COMP_ID;

	std::vector<albaProDBComponent *> *components = componentGroup->GetComponents();
	
	albaGUI *compGui = new albaGUI(this);

	//show all by default
	m_ShowComponents[currGroup] = true;
	compGui->Bool(baseID + ID_SHOW_COMPONENT, componentGroup->GetName(), &m_ShowComponents[currGroup], 1, "Show/Hide");
	wxListBox *listBox = compGui->ListBox(baseID + ID_SELECT_COMPONENT, "");

	for (int comp = 0; comp < components->size(); comp++)
	{
		listBox->Append(components->at(comp)->GetName().GetCStr());
	}
	compGui->Divider(1);

	listBox->SetSelection(0);

	m_ComponentListBox.push_back(listBox);
	m_ComponentGui.push_back(compGui);

	// Add to Gui
	m_GroupGui->Add(compGui);
	compGui->FitGui();
	compGui->Update();

	UpdateGui();
}
//----------------------------------------------------------------------------
void albaVMEProsthesis::ClearComponentGroups()
{
	for (int i = 0; i < m_TransformFilters.size(); i++)
	{
		m_AppendPolydata->RemoveInput(m_TransformFilters[i]->GetOutput());
		vtkDEL(m_TransformFilters[i]);
		vtkDEL(m_Transforms[i]);
		//m_ContentGui->Remove(m_ComponentGui[i]);

		delete m_ComponentGui[i];
	}

	m_TransformFilters.clear();
	m_Transforms.clear();
	m_ComponentListBox.clear();
	m_ComponentGui.clear();


	// Rebuild Gui
	m_GroupGui = NULL;
	m_GroupGui = new appGUI(this);

	m_ContentGui->DestroyChildren();
	m_ContentGui->Add(m_GroupGui);

	UpdateGui();
}

//----------------------------------------------------------------------------
void albaVMEProsthesis::SetProsthesis(albaProDBProsthesis *prosthesis)
{
	if (prosthesis)
	{
		CreateGui();

		if (m_AppendPolydata == NULL)
			vtkNEW(m_AppendPolydata);

		// Remove current components
		ClearComponentGroups();

		m_Prosthesis = prosthesis;

		m_ProsthesisName = m_Prosthesis->GetProducer();
		m_ProsthesisName += " | " + m_Prosthesis->GetName();

		std::vector<albaProDBCompGroup *> *componentsVector = prosthesis->GetCompGroups();

		for (int i = 0; i < componentsVector->size(); i++)
		{
			AddComponentGroup(componentsVector->at(i));
		}

		SetData(m_AppendPolydata->GetOutput(), 0, ALBA_VME_REFERENCE_DATA);
			
		UpdateGui();
	}
}
//----------------------------------------------------------------------------
void albaVMEProsthesis::ChangeProsthesis()
{
	appGUIDialogProsthesisSelection pd(_("Select Prosthesis"));
	pd.SetProducer(m_Prosthesis->GetProducer());
	pd.Show();

	if (pd.OkClosed())
	{
		albaProDBProsthesis *prosthesis = pd.GetProsthesis();

		SetProsthesis(prosthesis);
		UpdateGui();
		FitParentGui();

		GetLogicManager()->VmeVisualModeChanged(this);
	}
}

//-------------------------------------------------------------------------
void albaVMEProsthesis::OnEvent(albaEventBase *alba_event)
{
	// events to be sent up or down in the tree are simply forwarded
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		albaID eventId = e->GetId();
		int compNum = m_ComponentGui.size();
		switch (eventId)
		{
		case ID_START: break;
		case ID_PROSTHESIS_CHANGE: ChangeProsthesis(); break;

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
void albaVMEProsthesis::OnComponentEvent(int compGroup, int id)
{
	switch (id)
	{
		case ID_SHOW_COMPONENT:
		{
			if (m_ShowComponents[compGroup])
				m_AppendPolydata->AddInput(m_TransformFilters[compGroup]->GetOutput());
			else
				m_AppendPolydata->RemoveInput(m_TransformFilters[compGroup]->GetOutput());
			m_AppendPolydata->Update();
			GetLogicManager()->CameraUpdate();
		}
		break;
		case ID_SELECT_COMPONENT:
		{
			std::vector<albaProDBCompGroup *> * compGroups = m_Prosthesis->GetCompGroups();
			albaProDBCompGroup *group = compGroups->at(compGroup);
			int groups = compGroups->size();
			std::vector<albaProDBComponent *> *components = group->GetComponents();
			int compId = m_ComponentListBox[compGroup]->GetSelection();

			if (groups > 0)
			{
				//update current VTK data 
				albaProDBComponent * currentComp = components->at(compId);
				m_TransformFilters[compGroup]->SetInput(currentComp->GetVTKData());


				//update current following transforms
				for (int i = compGroup + 1; i < groups; i++)
				{
					vtkTransform *compTra = m_Transforms[i];
					compTra->SetMatrix(m_Transforms[i - 1]->GetMatrix());

					albaProDBComponent * prevComp = currentComp;


					albaProDBCompGroup *currentGroup = compGroups->at(i);
					int prevSelComp = m_ComponentListBox[i]->GetSelection();
					currentComp = currentGroup->GetComponents()->at(prevSelComp);

					compTra->Concatenate(prevComp->GetMatrix().GetVTKMatrix());

					compTra->Update();
				}
			}
			m_AppendPolydata->Update();
			GetLogicManager()->CameraUpdate();
		}
		break;
		default:
			break;
	}
}

//-------------------------------------------------------------------------
albaVMEOutput *albaVMEProsthesis::GetOutput()
{
	// allocate the right type of output on demand
	if (m_Output == NULL)
	{
		SetOutput(albaVMEOutputSurface::New()); // create the output
	}
	return m_Output;
}
