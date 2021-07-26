/*=========================================================================
Program:   AssemblerPro
Module:    appGUIDialogModel.cpp
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

#include "appGUIDialogModel.h"
#include "appUtils.h"

#include "albaDecl.h"
#include "albaGUIButton.h"
#include "albaGUILab.h"
#include "albaGUIPicButton.h"
#include "albaGUIValidator.h"

#include "wx\image.h"
#include "wx\window.h"

enum MODEL_DIALOG_ID
{
	ID_MODEL_DIALOG_TEXT = MINID,
	ID_MODEL_DIALOG_TYPE,
	ID_MODEL_DIALOG_SIDE,
	ID_MODEL_DIALOG_IMM,
	ID_MODEL_DIALOG_OK_PRESSED,
};

//----------------------------------------------------------------------------
appGUIDialogModel::appGUIDialogModel(const wxString& title, long style)
: albaGUIDialog(title, style)
{
	m_Gui = NULL;

	CreateModelDialog();
}
//----------------------------------------------------------------------------
appGUIDialogModel::~appGUIDialogModel()
{
}

//----------------------------------------------------------------------------
void appGUIDialogModel::OnEvent(albaEventBase *alba_event)
{
	switch (alba_event->GetId())
	{
	case ID_MODEL_DIALOG_TEXT:
		break;
	case ID_MODEL_DIALOG_IMM:
		SelectImage();
		break;
	case ID_MODEL_DIALOG_TYPE:
		break;
	case ID_MODEL_DIALOG_SIDE:
		break;
	case ID_MODEL_DIALOG_OK_PRESSED:
	{
		m_CurrentModel.name = m_ModelName_textCtrl->GetValue();
		this->Close();
	}
	break;

	default:
		albaGUIDialog::OnEvent(alba_event);
	}
}

//----------------------------------------------------------------------------
void appGUIDialogModel::SelectImage()
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

//----------------------------------------------------------------------------
void appGUIDialogModel::Show()
{
	CreateModelDialog();
	ShowModal();
}

//----------------------------------------------------------------------------
void appGUIDialogModel::CreateModelDialog()
{
	wxString title = "Edit Model";

	//////////////////////////////////////////////////////////////////////////
	if (m_Gui == NULL)
	{
		m_Gui = new albaGUI(this);		

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
			m_ModelImageButton = new albaGUIPicButton(this, previewBitmap, -1);

			panelWidth = m_ModelImage->GetWidth();

			imageBoxSizer->Add(m_ModelImageButton, 0, wxALL | wxALIGN_LEFT, 0);

			// BUTTON - Change Model Brand Image
			albaGUIButton *immBtn = new albaGUIButton(this, ID_MODEL_DIALOG_IMM, "Change Image", wxPoint(-1, -1));
			immBtn->SetListener(this);
			imageBoxSizer->Add(immBtn, 0, wxALIGN_RIGHT, 0);

			main2BoxSizer->Add(imageBoxSizer, 0, wxALL, 0);

			delete previewBitmap;
		}

		//////////////////////////////////////////////////////////////////////////
		// Info Sizer
		wxBoxSizer *infoBoxSizer = new wxBoxSizer(wxVERTICAL);

		// TEXT - Model Name
		wxStaticBoxSizer *labelSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, "Model Name");
		m_ModelName_textCtrl = new wxTextCtrl(this, ID_MODEL_DIALOG_TEXT, m_CurrentModel.name, wxPoint(-1, -1), wxSize(panelWidth, 20), wxALL | wxEXPAND);
		m_ModelName_textCtrl->SetEditable(true);
		m_ModelName_textCtrl->SetMaxLength(64);
		labelSizer1->Add(m_ModelName_textCtrl, 0, wxALL | wxEXPAND, 0);
		infoBoxSizer->Add(labelSizer1, 0, wxALL | wxEXPAND, 5);

		wxBoxSizer *propBoxSizer = new wxBoxSizer(wxHORIZONTAL);

		// RADIO - Type Model
		wxString typeChoices[]{ "Acetabular","Femoral","Modular" };
		wxRadioBox *typeRadioBox = new wxRadioBox(this, ID_MODEL_DIALOG_TYPE, "Type", wxPoint(-1, -1), wxSize(-1, -1), 3, typeChoices);
		typeRadioBox->SetValidator(albaGUIValidator(this, ID_MODEL_DIALOG_TYPE, typeRadioBox, &(m_CurrentModel.type)));
		propBoxSizer->Add(typeRadioBox, 0, wxALL | wxEXPAND, 5);

		// RADIO - Side Model
		wxString sideChoices[]{ "Left","Right","Both" };
		wxRadioBox *sideRadioBox = new wxRadioBox(this, ID_MODEL_DIALOG_SIDE, "Side", wxPoint(-1, -1), wxSize(-1, -1), 3, sideChoices);
		sideRadioBox->SetValidator(albaGUIValidator(this, ID_MODEL_DIALOG_SIDE, sideRadioBox, &(m_CurrentModel.side)));
		propBoxSizer->Add(sideRadioBox, 0, wxALL | wxEXPAND, 5);

		infoBoxSizer->Add(propBoxSizer, 0, wxALL | wxEXPAND, 0);

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		// TEST - Moved Components Gui into Dialog
		// 		wxStaticBoxSizer *labelSizer2 = new wxStaticBoxSizer(wxVERTICAL, this, "Components");
		// 		
		// 		// COMBO - Components
		// 		wxString componentChoices[]{""};
		// 		wxComboBox *componentComboBox = new wxComboBox(this, ID_SELECT_COMPONENT, "", wxPoint(-1, -1), wxSize(-1, -1), **componentChoices);
		// 		componentComboBox->SetValidator(albaGUIValidator(this, ID_SELECT_MODEL, componentComboBox, &m_SelectedComponent));
		// 		labelSizer2->Add(componentComboBox, 0, wxALL | wxEXPAND, 0);
		// 
		// 		wxBoxSizer *componentActionBoxSizer = new wxBoxSizer(wxHORIZONTAL);
		// 		
		// 		// BUTTON - Edit
		// 		albaGUIButton *editBtn = new albaGUIButton(this, ID_EDIT_COMPONENT, "Edit", wxPoint(-1, -1));
		// 		editBtn->SetListener(this);
		// 		componentActionBoxSizer->Add(editBtn, 0, wxALIGN_RIGHT, 0);
		// 
		// 		// BUTTON - Add
		// 		albaGUIButton *addBtn = new albaGUIButton(this, ID_ADD_COMPONENT, "Add", wxPoint(-1, -1));
		// 		addBtn->SetListener(this);
		// 		componentActionBoxSizer->Add(addBtn, 0, wxALIGN_RIGHT, 0);
		// 
		// 		labelSizer2->Add(componentActionBoxSizer, 0, wxALL | wxEXPAND, 0);
		// 
		// 		infoBoxSizer->Add(labelSizer2, 0, wxALL | wxEXPAND, 5);
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		
		// TEXT - Empty Separator
		infoBoxSizer->Add(new albaGUILab(this, -1, " "), 0, wxALIGN_LEFT, 5);

		main2BoxSizer->Add(infoBoxSizer, 0, wxALL, 0);
		mainBoxSizer->Add(main2BoxSizer, 0, wxALL, 5);

		// BUTTON - Ok
		albaGUIButton *okBtn = new albaGUIButton(this, ID_MODEL_DIALOG_OK_PRESSED, "OK", wxPoint(-1, -1));
		okBtn->SetListener(this);
		mainBoxSizer->Add(okBtn, 0, wxALIGN_RIGHT, 0);

		//////////////////////////////////////////////////////////////////////////
		//m_Gui->Add(mainBoxSizer, 0, wxALL, 5);
		m_Gui->Fit();
		Fit();

		// Show dialog
		wxSize s = albaGetFrame()->GetSize();
		wxPoint p = albaGetFrame()->GetPosition();

		SetSize(560, 395);

		int posX = p.x + s.GetWidth() * .5 - this->GetSize().GetWidth() * .5;
		int posY = p.y + s.GetHeight() * .5 - this->GetSize().GetHeight() * .5;

		this->SetPosition(wxPoint(posX, posY));

		Add(m_Gui, 1);
		Add(mainBoxSizer, 0);
	}

	UpdateModelDialog();
}

//----------------------------------------------------------------------------
void appGUIDialogModel::UpdateModelDialog()
{
	if (m_Gui)
	{
		// Update Image
		if (wxFileExists(m_CurrentModel.image))
		{
			m_ModelImage->LoadFile(m_CurrentModel.image.c_str(), wxBITMAP_TYPE_ANY);
			m_ModelImageButton->SetBitmapSelected(wxBitmap(*m_ModelImage));
			m_ModelImageButton->Update();
			m_Gui->Update();
		}

		m_ModelName_textCtrl->SetValue(m_CurrentModel.name);
	}
}