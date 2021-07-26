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
#include "appGUIDialogModel.h"
#include "appGUIDialogProducer.h"
#include "appLogic.h"
#include "appUtils.h"

#include "albaGUI.h"
#include "albaGUIButton.h"
#include "albaGUIDialog.h"
#include "albaGUILab.h"
#include "albaGUIPicButton.h"
#include "albaGUIValidator.h"
#include "albaProsthesisDBManager.h"
#include "albaServiceClient.h"
#include "albaVME.h"

#include "wx\image.h"
#include "wx\window.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(appOpCreateProsthesis);

//----------------------------------------------------------------------------
appOpCreateProsthesis::appOpCreateProsthesis(wxString label) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;

	m_DBManager = NULL;

	m_ProducerComboBox = NULL;
	m_ModelComboBox = NULL;
	m_ComponentComboBox = NULL;

	m_SelectedProducer = 0;
	m_SelectedModel = 0;
	m_SelectedComponent = 0;
}

//----------------------------------------------------------------------------
appOpCreateProsthesis::~appOpCreateProsthesis()
{
	m_ProsthesisVect.clear();

	m_ProducerNameList.clear();
	m_ModelNameList.clear();
	m_ComponentNameList.clear();
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
	LoadInfo();
	
	if (!m_TestMode)
	{
		CreateGui();
	}
}

