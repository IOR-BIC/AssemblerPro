/*=========================================================================
Program:   AssemblerPro
Module:    appGUIDialogProsthesis.cpp
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

#include "appGUIDialogProsthesis.h"
#include "appLogic.h"
#include "appUtils.h"

#include "albaDecl.h"
#include "albaGUIButton.h"
#include "albaGUILab.h"
#include "albaGUIPicButton.h"
#include "albaGUIValidator.h"
#include "albaServiceClient.h"

#include "wx\statline.h"
#include "wx\image.h"
#include "wx\combobox.h"
#include "wx\window.h"

enum PROSTHESIS_DIALOG_ID
{
	ID_DIALOG_TEXT = MINID,
	ID_DIALOG_PRODUCER,
	ID_DIALOG_PRODUCER_EDIT,
	ID_DIALOG_PRODUCER_ADD,
	ID_DIALOG_TYPE,
	ID_DIALOG_SIDE,
	ID_DIALOG_IMM,
	ID_DIALOG_COMPONENT,
	ID_DIALOG_COMPONENT_EDIT,
	ID_DIALOG_COMPONENT_ADD,
	ID_DIALOG_COMPONENT_REM,
	ID_DIALOG_CANCEL_PRESSED,
	ID_DIALOG_OK_PRESSED,
};

//----------------------------------------------------------------------------
appGUIDialogProsthesis::appGUIDialogProsthesis(const wxString& title, long style)
: albaGUIDialog(title, style)
{
	m_Gui = NULL;
	m_MainBoxSizer = NULL;
	m_ImageSizer = NULL;
	m_ImageComboBox = NULL;
	m_Image = NULL;
	m_ImageButton = NULL;
	m_Name_textCtrl = NULL;

	// Load Producers Info
	std::vector<albaProDBProducer *> DBproducers = ((appLogic*)GetLogicManager())->GetProsthesisDBManager()->GetProducers();

	m_ProducerNameList.clear();
	m_SelectedProducer = 0;
	for (int p = 0; p < DBproducers.size(); p++)
	{
		m_ProducerNameList.push_back(DBproducers[p]->GetName().GetCStr());
	}

	m_SelectedType = 0;
	m_SelectedSide = 0;

	m_ComponentGroupCombo = NULL;
	m_ComponentNameList.clear();
	m_ComponentNameList.push_back("Group");
	m_SelectedComponentGroup = 0;

	CreateDialog();
}
//----------------------------------------------------------------------------
appGUIDialogProsthesis::~appGUIDialogProsthesis()
{
	m_ProducerNameList.clear();
}

//----------------------------------------------------------------------------
void appGUIDialogProsthesis::OnEvent(albaEventBase *alba_event)
{
	switch (alba_event->GetId())
	{
	case ID_DIALOG_TEXT: break;
	case ID_DIALOG_IMM: SelectImage(); break;
	case ID_DIALOG_TYPE: break;
	case ID_DIALOG_SIDE: break;

	case ID_DIALOG_COMPONENT_ADD: AddComponentGroup(); break;
	case ID_DIALOG_COMPONENT_EDIT: EditComponentGroup(); break;
	case ID_DIALOG_COMPONENT_REM: RemoveComponentGroup(); break;

	case ID_DIALOG_OK_PRESSED:
	{
		// Update Prosthesis
		m_CurrentProsthesis.name = m_Name_textCtrl->GetValue();
		m_CurrentProsthesis.image = m_ImagePath;
		m_CurrentProsthesis.producer = m_ProducerNameList[m_SelectedProducer];
		m_CurrentProsthesis.type = m_SelectedType;
		m_CurrentProsthesis.side = m_SelectedSide;

		m_CurrentProsthesis.componentGroup.clear();
		for (int c = 0; c < m_ComponentNameList.size(); c++)
			m_CurrentProsthesis.componentGroup.push_back(m_ComponentNameList[c]);

		m_CurrentProsthesis.isChanged = true;

		this->Close();
	}
	break;
	case ID_DIALOG_CANCEL_PRESSED:
		m_CurrentProsthesis.isChanged = false;
		this->Close();
		break;

	default:
		albaGUIDialog::OnEvent(alba_event);
	}
}
//----------------------------------------------------------------------------
void appGUIDialogProsthesis::RemoveComponentGroup()
{
	if (m_ComponentNameList.size() > 1 && m_SelectedComponentGroup < m_ComponentNameList.size())
	{
		m_ComponentNameList.erase(m_ComponentNameList.begin() + m_SelectedComponentGroup);
		m_ComponentGroupCombo->Delete(m_SelectedComponentGroup);

		m_SelectedComponentGroup--;

		m_ComponentGroupCombo->Select(m_SelectedComponentGroup);
		m_ComponentGroupCombo->Update();
	}

	m_Gui->Enable(ID_DIALOG_COMPONENT_REM, m_ComponentNameList.size() > 1);
}
//----------------------------------------------------------------------------
void appGUIDialogProsthesis::EditComponentGroup()
{
	wxTextEntryDialog *dlg = new wxTextEntryDialog(this, "Name", "Component Group", m_ComponentNameList[m_SelectedComponentGroup]);
	int result = dlg->ShowModal();
	wxString name = dlg->GetValue();

	if (!name.IsEmpty())
	{
		m_ComponentNameList[m_SelectedComponentGroup] = name;
		m_ComponentGroupCombo->SetString(m_SelectedComponentGroup, name);
		m_ComponentGroupCombo->Select(m_SelectedComponentGroup);
		m_ComponentGroupCombo->Update();
	}
}
//----------------------------------------------------------------------------
void appGUIDialogProsthesis::AddComponentGroup()
{
	wxTextEntryDialog *dlg = new wxTextEntryDialog(this, "Name", "Component Group", "");
	int result = dlg->ShowModal();
	wxString name = dlg->GetValue();

	if (!name.IsEmpty())
	{
		m_ComponentNameList.push_back(name);
		m_SelectedComponentGroup = m_ComponentNameList.size() - 1;
		m_ComponentGroupCombo->Insert(name, m_SelectedComponentGroup);
		m_ComponentGroupCombo->Select(m_SelectedComponentGroup);
	}

	m_Gui->Enable(ID_DIALOG_COMPONENT_REM, m_ComponentNameList.size() > 1);
}

//----------------------------------------------------------------------------
void appGUIDialogProsthesis::SelectImage()
{
	albaString fileNameFullPath = albaGetDocumentsDirectory().c_str();
	albaString wildc = "Image file (*.bmp)|*.bmp";
	wxString imagePath = albaGetOpenFile(fileNameFullPath.GetCStr(), wildc, "Select file").c_str();

	if (wxFileExists(imagePath))
	{
		m_ImagePath = imagePath;
		UpdateDialog();
	}
}

//----------------------------------------------------------------------------
void appGUIDialogProsthesis::SetProsthesis(Prosthesis &prosthesis)
{
	m_CurrentProsthesis = prosthesis;

	m_Name_textCtrl->SetValue(m_CurrentProsthesis.name);
	m_ImagePath = m_CurrentProsthesis.image;
	m_SelectedType = m_CurrentProsthesis.type;
	m_SelectedSide = m_CurrentProsthesis.side;

	m_SelectedProducer = 0;
	for (int p = 0; p < m_ProducerNameList.size(); p++)
		if (m_ProducerNameList[p] == m_CurrentProsthesis.producer)
			m_SelectedProducer = p;

	m_SelectedComponentGroup = 0;
	for (int c = 0; c < m_CurrentProsthesis.componentGroup.size(); c++)
		m_ComponentNameList.push_back(m_CurrentProsthesis.componentGroup[c]);

	if (m_ComponentNameList.size() == 0)
	{
		m_ComponentNameList.push_back("Group");
	}
}

//----------------------------------------------------------------------------
void appGUIDialogProsthesis::Show()
{
	CreateDialog();
	ShowModal();
}

//----------------------------------------------------------------------------
void appGUIDialogProsthesis::CreateDialog()
{
	if (m_Gui == NULL)
	{
		m_Gui = new albaGUI(this);		

		//////////////////////////////////////////////////////////////////////////

		m_MainBoxSizer = new wxBoxSizer(wxVERTICAL);
		wxBoxSizer *mainBoxSizer2 = new wxBoxSizer(wxVERTICAL);
		wxBoxSizer *mainBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);

		//
		int dialogWidth = 300;
		int imageWidth = 0;

		wxString imagesPath = appUtils::GetConfigDirectory().c_str();
		wxString imgPath = imagesPath + "/Wizard/Model.bmp";

		if (wxFileExists(m_CurrentProsthesis.image))
			imgPath = m_CurrentProsthesis.image;

		if (wxFileExists(imgPath))
		{
			// IMAGE Prosthesis
			m_ImageSizer = new wxBoxSizer(wxVERTICAL);

			wxImage *previewImage = new wxImage();
			previewImage->LoadFile(imgPath.c_str(), wxBITMAP_TYPE_ANY);

			wxBitmap *previewBitmap = new wxBitmap(*previewImage);
			m_ImageButton = new albaGUIPicButton(this, previewBitmap, -1);

			imageWidth = previewImage->GetWidth();

			m_ImageSizer->Add(m_ImageButton, 0, wxALL | wxALIGN_LEFT, 0);
			mainBoxSizer2->Add(m_ImageSizer);

			// BUTTON - Change Producer Brand Image
			albaGUIButton *immBtn = new albaGUIButton(this, ID_DIALOG_IMM, "Change Image", wxPoint(-1, -1));
			immBtn->SetListener(this);
			mainBoxSizer2->Add(immBtn, 0, wxALIGN_RIGHT, 0);
		
			delete previewBitmap;
			delete previewImage;
		}

		int panelWidth = dialogWidth - imageWidth;

		//////////////////////////////////////////////////////////////////////////
		// Info Sizer
		wxBoxSizer *infoBoxSizer = new wxBoxSizer(wxVERTICAL);
		infoBoxSizer->SetMinSize(panelWidth, 300);

		// TEXT - Prosthesis Name
		wxStaticBoxSizer *labelSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, "Name");
		m_Name_textCtrl = new wxTextCtrl(this, ID_DIALOG_TEXT, m_CurrentProsthesis.name, wxPoint(-1, -1), wxSize(panelWidth, 20), wxALL | wxEXPAND);
		m_Name_textCtrl->SetEditable(true);
		m_Name_textCtrl->SetMaxLength(64);
		labelSizer1->Add(m_Name_textCtrl, 0, wxALL | wxEXPAND, 0);

		infoBoxSizer->Add(labelSizer1, 0, wxALL | wxEXPAND, 5);

		//////////////////////////////////////////////////////////////////////////

		wxStaticBoxSizer *producerBoxSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, "Producer");

		// COMBO - Producer
		wxComboBox *producerCombo = new wxComboBox(this, ID_DIALOG_PRODUCER, "", wxPoint(-1, -1), wxSize(panelWidth, 20), m_ProducerNameList.size(), &m_ProducerNameList[0]);
		producerCombo->SetValidator(albaGUIValidator(this, ID_DIALOG_PRODUCER, producerCombo, &m_SelectedProducer));
		producerBoxSizer->Add(producerCombo, 0, wxALL | wxEXPAND, 0);

		wxBoxSizer *producerBtnSizer = new wxBoxSizer(wxHORIZONTAL);

		// BUTTON - Producer Edit
		albaGUIButton *producerEditBtn = new albaGUIButton(this, ID_DIALOG_PRODUCER_EDIT, "Edit", wxPoint(-1, -1), wxSize(-1, -1));
		producerEditBtn->SetListener(this);
		producerEditBtn->Enable(false);
		producerBtnSizer->Add(producerEditBtn, 0, wxALIGN_RIGHT, 5);

		// BUTTON - Producer Add
		albaGUIButton *producerAddBtn = new albaGUIButton(this, ID_DIALOG_PRODUCER_ADD, "Add New", wxPoint(-1, -1), wxSize(-1, -1));
		producerAddBtn->SetListener(this);
		producerAddBtn->Enable(false);
		producerBtnSizer->Add(producerAddBtn, 0, wxALIGN_RIGHT, 5);

		producerBoxSizer->Add(producerBtnSizer, 0, wxALL | wxEXPAND);

		infoBoxSizer->Add(producerBoxSizer, 0, wxALL | wxEXPAND, 5);
		
		//////////////////////////////////////////////////////////////////////////
		wxBoxSizer *propBoxSizer = new wxBoxSizer(wxHORIZONTAL);

		wxStaticBoxSizer *typeBoxSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, "Type");

		// COMBO - Type
		wxString typeChoices[]{ "Acetabular","Femoral","Modular" };
		wxComboBox *typeCombo = new wxComboBox(this, ID_DIALOG_PRODUCER, "", wxPoint(-1, -1), wxSize(panelWidth, 20), 3, typeChoices);
		typeCombo->SetValidator(albaGUIValidator(this, ID_DIALOG_PRODUCER, typeCombo, &m_SelectedType));
		typeBoxSizer->Add(typeCombo, 0, wxALL | wxEXPAND, 0);

		propBoxSizer->Add(typeBoxSizer, 0, wxALL | wxEXPAND, 5);
				
		// RADIO - Side
		wxString sideChoices[]{ "Left","Right","Both" };
		wxRadioBox *sideRadioBox = new wxRadioBox(this, ID_DIALOG_SIDE, "Side", wxPoint(-1, -1), wxSize(-1, -1), 3, sideChoices);
		sideRadioBox->SetValidator(albaGUIValidator(this, ID_DIALOG_SIDE, sideRadioBox, &m_SelectedSide));
		propBoxSizer->Add(sideRadioBox, 0, wxALL | wxEXPAND, 5);

		infoBoxSizer->Add(propBoxSizer, 0, wxALL | wxEXPAND, 0);
		
		//////////////////////////////////////////////////////////////////////////

		wxStaticBoxSizer *componentGroupBoxSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, "Component Group");

		// COMBO - Component Group
		m_ComponentGroupCombo = new wxComboBox(this, ID_DIALOG_COMPONENT, "", wxPoint(-1, -1), wxSize(panelWidth, 20), m_ComponentNameList.size(), &m_ComponentNameList[0]);
		m_ComponentGroupCombo->SetValidator(albaGUIValidator(this, ID_DIALOG_COMPONENT, m_ComponentGroupCombo, &m_SelectedComponentGroup));
		componentGroupBoxSizer->Add(m_ComponentGroupCombo, 0, wxALL | wxEXPAND, 0);

		// BUTTON - Component Group Edit
		albaGUIButton *componentGroupEditBtn = new albaGUIButton(this, ID_DIALOG_COMPONENT_EDIT, "Edit", wxPoint(-1, -1), wxSize(80, 23));
		componentGroupEditBtn->SetListener(this);
		componentGroupBoxSizer->Add(componentGroupEditBtn, 0, wxALIGN_RIGHT, 5);

		// BUTTON - Component Group Add
		albaGUIButton *componentGroupAddBtn = new albaGUIButton(this, ID_DIALOG_COMPONENT_ADD, "+", wxPoint(-1, -1), wxSize(50, 23));
		componentGroupAddBtn->SetListener(this);
		componentGroupBoxSizer->Add(componentGroupAddBtn, 0, wxALIGN_RIGHT, 5);

		// BUTTON - Component Group Del
		albaGUIButton *componentGroupDelBtn = new albaGUIButton(this, ID_DIALOG_COMPONENT_REM, "-", wxPoint(-1, -1), wxSize(50, 23));
		componentGroupDelBtn->SetListener(this);
		componentGroupBoxSizer->Add(componentGroupDelBtn, 0, wxALIGN_RIGHT, 5);

		infoBoxSizer->Add(componentGroupBoxSizer, 0, wxALL | wxEXPAND, 5);

		//////////////////////////////////////////////////////////////////////////
		
		// TEXT - Empty Separator
		infoBoxSizer->Add(new albaGUILab(this, -1, " "), 0, wxALIGN_LEFT, 5);

		mainBoxSizer3->Add(mainBoxSizer2, 0, wxALL, 0);
		mainBoxSizer3->Add(infoBoxSizer, 0, wxALL, 0);
		m_MainBoxSizer->Add(mainBoxSizer3, 0, wxALL, 5);
		
		//////////////////////////////////////////////////////////////////////////

		// LINE
		m_MainBoxSizer->Add(new wxStaticLine(this, -1, wxPoint(-1, -1), wxSize(dialogWidth + imageWidth + 60, 1)), 0, wxALL, 5);

		// Buttons
		wxBoxSizer *btnBoxSizer = new wxBoxSizer(wxHORIZONTAL);

		// BUTTON - Cancel
		albaGUIButton *cancelBtn = new albaGUIButton(this, ID_DIALOG_CANCEL_PRESSED, "Cancel", wxPoint(-1, -1));
		cancelBtn->SetListener(this);
		btnBoxSizer->Add(cancelBtn, 0, wxALIGN_RIGHT, 15);

		// BUTTON - Ok
		albaGUIButton *okBtn = new albaGUIButton(this, ID_DIALOG_OK_PRESSED, "OK", wxPoint(-1, -1));
		okBtn->SetListener(this);
		btnBoxSizer->Add(okBtn, 0, wxALIGN_RIGHT, 15);

		m_MainBoxSizer->Add(btnBoxSizer, 0, wxALL | wxALIGN_RIGHT, 5);

		//////////////////////////////////////////////////////////////////////////

		m_ImageSizer->Fit(this);
		m_MainBoxSizer->Fit(this);
		m_Gui->Fit();
		Fit();

		// Show dialog
		wxSize s = albaGetFrame()->GetSize();
		wxPoint p = albaGetFrame()->GetPosition();

		SetSize(dialogWidth + imageWidth + 70, 420);

		int posX = p.x + s.GetWidth() * .5 - this->GetSize().GetWidth() * .5;
		int posY = p.y + s.GetHeight() * .5 - this->GetSize().GetHeight() * .5;

		this->SetPosition(wxPoint(posX, posY));

		Add(m_Gui, 1);
		Add(m_MainBoxSizer, 0);
	}

	UpdateDialog();
}

//----------------------------------------------------------------------------
void appGUIDialogProsthesis::UpdateDialog()
{
	if (m_Gui)
	{
		// Update Producer
		for (int p = 0; p < m_ProducerNameList.size(); p++)
		{
			if (m_CurrentProsthesis.producer == m_ProducerNameList[p]) m_SelectedProducer = p;
		}
		
		// Update Image
		if (wxFileExists(m_ImagePath))
		{
			if (m_ImageButton != NULL)
			{
				m_ImageSizer->Remove(m_ImageButton);
				delete m_ImageButton;
			}

			// Load and show the image
			wxImage *previewImage = new wxImage();
			previewImage->LoadFile(m_ImagePath.c_str(), wxBITMAP_TYPE_ANY);

			wxBitmap *previewBitmap;

			// if (flipImage)
			// previewBitmap = new wxBitmap(previewImage->Mirror(true));
			// else
			previewBitmap = new wxBitmap(previewImage);

			m_ImageButton = new albaGUIPicButton(this, previewBitmap, -1);
			m_ImageSizer->Add(m_ImageButton);

			m_ImageSizer->Fit(this);
			m_MainBoxSizer->Fit(this);
			m_Gui->Fit();
			Fit();

			delete previewBitmap;
			delete previewImage;
		}

		m_Name_textCtrl->SetValue(m_CurrentProsthesis.name);
	}
}