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
#include "albaVME.h"
#include "wx\window.h"
#include "albaGUIDialog.h"
#include "appUtils.h"
#include "albaGUIPicButton.h"
#include "albaGUILab.h"
#include "albaGUIButton.h"
#include "wx\image.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(appOpCreateProsthesis);

//----------------------------------------------------------------------------
appOpCreateProsthesis::appOpCreateProsthesis(wxString label) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;

	m_ProducerComboBox = NULL;
	m_ModelComboBox = NULL;
	m_ComponentsComboBox = NULL;

	m_SelectedProducer = 0;
	m_SelectedModel = 0;
	m_SelectedComponent = 0;

	m_ProducerDialog = NULL;
	m_IsProducerDialogOpened = false;
	m_OkButtonPressed = false;
}

//----------------------------------------------------------------------------
appOpCreateProsthesis::~appOpCreateProsthesis()
{
	m_ProducerList.clear();
	m_ModelList.clear();

	cppDEL(m_ProducerDialog);
}

//----------------------------------------------------------------------------
bool appOpCreateProsthesis::Accept(albaVME *node)
{
	//return node->IsA("...");
	return true;
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

//----------------------------------------------------------------------------
void appOpCreateProsthesis::LoadInfo()
{
	// Load Producers Info
	m_ProducerList.clear();
	m_ProducerList.push_back("");
	m_ProducerList.push_back("Producer 1");
	m_ProducerList.push_back("Producer 2");
	m_ProducerList.push_back("Producer 3");

	// Load Models Info
	m_ModelList.clear();
	m_ModelList.push_back("");
	m_ModelList.push_back("Model 1");
	m_ModelList.push_back("Model 2");
	m_ModelList.push_back("Model 3");

	// Load Components Info
	m_ComponentList.clear();
	m_ComponentList.push_back("");
	m_ComponentList.push_back("Component 1");
	m_ComponentList.push_back("Component 2");
	m_ComponentList.push_back("Component 3");
}

//----------------------------------------------------------------------------
void appOpCreateProsthesis::OpStop(int result)
{
	if (!m_TestMode)
	{
		HideGui();
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

			case ID_SELECT_PRODUCER:
				m_SelectedModel = 0; // Reset Selection
				m_SelectedComponent = 0; // Reset Selection
				UpdateGui();
			break;

			case ID_EDIT_PRODUCER:
				EditProducer();
				break;

			case ID_ADD_PRODUCER:
				AddProducer();
				break;

			case ID_SELECT_MODEL:
				UpdateGui();
				break;

			case ID_EDIT_MODEL:
				EditModel();
				break;

			case ID_ADD_MODEL:
				AddModel();
				break;

			case ID_SELECT_COMPONENT:
				UpdateGui();
				break;

			case ID_EDIT_COMPONENT:
				EditComponent();
				break;

			case ID_ADD_COMPONENT:
				AddComponent();
				break;

			case ID_PRODUCER_DIALOG_OK_PRESSED:
			{
				HideProducerDialog();
				m_OkButtonPressed = true;
			}
			break;

			case ID_PRODUCER_DIALOG_TEXT:

				break;

			case ID_PRODUCER_DIALOG_IMM:
			{
				albaString fileNameFullPath = albaGetDocumentsDirectory().c_str();
				albaString wildc = "Image file (*.bmp)|*.bmp";
				m_ProducerImage = albaGetOpenFile(fileNameFullPath.GetCStr(), wildc, "Select file").c_str();

				UpdateProducerDialog();
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
void appOpCreateProsthesis::CreateGui()
{
	// Interface:
	m_Gui = new appGUI(this);

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("Producer", true);
	m_ProducerComboBox = m_Gui->Combo(ID_SELECT_PRODUCER, "", &m_SelectedProducer, m_ProducerList.size(), &m_ProducerList[0], "");
	m_Gui->TwoButtons(ID_EDIT_PRODUCER, ID_ADD_PRODUCER, "Edit", "Add New");
	m_Gui->Divider(1);

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("Model", true);
	m_ModelComboBox = m_Gui->Combo(ID_SELECT_MODEL, "", &m_SelectedModel, m_ModelList.size(), &m_ModelList[0], "");
	m_Gui->TwoButtons(ID_EDIT_MODEL, ID_ADD_MODEL, "Edit", "Add New");
	
	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("Components", true);
	m_ComponentsComboBox = m_Gui->Combo(ID_SELECT_COMPONENT, "", &m_SelectedComponent, m_ComponentList.size(), &m_ComponentList[0], "");
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

//----------------------------------------------------------------------------
void appOpCreateProsthesis::AddProducer()
{
	m_ProducerName = wxString::Format("new Producer %d", m_ProducerList.size());
	m_ProducerSite = "www." + m_ProducerList[m_SelectedProducer] + ".com";

	m_ProducerList.push_back(m_ProducerName);
	m_ProducerComboBox->Append(m_ProducerName);
	m_SelectedProducer = m_ProducerList.size() - 1;
	
	ShowProducerDialog();

	UpdateGui();
}
//----------------------------------------------------------------------------
void appOpCreateProsthesis::EditProducer()
{
	m_ProducerName = m_ProducerList[m_SelectedProducer];
	
	ShowProducerDialog();

	m_ProducerName_textCtrl->Update();
	m_ProducerSite_textCtrl->Update();

	wxString newName = m_ProducerName_textCtrl->GetValue();
	wxString newSite = m_ProducerSite_textCtrl->GetValue();

	m_ProducerList[m_SelectedProducer] = newName;
	m_ProducerComboBox->SetString(m_SelectedProducer, newName);

	UpdateGui();
}

//----------------------------------------------------------------------------
void appOpCreateProsthesis::AddModel()
{

}
//----------------------------------------------------------------------------
void appOpCreateProsthesis::EditModel()
{

}

//----------------------------------------------------------------------------
void appOpCreateProsthesis::AddComponent()
{

}
//----------------------------------------------------------------------------
void appOpCreateProsthesis::EditComponent()
{

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
		wxString imgPath = imagesPath + "/Wizard/DialogV.bmp";

		if (wxFileExists(imgPath))
		{
			//wxImage *previewImage;
			wxBitmap *previewBitmap;

			// Load and show the image
			m_PreviewImage = new wxImage();
			m_PreviewImage->LoadFile(imgPath.c_str(), wxBITMAP_TYPE_ANY);

			previewBitmap = new wxBitmap(*m_PreviewImage);
			m_PreviewImageButton = new albaGUIPicButton(m_ProducerDialog, previewBitmap, -1);

			panelWidth = m_PreviewImage->GetWidth();

			mainBoxSizer->Add(m_PreviewImageButton, 0, wxALL | wxALIGN_CENTER, 0);

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
		m_ProducerName_textCtrl = new wxTextCtrl(m_ProducerDialog, ID_PRODUCER_DIALOG_TEXT, m_ProducerName, wxPoint(-1, -1), wxSize(panelWidth, 25), wxALL | wxEXPAND);
		m_ProducerName_textCtrl->SetEditable(true);
		labelSizer1->Add(m_ProducerName_textCtrl, 0, wxALL | wxEXPAND, 0);
		infoBoxSizer->Add(labelSizer1, 0, wxALL | wxEXPAND, 5);

		// TEXT - Producer Web Site
		wxStaticBoxSizer *labelSizer2 = new wxStaticBoxSizer(wxVERTICAL, m_ProducerDialog, "Web Site");
		m_ProducerSite_textCtrl = new wxTextCtrl(m_ProducerDialog, ID_PRODUCER_DIALOG_TEXT, m_ProducerSite, wxPoint(-1, -1), wxSize(panelWidth, 25), wxALL | wxEXPAND);
		m_ProducerSite_textCtrl->SetEditable(true);
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

	if (m_IsProducerDialogOpened)
		HideProducerDialog();

	m_ProducerDialog->ShowModal();

	m_IsProducerDialogOpened = true;
	m_OkButtonPressed = false;
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
	if (wxFileExists(m_ProducerImage))
	{
		m_PreviewImage->LoadFile(m_ProducerImage.c_str(), wxBITMAP_TYPE_ANY);
		m_PreviewImageButton->Update();
		m_ProducerDialog->Update();
	}

}
