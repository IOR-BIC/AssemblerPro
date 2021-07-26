/*=========================================================================
Program:   AssemblerPro
Module:    appGUIDialogProducer.cpp
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

#include "appGUIDialogProducer.h"
#include "appUtils.h"

#include "albaDecl.h"
#include "albaGUIButton.h"
#include "albaGUILab.h"
#include "albaGUIPicButton.h"
#include "albaGUIValidator.h"

#include "wx\image.h"
#include "wx\window.h"

enum PRODUCER_DIALOG_ID
{
	ID_PRODUCER_DIALOG_TEXT = MINID,
	ID_PRODUCER_DIALOG_IMM,
	ID_PRODUCER_DIALOG_OK_PRESSED,
};

//----------------------------------------------------------------------------
appGUIDialogProducer::appGUIDialogProducer(const wxString& title, long style)
: albaGUIDialog(title, style)
{
	m_Gui = NULL;

	CreateProducerDialog();
}
//----------------------------------------------------------------------------
appGUIDialogProducer::~appGUIDialogProducer()
{
}

//----------------------------------------------------------------------------
void appGUIDialogProducer::OnEvent(albaEventBase *alba_event)
{
	switch (alba_event->GetId())
	{
	case ID_PRODUCER_DIALOG_TEXT:
	{
		m_CurrentProducer.name = m_ProducerName_textCtrl->GetValue();
		m_CurrentProducer.webSite = m_ProducerSite_textCtrl->GetValue();
	}
	break;

	case ID_PRODUCER_DIALOG_IMM:
	{
		SelectImage();
	}
	break;

	case ID_PRODUCER_DIALOG_OK_PRESSED:
	{
		m_CurrentProducer.name = m_ProducerName_textCtrl->GetValue();
		m_CurrentProducer.webSite = m_ProducerSite_textCtrl->GetValue();
		this->Close();
	}
	break;

	default:
		albaGUIDialog::OnEvent(alba_event);
	}
}

//----------------------------------------------------------------------------
void appGUIDialogProducer::SelectImage()
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

//----------------------------------------------------------------------------
void appGUIDialogProducer::Show()
{
	CreateProducerDialog();
	ShowModal();
}

//----------------------------------------------------------------------------
void appGUIDialogProducer::CreateProducerDialog()
{
	wxString title = "Edit Producer";

	//////////////////////////////////////////////////////////////////////////
	if (m_Gui == NULL)
	{
		m_Gui = new albaGUI(this);
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
			m_ProducerImageButton = new albaGUIPicButton(this, previewBitmap, -1);

			panelWidth = m_ProducerImage->GetWidth();

			mainBoxSizer->Add(m_ProducerImageButton, 0, wxALL | wxALIGN_CENTER, 0);

			// BUTTON - Change Producer Brand Image
			albaGUIButton *immBtn = new albaGUIButton(this, ID_PRODUCER_DIALOG_IMM, "Change Image", wxPoint(-1, -1));
			immBtn->SetListener(this);
			mainBoxSizer->Add(immBtn, 0, wxALIGN_RIGHT, 0);

			delete previewBitmap;
		}

		//////////////////////////////////////////////////////////////////////////
		// Info Sizer
		wxBoxSizer *infoBoxSizer = new wxBoxSizer(wxVERTICAL);

		// TEXT - Producer Name
		wxStaticBoxSizer *labelSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, "Producer Name");
		m_ProducerName_textCtrl = new wxTextCtrl(this, ID_PRODUCER_DIALOG_TEXT, m_CurrentProducer.name, wxPoint(-1, -1), wxSize(panelWidth, 20), wxALL | wxEXPAND);
		m_ProducerName_textCtrl->SetEditable(true);
		m_ProducerName_textCtrl->SetMaxLength(64);
		labelSizer1->Add(m_ProducerName_textCtrl, 0, wxALL | wxEXPAND, 0);
		infoBoxSizer->Add(labelSizer1, 0, wxALL | wxEXPAND, 5);

		// TEXT - Producer Web Site
		wxStaticBoxSizer *labelSizer2 = new wxStaticBoxSizer(wxVERTICAL, this, "Web Site");
		m_ProducerSite_textCtrl = new wxTextCtrl(this, ID_PRODUCER_DIALOG_TEXT, m_CurrentProducer.webSite, wxPoint(-1, -1), wxSize(panelWidth, 20), wxALL | wxEXPAND);
		m_ProducerSite_textCtrl->SetEditable(true);
		m_ProducerSite_textCtrl->SetMaxLength(64);
		labelSizer2->Add(m_ProducerSite_textCtrl, 0, wxALL | wxEXPAND, 0);
		infoBoxSizer->Add(labelSizer2, 0, wxALL | wxEXPAND, 5);

		// TEXT - Empty Separator
		infoBoxSizer->Add(new albaGUILab(this, -1, " "), 0, wxALIGN_LEFT, 5);

		mainBoxSizer->Add(infoBoxSizer, 0, wxALL, 5);

		// BUTTON - Ok
		albaGUIButton *okBtn = new albaGUIButton(this, ID_PRODUCER_DIALOG_OK_PRESSED, "OK", wxPoint(-1, -1));
		okBtn->SetListener(this);
		mainBoxSizer->Add(okBtn, 0, wxALIGN_RIGHT, 0);

		//////////////////////////////////////////////////////////////////////////
		//m_Gui->Add(mainBoxSizer, 0, wxALL, 5);
		m_Gui->Fit();

		// Show dialog
		wxSize s = albaGetFrame()->GetSize();
		wxPoint p = albaGetFrame()->GetPosition();

		SetSize(430, 355);

		int posX = p.x + s.GetWidth() * .5 - this->GetSize().GetWidth() * .5;
		int posY = p.y + s.GetHeight() * .5 - this->GetSize().GetHeight() * .5;

		this->SetPosition(wxPoint(posX, posY));

		Add(m_Gui, 1);
		Add(mainBoxSizer, 0);
	}

	UpdateProducerDialog();
}

//----------------------------------------------------------------------------
void appGUIDialogProducer::UpdateProducerDialog()
{
	if (m_Gui)
	{
		// Update Image
		if (wxFileExists(m_CurrentProducer.brandImage))
		{
			m_ProducerImage->LoadFile(m_CurrentProducer.brandImage.c_str(), wxBITMAP_TYPE_ANY);
			m_ProducerImageButton->SetBitmapSelected(wxBitmap(*m_ProducerImage));
			m_ProducerImageButton->Update();
			m_Gui->Update();
		}

		m_ProducerName_textCtrl->SetValue(m_CurrentProducer.name);
		m_ProducerSite_textCtrl->SetValue(m_CurrentProducer.webSite);
	}
}