/// Load/Save Data
//----------------------------------------------------------------------------
void appOpCreateProsthesis::LoadInfo()
{
	m_DBManager = ((appLogic*)GetLogicManager())->GetProsthesisDBManager();

	//////////////////////////////////////////////////////////////////////////
	wxString dbFilePath = appUtils::GetConfigDirectory().c_str();

	std::vector<albaProDBProshesis *> DBprosthesis = m_DBManager->GetProstheses();
	std::vector<albaProDBProducer *> DBproducers = m_DBManager->GetProducers();

	m_ProsthesisVect.clear();

	for (int p = 0; p < DBproducers.size(); p++)
	{
		Producer producer;
		producer.index = p;
		producer.name = DBproducers[p]->GetName();
		producer.webSite = DBproducers[p]->GetWebSite();
		producer.brandImage = DBproducers[p]->GetImgFileName();
		producer.models.clear();

		for (int m = 0; m < DBprosthesis.size(); m++)
		{
			if (DBprosthesis[m]->GetProducer().Equals(producer.name))
			{
				Model model;
				model.index = m;
				model.name = DBprosthesis[m]->GetName();
				model.type = DBprosthesis[m]->GetType() == "Femoral" ? FEMORAL : ACETABULAR;
				model.image = DBprosthesis[m]->GetImgFileName();
				model.side = DBprosthesis[m]->GetSide();

				model.components.clear();

				for (int c = 0; c < 3; c++)
				{
					Component  component;
					component.name = wxString::Format("%s - Component %d", model.name, (c + 1));

					model.components.push_back(component);
				}

				producer.models.push_back(model);
			}
		}

		m_ProsthesisVect.push_back(producer);
	}

	// GUI
	// Load Producers Info
	m_ProducerNameList.clear();
	m_ProducerNameList.push_back("");

	for (int p = 0; p < m_ProsthesisVect.size(); p++)
	{
		m_ProducerNameList.push_back(m_ProsthesisVect[p].name);
	}

	// Load Models Info
	m_ModelNameList.clear();
	m_ModelNameList.push_back("");

	// Load Components Info
	m_ComponentNameList.clear();
	m_ComponentNameList.push_back("");
}
//----------------------------------------------------------------------------
void appOpCreateProsthesis::SaveInfo()
{
	int pChanges = 0;
	int mChanges = 0;
	int cChanges = 0;

	m_DBManager = ((appLogic*)GetLogicManager())->GetProsthesisDBManager();
	
	std::vector<albaProDBProshesis *> DBprosthesis = m_DBManager->GetProstheses();
	std::vector<albaProDBProducer *> DBproducers = m_DBManager->GetProducers();

	//////////////////////////////////////////////////////////////////////////

	// Update Producers
	for (int p=0; p< m_ProsthesisVect.size(); p++)
	{
		if (m_ProsthesisVect[p].index < 0)
		{
			// Add New Producer
			albaProDBProducer *newProducer = new albaProDBProducer();
			newProducer->SetName(m_ProsthesisVect[p].name);
			newProducer->SetImgFileName(m_ProsthesisVect[p].brandImage);
			newProducer->SetWebSite(m_ProsthesisVect[p].webSite);

			DBproducers.push_back(newProducer);
		}
	}

	// Update Prosthesis
	for (int p = 0; p < m_ProsthesisVect.size(); p++)
	{
		if (m_ProsthesisVect[p].isChanged)
		{
			pChanges++;

			if (m_ProsthesisVect[p].index < 0)
			{
				albaProDBProshesis *newProsthesis = new albaProDBProshesis();
				DBprosthesis.push_back(newProsthesis);

				m_ProsthesisVect[p].index = DBprosthesis.size() - 1;
			}

			if (m_ProsthesisVect[p].index >= 0)
			{
				DBprosthesis[m_ProsthesisVect[p].index]->SetProducer(m_ProsthesisVect[p].name);
				DBprosthesis[m_ProsthesisVect[p].index]->SetImgFileName(m_ProsthesisVect[p].brandImage);
			}
		}

		for (int m = 0; m < m_ProsthesisVect[p].models.size(); m++)
		{
			if (m_ProsthesisVect[p].models[m].isChanged)
			{
				// Save Model changes
				mChanges++;

				DBprosthesis[m_ProsthesisVect[p].index]->SetName(m_ProsthesisVect[p].models[m].name);
				DBprosthesis[m_ProsthesisVect[p].index]->SetImgFileName(m_ProsthesisVect[p].models[m].image);
				DBprosthesis[m_ProsthesisVect[p].index]->SetType(m_ProsthesisVect[p].models[m].type);
				DBprosthesis[m_ProsthesisVect[p].index]->SetSide((albaProDBProshesis::PRO_SIDES)m_ProsthesisVect[p].models[m].side);
			}

			for (int c = 0; c < m_ProsthesisVect[p].models[m].components.size(); c++)
			{
				if (m_ProsthesisVect[p].models[m].components[c].isChanged)
				{
					// Save Component changes
					cChanges++;
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	wxString dbFilePath = appUtils::GetConfigDirectory().c_str();
	dbFilePath += "/SavedDB.xml";

	m_DBManager->SaveDBToFile(dbFilePath);

	wxString message = wxString::Format("Producer: %d changes. \nModels: %d changes. \nComponents: %d changes.", pChanges, mChanges, cChanges);
	wxMessageBox(message);
}

//----------------------------------------------------------------------------
void appOpCreateProsthesis::OpStop(int result)
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
void appOpCreateProsthesis::OpDo()
{
}

//----------------------------------------------------------------------------
void appOpCreateProsthesis::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		//if (e->GetSender() == m_Gui)
		{
			switch (e->GetId())
			{
				// GUI Events
			case wxOK:
				OpStop(OP_RUN_OK);
				break;

			case wxCANCEL:
				OpStop(OP_RUN_CANCEL);
				break;

			case ID_SELECT_PRODUCER:
				SelectProducer();
				break;

			case ID_EDIT_PRODUCER:
				EditProducer();
				break;

			case ID_ADD_PRODUCER:
				AddProducer();
				break;

			case ID_SELECT_MODEL:
				SelectModel();
				break;

			case ID_EDIT_MODEL:
				EditModel();
				break;

			case ID_ADD_MODEL:
				AddModel();
				break;

			case ID_SELECT_COMPONENT:
				SelectComponent();
				break;

			case ID_EDIT_COMPONENT:
				EditComponent();
				break;

			case ID_ADD_COMPONENT:
				AddComponent();
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
void appOpCreateProsthesis::CreateGui()
{
	// Interface:
	m_Gui = new appGUI(this);

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("Producer", true);
	m_ProducerComboBox = m_Gui->Combo(ID_SELECT_PRODUCER, "", &m_SelectedProducer, m_ProducerNameList.size(), &m_ProducerNameList[0], "");
	m_Gui->TwoButtons(ID_EDIT_PRODUCER, ID_ADD_PRODUCER, "Edit", "Add New");
	m_Gui->Divider(1);

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("Model", true);
	m_ModelComboBox = m_Gui->Combo(ID_SELECT_MODEL, "", &m_SelectedModel, m_ModelNameList.size(), &m_ModelNameList[0], "");
	m_Gui->TwoButtons(ID_EDIT_MODEL, ID_ADD_MODEL, "Edit", "Add New");
	
	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("Components", true);
	m_ComponentComboBox = m_Gui->Combo(ID_SELECT_COMPONENT, "", &m_SelectedComponent, m_ComponentNameList.size(), &m_ComponentNameList[0], "");
	m_Gui->TwoButtons(ID_EDIT_COMPONENT, ID_ADD_COMPONENT, "Edit", "Add New");

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();
	m_Gui->Label("");
	
	UpdateGui();
	ShowGui();
}
//----------------------------------------------------------------------------
void appOpCreateProsthesis::UpdateGui()
{
	if (m_Gui)
	{
		// Enable Producer Gui
		m_Gui->Enable(ID_EDIT_PRODUCER, m_SelectedProducer > 0);

		// Enable Model Gui
		for (int i = ID_MODEL_GROUP_START; i < ID_MODEL_GROUP_END; i++)
		{
			bool enable = m_SelectedProducer > 0;

			if(i==ID_EDIT_MODEL) 
				enable = enable && m_SelectedModel > 0;

			m_Gui->Enable(i, enable);
		}

		// Enable Component Gui
		for (int i = ID_COMPONENT_GROUP_START; i < ID_COMPONENT_GROUP_END; i++)
		{
			bool enable = m_SelectedModel > 0;

			if (i == ID_EDIT_COMPONENT)
				enable = enable && m_SelectedComponent > 0;

			m_Gui->Enable(i, enable);
		}

		m_Gui->Update();
	}
}

//////////////////////////////////////////////////////////////////////////
/// Prosthesis Management

// Producer
//----------------------------------------------------------------------------
void appOpCreateProsthesis::SelectProducer()
{
	m_SelectedModel = 0; // Reset Model Selection
	m_SelectedComponent = 0; // Reset Component Selection

	m_ModelNameList.clear();
	m_ModelNameList.push_back("");

	m_ModelComboBox->Clear();
	m_ModelComboBox->Append("");

	if (m_SelectedProducer > 0)
		for (int m = 0; m < m_ProsthesisVect[m_SelectedProducer - 1].models.size(); m++)
		{
			m_ModelNameList.push_back(m_ProsthesisVect[m_SelectedProducer - 1].models[m].name);
			m_ModelComboBox->Append(m_ProsthesisVect[m_SelectedProducer - 1].models[m].name);
		}

	UpdateGui();
}
//----------------------------------------------------------------------------
void appOpCreateProsthesis::AddProducer()
{
	// Create New Producer
	m_CurrentProducer.name = wxString::Format("new Producer %d", m_ProducerNameList.size());
	m_CurrentProducer.webSite = "www." + m_CurrentProducer.name + ".com";
	m_CurrentProducer.brandImage = "";

	// Add to Combobox
	m_ProducerNameList.push_back(m_CurrentProducer.name);
	m_ProducerComboBox->Append(m_CurrentProducer.name);
	
	// Add to Vector
	Producer producer;
	producer.index = -1;
	producer.name = m_CurrentProducer.name;
	producer.webSite = m_CurrentProducer.webSite;
	producer.brandImage = m_CurrentProducer.brandImage;
	m_ProsthesisVect.push_back(producer);

	// Select
	m_SelectedProducer = m_ProducerNameList.size() - 1;
	m_ProducerComboBox->Select(m_SelectedProducer);

	SelectProducer();

	EditProducer();
}
//----------------------------------------------------------------------------
void appOpCreateProsthesis::EditProducer()
{
	m_CurrentProducer.index = m_ProsthesisVect[m_SelectedProducer - 1].index;
	m_CurrentProducer.name = m_ProsthesisVect[m_SelectedProducer - 1].name;
	m_CurrentProducer.webSite = m_ProsthesisVect[m_SelectedProducer - 1].webSite;
	m_CurrentProducer.brandImage = m_ProsthesisVect[m_SelectedProducer - 1].brandImage;
	
	appGUIDialogProducer pd(_("Edit Producer"));
	pd.SetProducer(m_CurrentProducer);
	pd.Show();

	UpdateProducer(pd.GetProducer());
}
//----------------------------------------------------------------------------
void appOpCreateProsthesis::UpdateProducer(Producer producer)
{
	//////////////////////////////////////////////////////////////////////////

	// Update Combobox
	m_ProducerNameList[m_SelectedProducer] = producer.name;
	m_ProducerComboBox->SetString(m_SelectedProducer, producer.name);

	// Update Vector Element
	m_ProsthesisVect[m_SelectedProducer - 1].name = producer.name;
	m_ProsthesisVect[m_SelectedProducer - 1].webSite = producer.webSite;
	m_ProsthesisVect[m_SelectedProducer - 1].brandImage = producer.brandImage;
	m_ProsthesisVect[m_SelectedProducer - 1].isChanged = true;

	UpdateGui();
}

// Model
//----------------------------------------------------------------------------
void appOpCreateProsthesis::SelectModel()
{
	m_SelectedComponent = 0; // Reset Component Selection
	
	m_ComponentNameList.clear();
	m_ComponentNameList.push_back("");

	m_ComponentComboBox->Clear();
	m_ComponentComboBox->Append("");

	if (m_SelectedProducer > 0 && m_SelectedModel > 0)
		for (int c = 0; c < m_ProsthesisVect[m_SelectedProducer - 1].models[m_SelectedModel - 1].components.size(); c++)
		{
			m_ComponentNameList.push_back(m_ProsthesisVect[m_SelectedProducer - 1].models[m_SelectedModel - 1].components[c].name);
			m_ComponentComboBox->Append(m_ProsthesisVect[m_SelectedProducer - 1].models[m_SelectedModel - 1].components[c].name);
		}

	UpdateGui();
}
//----------------------------------------------------------------------------
void appOpCreateProsthesis::AddModel()
{
	// Create New Model
	m_CurrentModel.name = wxString::Format("%s - new Model %d", m_ProducerNameList[m_SelectedProducer], m_ModelNameList.size());
	m_CurrentModel.image = "";

	// Add to Combobox
	m_ModelNameList.push_back(m_CurrentModel.name);
	m_ModelComboBox->Append(m_CurrentModel.name);

	// Add to Vector
	Model model;
	model.name = m_CurrentModel.name;
	model.image = m_CurrentModel.name;
	m_ProsthesisVect[m_SelectedProducer - 1].models.push_back(model);

	// Select
	m_SelectedModel = m_ModelNameList.size() - 1;
	m_ModelComboBox->Select(m_SelectedModel);

	SelectModel();

	EditModel();
}
//----------------------------------------------------------------------------
void appOpCreateProsthesis::EditModel()
{
	m_CurrentModel.index = m_ProsthesisVect[m_SelectedProducer - 1].models[m_SelectedModel - 1].index;
	m_CurrentModel.name = m_ProsthesisVect[m_SelectedProducer - 1].models[m_SelectedModel - 1].name;
	m_CurrentModel.image = m_ProsthesisVect[m_SelectedProducer - 1].models[m_SelectedModel - 1].image;
	m_CurrentModel.type = m_ProsthesisVect[m_SelectedProducer - 1].models[m_SelectedModel - 1].type;
	m_CurrentModel.side = m_ProsthesisVect[m_SelectedProducer - 1].models[m_SelectedModel - 1].side;

	appGUIDialogModel md(_("Edit Model"));
	md.SetModel(m_CurrentModel);
	md.ShowModal();

	UpdateModel(md.GetModel());
}
//----------------------------------------------------------------------------
void appOpCreateProsthesis::UpdateModel(Model model)
{
	//////////////////////////////////////////////////////////////////////////

	// Update Combobox
	m_ModelNameList[m_SelectedModel] = model.name;
	m_ModelComboBox->SetString(m_SelectedModel, model.name);

	// Update Vector Element
	m_ProsthesisVect[m_SelectedProducer - 1].models[m_SelectedModel - 1].name = model.name;
	m_ProsthesisVect[m_SelectedProducer - 1].models[m_SelectedModel - 1].image = model.image;
	m_ProsthesisVect[m_SelectedProducer - 1].models[m_SelectedModel - 1].type = model.type;
	m_ProsthesisVect[m_SelectedProducer - 1].models[m_SelectedModel - 1].side = model.side;
	m_ProsthesisVect[m_SelectedProducer - 1].models[m_SelectedModel - 1].isChanged = true;

	UpdateGui();
}

// Component
//----------------------------------------------------------------------------
void appOpCreateProsthesis::SelectComponent()
{
	// TODO Load Component Info
	
	UpdateGui();
}
//----------------------------------------------------------------------------
void appOpCreateProsthesis::AddComponent()
{
	// Create New Component
	m_CurrentComponent.name = wxString::Format("%s - new Component %d", m_ModelNameList[m_SelectedModel], m_ComponentNameList.size());

	// Add to Combobox
	m_ComponentNameList.push_back(m_CurrentComponent.name);
	m_ComponentComboBox->Append(m_CurrentComponent.name);

	// Add to Vector
	Component component;
	component.name = m_CurrentComponent.name;

	m_ProsthesisVect[m_SelectedProducer - 1].models[m_SelectedModel - 1].components.push_back(component);

	// Select
	m_SelectedComponent = m_ComponentNameList.size() - 1;
	m_ComponentComboBox->Select(m_SelectedComponent);

	SelectComponent();

	EditComponent();
}
//----------------------------------------------------------------------------
void appOpCreateProsthesis::EditComponent()
{
	m_CurrentComponent.name = m_ProsthesisVect[m_SelectedProducer - 1].models[m_SelectedModel - 1].components[m_SelectedComponent-1].name;

	appGUIDialogComponent cd(_("Edit Component"));
	cd.SetComponent(&m_CurrentComponent);
	cd.ShowModal();

	UpdateComponent(cd.GetComponent());
}
//----------------------------------------------------------------------------
void appOpCreateProsthesis::UpdateComponent(Component component)
{
	//////////////////////////////////////////////////////////////////////////

	// Update Combobox
	m_ComponentNameList[m_SelectedComponent] = component.name;;
	m_ComponentComboBox->SetString(m_SelectedComponent, component.name);

	// Update Vector Element
	m_ProsthesisVect[m_SelectedProducer - 1].models[m_SelectedModel - 1].components[m_SelectedComponent - 1].name = component.name;;
	m_ProsthesisVect[m_SelectedProducer - 1].models[m_SelectedModel - 1].components[m_SelectedComponent - 1].isChanged = true;

	UpdateGui();
}
