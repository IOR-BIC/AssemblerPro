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

#include "albaGUI.h"
#include "albaGUIButton.h"
#include "albaGUIDialog.h"
#include "albaGUILab.h"
#include "albaGUIPicButton.h"
#include "albaVME.h"
#include "appUtils.h"
#include "wx\image.h"
#include "wx\window.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(appOpCreateProsthesis);

//----------------------------------------------------------------------------
appOpCreateProsthesis::appOpCreateProsthesis(wxString label) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;

	m_ProducerComboBox = NULL;
	m_ModelComboBox = NULL;
	m_ComponentComboBox = NULL;

	m_SelectedProducer = 0;
	m_SelectedModel = 0;
	m_SelectedComponent = 0;

	m_ProducerDialog = NULL;
	m_IsProducerDialogOpened = false;
	m_ProducerOkButtonPressed = false;

	m_ModelDialog = NULL;
	m_IsModelDialogOpened = false;
	m_ModelOkButtonPressed = false;

	m_ComponentDialog = NULL;
	m_IsComponentDialogOpened = false;
	m_ComponentOkButtonPressed = false;
}

//----------------------------------------------------------------------------
appOpCreateProsthesis::~appOpCreateProsthesis()
{
	m_ProsthesisVect.clear();

	m_ProducerNameList.clear();
	m_ModelNameList.clear();
	m_ComponentNameList.clear();

	cppDEL(m_ProducerDialog);
	cppDEL(m_ModelDialog);
	cppDEL(m_ComponentDialog);
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
	wxString imagesPath = appUtils::GetConfigDirectory().c_str();

	m_ProsthesisVect.clear();

	for (int p = 0; p < 3; p++)
	{
		Producer producer;
		producer.name = wxString::Format("Producer %d", (p + 1));
		producer.webSite = wxString::Format("www.producer_%d.com", (p + 1));
		producer.brandImage = imagesPath + "/Wizard/Producer.bmp";
		producer.models.clear();

		for (int m = 0; m < 4; m++)
		{
			Model model;
			model.name = wxString::Format("%s - Model %d", producer.name, (m + 1));
			model.image = "";
			model.components.clear();

			for (int c = 0; c < 3; c++)
			{
				Component  component;
				component.name = wxString::Format("%s - Component %d", model.name, (c + 1));

				model.components.push_back(component);
			}

			producer.models.push_back(model);
		}

		m_ProsthesisVect.push_back(producer);
	}
	
	// Load Producers Info
	m_ProducerNameList.clear();
	m_ProducerNameList.push_back("");

	for (int p=0; p<m_ProsthesisVect.size(); p++)
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

	for (int p = 0; p < m_ProsthesisVect.size(); p++)
	{
		if (m_ProsthesisVect[p].isChanged)
		{
			// Save Producer changes
			pChanges++;
		}

		for (int m = 0; m < m_ProsthesisVect[p].models.size(); m++)
		{
			if (m_ProsthesisVect[p].models[m].isChanged)
			{
				// Save Model changes
				mChanges++;
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

				// DIALOG Events
			case ID_PRODUCER_DIALOG_OK_PRESSED:
			{
				HideProducerDialog();
				m_ProducerOkButtonPressed = true;

				UpdateProducer();
			}
			break;

			case ID_PRODUCER_DIALOG_TEXT:
				break;

			case ID_PRODUCER_DIALOG_IMM:
			{
				albaString fileNameFullPath = albaGetDocumentsDirectory().c_str();
				albaString wildc = "Image file (*.bmp)|*.bmp";
				wxString imagePath = albaGetOpenFile(fileNameFullPath.GetCStr(), wildc, "Select file").c_str();

				if (wxFileExists(imagePath))
				{
					m_CurrentProducer.brandImage = imagePath;
					UpdateProducerDialog();
				}
			}
			break;

			case ID_MODEL_DIALOG_OK_PRESSED:
			{
				HideModelDialog();
				m_ModelOkButtonPressed = true;

				UpdateModel();
			}
			break;

			case ID_MODEL_DIALOG_TEXT:
				break;

			case ID_MODEL_DIALOG_IMM:
			{
				albaString fileNameFullPath = albaGetDocumentsDirectory().c_str();
				albaString wildc = "Image file (*.bmp)|*.bmp";
				wxString imagePath = albaGetOpenFile(fileNameFullPath.GetCStr(), wildc, "Select file").c_str();

				if (wxFileExists(imagePath))
				{
					m_CurrentModel.image = imagePath;
					UpdateModelDialog();
				}
			}
			break;

			case ID_COMPONENT_DIALOG_OK_PRESSED:
			{
				HideComponentDialog();
				m_ComponentOkButtonPressed = true;

				UpdateComponent();
			}
			break;

			case ID_COMPONENT_DIALOG_TEXT:
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
	m_CurrentProducer.name = m_ProsthesisVect[m_SelectedProducer - 1].name;
	m_CurrentProducer.webSite = m_ProsthesisVect[m_SelectedProducer - 1].webSite;
	m_CurrentProducer.brandImage = m_ProsthesisVect[m_SelectedProducer - 1].brandImage;

	ShowProducerDialog();
}
//----------------------------------------------------------------------------
void appOpCreateProsthesis::UpdateProducer()
{
	//////////////////////////////////////////////////////////////////////////
	// Update Producer
	wxString newName = m_ProducerName_textCtrl->GetValue();
	wxString newSite = m_ProducerSite_textCtrl->GetValue();

	// Update Combobox
	m_ProducerNameList[m_SelectedProducer] = newName;
	m_ProducerComboBox->SetString(m_SelectedProducer, newName);

	// Update Vector Element
	m_ProsthesisVect[m_SelectedProducer - 1].name = newName;
	m_ProsthesisVect[m_SelectedProducer - 1].webSite = newSite;
	m_ProsthesisVect[m_SelectedProducer - 1].brandImage = m_CurrentProducer.brandImage;
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
	m_CurrentModel.name = m_ProsthesisVect[m_SelectedProducer - 1].models[m_SelectedModel - 1].name;
	m_CurrentModel.image = m_ProsthesisVect[m_SelectedProducer - 1].models[m_SelectedModel - 1].image;

	ShowModelDialog();
}
//----------------------------------------------------------------------------
void appOpCreateProsthesis::UpdateModel()
{
	//////////////////////////////////////////////////////////////////////////
	// Update Model
	wxString newName = m_ModelName_textCtrl->GetValue();

	// Update Combobox
	m_ModelNameList[m_SelectedModel] = newName;
	m_ModelComboBox->SetString(m_SelectedModel, newName);

	// Update Vector Element
	m_ProsthesisVect[m_SelectedProducer - 1].models[m_SelectedModel - 1].name = newName;
	m_ProsthesisVect[m_SelectedProducer - 1].models[m_SelectedModel - 1].image = m_CurrentModel.image;
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

	ShowComponentDialog();
}
//----------------------------------------------------------------------------
void appOpCreateProsthesis::UpdateComponent()
{
	//////////////////////////////////////////////////////////////////////////
	// Update Component
	wxString newName = m_ComponentName_textCtrl->GetValue();

	// Update Combobox
	m_ComponentNameList[m_SelectedComponent] = newName;
	m_ComponentComboBox->SetString(m_SelectedComponent, newName);

	// Update Vector Element
	m_ProsthesisVect[m_SelectedProducer - 1].models[m_SelectedModel - 1].components[m_SelectedComponent - 1].name = newName;
	m_ProsthesisVect[m_SelectedProducer - 1].models[m_SelectedModel - 1].components[m_SelectedComponent - 1].isChanged = true;

	UpdateGui();
}

/// Dialog Producer
//----------------------------------------------------------------------------
void appOpCreateProsthesis::ShowProducerDialog()
{
	wxString title = "Edit Producer";

	//////////////////////////////////////////////////////////////////////////
	if (m_ProducerDialog == NULL)
	{
		m_ProducerDialog = new albaGUIDialog(title, albaCLOSEWINDOW);

		//////////////////////////////////////////////////////////////////////////

		wxBoxSizer *mainBoxSizer = new wxBoxSizer(wxVERTICAL);

		//
		int panelWidth = 400;

		wxString imagesPath = appUtils::GetConfigDirectory().c_str();
		wxString imgPath = imagesPath + "/Wizard/Producer.bmp";

		if (wxFileExists(m_CurrentProducer.brandImage))
			imgPath = m_CurrentProducer.brandImage;

		if (wxFileExists(imgPath))
		{
			//wxImage *previewImage;
			wxBitmap *previewBitmap;

			// Load and show the image
			m_ProducerImage = new wxImage();
			m_ProducerImage->LoadFile(imgPath.c_str(), wxBITMAP_TYPE_ANY);

			previewBitmap = new wxBitmap(*m_ProducerImage);
			m_ProducerImageButton = new albaGUIPicButton(m_ProducerDialog, previewBitmap, -1);

			panelWidth = m_ProducerImage->GetWidth();

			mainBoxSizer->Add(m_ProducerImageButton, 0, wxALL | wxALIGN_CENTER, 0);

			// BUTTON - Change Producer Brand Image
			albaGUIButton *immBtn = new albaGUIButton(m_ProducerDialog, ID_PRODUCER_DIALOG_IMM, "Change Image", wxPoint(-1, -1));
			immBtn->SetListener(this);
			mainBoxSizer->Add(immBtn, 0, wxALIGN_RIGHT, 0);
			
			delete previewBitmap;
		}

		//////////////////////////////////////////////////////////////////////////
		// Info Sizer
		wxBoxSizer *infoBoxSizer = new wxBoxSizer(wxVERTICAL);

		// TEXT - Producer Name
		wxStaticBoxSizer *labelSizer1 = new wxStaticBoxSizer(wxVERTICAL, m_ProducerDialog, "Producer Name");
		m_ProducerName_textCtrl = new wxTextCtrl(m_ProducerDialog, ID_PRODUCER_DIALOG_TEXT, m_CurrentProducer.name, wxPoint(-1, -1), wxSize(panelWidth, 25), wxALL | wxEXPAND);
		m_ProducerName_textCtrl->SetEditable(true);
		m_ProducerName_textCtrl->SetMaxLength(64);
		labelSizer1->Add(m_ProducerName_textCtrl, 0, wxALL | wxEXPAND, 0);
		infoBoxSizer->Add(labelSizer1, 0, wxALL | wxEXPAND, 5);

		// TEXT - Producer Web Site
		wxStaticBoxSizer *labelSizer2 = new wxStaticBoxSizer(wxVERTICAL, m_ProducerDialog, "Web Site");
		m_ProducerSite_textCtrl = new wxTextCtrl(m_ProducerDialog, ID_PRODUCER_DIALOG_TEXT, m_CurrentProducer.webSite, wxPoint(-1, -1), wxSize(panelWidth, 25), wxALL | wxEXPAND);
		m_ProducerSite_textCtrl->SetEditable(true);
		m_ProducerSite_textCtrl->SetMaxLength(64);
		labelSizer2->Add(m_ProducerSite_textCtrl, 0, wxALL | wxEXPAND, 0);
		infoBoxSizer->Add(labelSizer2, 0, wxALL | wxEXPAND, 5);
		
		// TEXT - Empty Separator
		infoBoxSizer->Add(new albaGUILab(m_ProducerDialog, -1, " "), 0, wxALIGN_LEFT, 5);

		mainBoxSizer->Add(infoBoxSizer, 0, wxALL, 5);

		// BUTTON - Ok
		albaGUIButton *okBtn = new albaGUIButton(m_ProducerDialog, ID_PRODUCER_DIALOG_OK_PRESSED, "OK", wxPoint(-1, -1));
		okBtn->SetListener(this);
		mainBoxSizer->Add(okBtn, 0, wxALIGN_RIGHT, 0);

		//////////////////////////////////////////////////////////////////////////
		m_ProducerDialog->Add(mainBoxSizer, 0, wxALL, 5);
		m_ProducerDialog->Fit();

		// Show dialog
		wxSize s = albaGetFrame()->GetSize();
		wxPoint p = albaGetFrame()->GetPosition();
		int posX = p.x + s.GetWidth() * .5 - m_ProducerDialog->GetSize().GetWidth() * .5;
		int posY = p.y + s.GetHeight() * .5 - m_ProducerDialog->GetSize().GetHeight() * .5;

		m_ProducerDialog->SetPosition(wxPoint(posX, posY));
	}
	else
	{
		UpdateProducerDialog();
	}

	if (m_IsProducerDialogOpened)
		HideProducerDialog();

	m_ProducerDialog->ShowModal();

	m_IsProducerDialogOpened = true;
	m_ProducerOkButtonPressed = false;
}
//----------------------------------------------------------------------------
void appOpCreateProsthesis::HideProducerDialog()
{
	if (m_ProducerDialog)
		m_ProducerDialog->Hide();

	m_IsProducerDialogOpened = false;
}
//----------------------------------------------------------------------------
void appOpCreateProsthesis::UpdateProducerDialog()
{
	if (m_ProducerDialog)
	{
		// Update Image
		if (wxFileExists(m_CurrentProducer.brandImage))
		{
			m_ProducerImage->LoadFile(m_CurrentProducer.brandImage.c_str(), wxBITMAP_TYPE_ANY);
			m_ProducerImageButton->SetBitmapSelected(wxBitmap(*m_ProducerImage));
			m_ProducerImageButton->Update();
			m_ProducerDialog->Update();
		}

		m_ProducerName_textCtrl->SetValue(m_CurrentProducer.name);
		m_ProducerSite_textCtrl->SetValue(m_CurrentProducer.webSite);
	}
}

/// Dialog Model
//----------------------------------------------------------------------------
void appOpCreateProsthesis::ShowModelDialog()
{
	wxString title = "Edit Model";

	//////////////////////////////////////////////////////////////////////////
	if (m_ModelDialog == NULL)
	{
		m_ModelDialog = new albaGUIDialog(title, albaCLOSEWINDOW);

		//////////////////////////////////////////////////////////////////////////

		wxBoxSizer *mainBoxSizer = new wxBoxSizer(wxVERTICAL);
		wxBoxSizer *main2BoxSizer = new wxBoxSizer(wxHORIZONTAL);
		wxBoxSizer *imageBoxSizer = new wxBoxSizer(wxVERTICAL);
		//
		int panelWidth = 400;
		wxString imagesPath = appUtils::GetConfigDirectory().c_str();
		wxString imgPath = imagesPath + "/Wizard/Model.bmp";

		if (wxFileExists(m_CurrentModel.image))
			imgPath = m_CurrentModel.image;

		if (wxFileExists(imgPath))
		{
			wxBitmap *previewBitmap;

			// Load and show the image
			m_ModelImage = new wxImage();
			m_ModelImage->LoadFile(imgPath.c_str(), wxBITMAP_TYPE_ANY);

			previewBitmap = new wxBitmap(*m_ModelImage);
			m_ModelImageButton = new albaGUIPicButton(m_ModelDialog, previewBitmap, -1);

			panelWidth = m_ModelImage->GetWidth();

			imageBoxSizer->Add(m_ModelImageButton, 0, wxALL | wxALIGN_LEFT, 0);

			// BUTTON - Change Model Brand Image
			albaGUIButton *immBtn = new albaGUIButton(m_ModelDialog, ID_MODEL_DIALOG_IMM, "Change Image", wxPoint(-1, -1));
			immBtn->SetListener(this);
			imageBoxSizer->Add(immBtn, 0, wxALIGN_RIGHT, 0);

			main2BoxSizer->Add(imageBoxSizer, 0, wxALL, 0);

			delete previewBitmap;
		}

		//////////////////////////////////////////////////////////////////////////
		// Info Sizer
		wxBoxSizer *infoBoxSizer = new wxBoxSizer(wxVERTICAL);

		// TEXT - Model Name
		wxStaticBoxSizer *labelSizer1 = new wxStaticBoxSizer(wxVERTICAL, m_ModelDialog, "Model Name");
		m_ModelName_textCtrl = new wxTextCtrl(m_ModelDialog, ID_MODEL_DIALOG_TEXT, m_CurrentModel.name, wxPoint(-1, -1), wxSize(panelWidth, 25), wxALL | wxEXPAND);
		m_ModelName_textCtrl->SetEditable(true);
		m_ModelName_textCtrl->SetMaxLength(64);
		labelSizer1->Add(m_ModelName_textCtrl, 0, wxALL | wxEXPAND, 0);
		infoBoxSizer->Add(labelSizer1, 0, wxALL | wxEXPAND, 5);

		// TEXT - Empty Separator
		infoBoxSizer->Add(new albaGUILab(m_ModelDialog, -1, " "), 0, wxALIGN_LEFT, 5);

		main2BoxSizer->Add(infoBoxSizer, 0, wxALL, 0);
		mainBoxSizer->Add(main2BoxSizer, 0, wxALL, 5);

		// BUTTON - Ok
		albaGUIButton *okBtn = new albaGUIButton(m_ModelDialog, ID_MODEL_DIALOG_OK_PRESSED, "OK", wxPoint(-1, -1));
		okBtn->SetListener(this);
		mainBoxSizer->Add(okBtn, 0, wxALIGN_RIGHT, 0);

		//////////////////////////////////////////////////////////////////////////
		m_ModelDialog->Add(mainBoxSizer, 0, wxALL, 5);
		m_ModelDialog->Fit();

		// Show dialog
		wxSize s = albaGetFrame()->GetSize();
		wxPoint p = albaGetFrame()->GetPosition();
		int posX = p.x + s.GetWidth() * .5 - m_ModelDialog->GetSize().GetWidth() * .5;
		int posY = p.y + s.GetHeight() * .5 - m_ModelDialog->GetSize().GetHeight() * .5;

		m_ModelDialog->SetPosition(wxPoint(posX, posY));
	}
	else
	{
		UpdateModelDialog();
	}

	if (m_IsModelDialogOpened)
		HideModelDialog();

	m_ModelDialog->ShowModal();

	m_IsModelDialogOpened = true;
	m_ModelOkButtonPressed = false;
}
//----------------------------------------------------------------------------
void appOpCreateProsthesis::HideModelDialog()
{
	if (m_ModelDialog)
		m_ModelDialog->Hide();

	m_IsModelDialogOpened = false;
}
//----------------------------------------------------------------------------
void appOpCreateProsthesis::UpdateModelDialog()
{
	if (m_ModelDialog)
	{
		// Update Image
		if (wxFileExists(m_CurrentModel.image))
		{
			m_ModelImage->LoadFile(m_CurrentModel.image.c_str(), wxBITMAP_TYPE_ANY);
			m_ModelImageButton->SetBitmapSelected(wxBitmap(*m_ModelImage));
			m_ModelImageButton->Update();
			m_ModelDialog->Update();
		}

		m_ModelName_textCtrl->SetValue(m_CurrentModel.name);
	}
}

/// Dialog Component
//----------------------------------------------------------------------------
void appOpCreateProsthesis::ShowComponentDialog()
{
	wxString title = "Edit Component";

	//////////////////////////////////////////////////////////////////////////
	if (m_ComponentDialog == NULL)
	{
		m_ComponentDialog = new albaGUIDialog(title, albaCLOSEWINDOW);

		//////////////////////////////////////////////////////////////////////////

		wxBoxSizer *mainBoxSizer = new wxBoxSizer(wxVERTICAL);

		int panelWidth = 400;

		//////////////////////////////////////////////////////////////////////////
		// Info Sizer
		wxBoxSizer *infoBoxSizer = new wxBoxSizer(wxVERTICAL);

		// TEXT - Component Name
		wxStaticBoxSizer *labelSizer1 = new wxStaticBoxSizer(wxVERTICAL, m_ComponentDialog, "Component Name");
		m_ComponentName_textCtrl = new wxTextCtrl(m_ComponentDialog, ID_COMPONENT_DIALOG_TEXT, m_CurrentComponent.name, wxPoint(-1, -1), wxSize(panelWidth, 25), wxALL | wxEXPAND);
		m_ComponentName_textCtrl->SetEditable(true);
		m_ComponentName_textCtrl->SetMaxLength(64);
		labelSizer1->Add(m_ComponentName_textCtrl, 0, wxALL | wxEXPAND, 0);
		infoBoxSizer->Add(labelSizer1, 0, wxALL | wxEXPAND, 5);

		// TEXT - Empty Separator
		infoBoxSizer->Add(new albaGUILab(m_ComponentDialog, -1, " "), 0, wxALIGN_LEFT, 5);

		mainBoxSizer->Add(infoBoxSizer, 0, wxALL, 5);

		// BUTTON - Ok
		albaGUIButton *okBtn = new albaGUIButton(m_ComponentDialog, ID_COMPONENT_DIALOG_OK_PRESSED, "OK", wxPoint(-1, -1));
		okBtn->SetListener(this);
		mainBoxSizer->Add(okBtn, 0, wxALIGN_RIGHT, 0);

		//////////////////////////////////////////////////////////////////////////
		m_ComponentDialog->Add(mainBoxSizer, 0, wxALL, 5);
		m_ComponentDialog->Fit();

		// Show dialog
		wxSize s = albaGetFrame()->GetSize();
		wxPoint p = albaGetFrame()->GetPosition();
		int posX = p.x + s.GetWidth() * .5 - m_ComponentDialog->GetSize().GetWidth() * .5;
		int posY = p.y + s.GetHeight() * .5 - m_ComponentDialog->GetSize().GetHeight() * .5;

		m_ComponentDialog->SetPosition(wxPoint(posX, posY));
	}
	else
	{
		UpdateComponentDialog();
	}

	if (m_IsComponentDialogOpened)
		HideComponentDialog();

	m_ComponentDialog->ShowModal();

	m_IsComponentDialogOpened = true;
	m_ComponentOkButtonPressed = false;
}
//----------------------------------------------------------------------------
void appOpCreateProsthesis::HideComponentDialog()
{
	if (m_ComponentDialog)
		m_ComponentDialog->Hide();

	m_IsComponentDialogOpened = false;
}
//----------------------------------------------------------------------------
void appOpCreateProsthesis::UpdateComponentDialog()
{
	if (m_ComponentDialog)
	{
		m_ComponentName_textCtrl->SetValue(m_CurrentComponent.name);
	}
}