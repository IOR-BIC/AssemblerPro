/*=========================================================================
Program:   AssemblerPro
Module:    appGUIDialogProsthesisSelection.cpp
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

#include "appGUIDialogProsthesisSelection.h"
#include "appUtils.h"

#include "albaDecl.h"
#include "albaGUIButton.h"
#include "appGUIHyperLink.h"
#include "albaGUILab.h"
#include "albaGUIPicButton.h"
#include "albaGUIValidator.h"

#include "wx\image.h"
#include "wx\statline.h"
#include "wx\window.h"

enum PRODUCER_DIALOG_ID
{
	ID_PRODUCER_DIALOG_TEXT = MINID,
	ID_PRODUCER_DIALOG_IMM,
	ID_PRODUCER_DIALOG_LINK,
	ID_PRODUCER_DIALOG_OK_PRESSED,
};

//----------------------------------------------------------------------------
appGUIDialogProsthesisSelection::appGUIDialogProsthesisSelection(const wxString& title, long style)
	: albaGUIDialog(title, style)
{
	m_Gui = NULL;

	m_MainBoxSizer = NULL;

	m_ImageSizer = NULL;
	m_ProducerImageButton = NULL;

	m_ProducerName_textCtrl = NULL;
	
	m_CurrentProducer.name = "Producer";
	m_CurrentProducer.webSite = "www.producer.com";
	m_CurrentProducer.image = "producer.bmp";

	CreateDialog();
}
//----------------------------------------------------------------------------
appGUIDialogProsthesisSelection::~appGUIDialogProsthesisSelection()
{
}

//----------------------------------------------------------------------------
void appGUIDialogProsthesisSelection::OnEvent(albaEventBase *alba_event)
{
	switch (alba_event->GetId())
	{
	case ID_PRODUCER_DIALOG_TEXT:
	{
		m_CurrentProducer.name = m_ProducerName_textCtrl->GetValue();
		m_CurrentProducer.isChanged = true;
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
		this->Close();
	}
	break;

	default:
		albaGUIDialog::OnEvent(alba_event);
	}
}

//----------------------------------------------------------------------------
void appGUIDialogProsthesisSelection::SelectImage()
{
	albaString fileNameFullPath = albaGetDocumentsDirectory().c_str();
	albaString wildc = "Image file (*.bmp)|*.bmp";
	wxString imagePath = albaGetOpenFile(fileNameFullPath.GetCStr(), wildc, "Select file").c_str();

	if (wxFileExists(imagePath))
	{		
		m_CurrentProducer.image = imagePath;
		m_CurrentProducer.isChanged = true;

		UpdateDialog();
	}
}

//----------------------------------------------------------------------------
void appGUIDialogProsthesisSelection::Show()
{
	CreateDialog();
	ShowModal();
}

//----------------------------------------------------------------------------
void appGUIDialogProsthesisSelection::CreateDialog()
{
	if (m_Gui == NULL)
	{
		m_Gui = new albaGUI(this);
		//////////////////////////////////////////////////////////////////////////

		m_MainBoxSizer = new wxBoxSizer(wxVERTICAL);

		//
		int panelWidth = 400;

		wxString imagesPath = appUtils::GetConfigDirectory().c_str();
		wxString imgPath = imagesPath + "/Wizard/Producer.bmp";

		if (wxFileExists(m_CurrentProducer.image))
		{
			imgPath = m_CurrentProducer.image;
// 			imgPath += imagesPath + "\\Config\\" + m_CurrentProducer.brandImage;
// 			imgPath += ".bmp";
		}

		if (wxFileExists(imgPath))
		{
			// IMAGE Producer
			m_ImageSizer = new wxBoxSizer(wxVERTICAL);

			wxImage *previewImage = new wxImage();
			previewImage->LoadFile(imgPath.c_str(), wxBITMAP_TYPE_ANY);

			wxBitmap *previewBitmap = new wxBitmap(*previewImage);
			m_ProducerImageButton = new albaGUIPicButton(this, previewBitmap, -1);

			panelWidth = previewImage->GetWidth();

			m_ImageSizer->Add(m_ProducerImageButton);

			m_MainBoxSizer->Add(m_ImageSizer, 0, wxALL | wxALIGN_CENTER, 0);

			delete previewBitmap;
			delete previewImage;
		}

		//////////////////////////////////////////////////////////////////////////
		// Info Sizer
		wxBoxSizer *infoBoxSizer = new wxBoxSizer(wxVERTICAL);

		// TEXT - Producer Name
		wxStaticBoxSizer *labelSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, "Producer");
		m_ProducerName_textCtrl = new wxTextCtrl(this, ID_PRODUCER_DIALOG_TEXT, m_CurrentProducer.name, wxPoint(-1, -1), wxSize(300, 20), wxALL | wxEXPAND);
		m_ProducerName_textCtrl->SetEditable(false);
		m_ProducerName_textCtrl->Enable(false);
		m_ProducerName_textCtrl->SetMaxLength(64);
		labelSizer1->Add(m_ProducerName_textCtrl, 0, wxALL | wxEXPAND, 0);

		labelSizer1->Add(new albaGUILab(this, -1, "   "));

		// LINK - Producer Web Site
		appGUIHyperLink *link = new appGUIHyperLink(this, NULL, m_CurrentProducer.webSite);
		link->SetUrl(m_CurrentProducer.webSite);
		labelSizer1->Add(link, 0, wxALL | wxEXPAND, 0);

		infoBoxSizer->Add(labelSizer1, 0, wxALL | wxEXPAND, 5);

		//////////////////////////////////////////////////////////////////////////
		// TEXT - Empty Separator
		infoBoxSizer->Add(new albaGUILab(this, -1, " "), 0, wxALIGN_LEFT, 5);

		m_MainBoxSizer->Add(infoBoxSizer, 0, wxALL, 5);

		// LINE
		m_MainBoxSizer->Add(new wxStaticLine(this, -1, wxPoint(-1, -1), wxSize(430, 1)), 0, wxALL, 5);

		// BUTTON - Ok
		albaGUIButton *okBtn = new albaGUIButton(this, ID_PRODUCER_DIALOG_OK_PRESSED, "OK", wxPoint(-1, -1));
		okBtn->SetListener(this);
		m_MainBoxSizer->Add(okBtn, 0, wxALIGN_RIGHT, 0);

		//////////////////////////////////////////////////////////////////////////

		m_Gui->Fit();

		// Show dialog
		wxSize s = albaGetFrame()->GetSize();
		wxPoint p = albaGetFrame()->GetPosition();

		SetSize(430, 355);

		int posX = p.x + s.GetWidth() * .5 - this->GetSize().GetWidth() * .5;
		int posY = p.y + s.GetHeight() * .5 - this->GetSize().GetHeight() * .5;

		this->SetPosition(wxPoint(posX, posY));

		Add(m_Gui, 1);
		Add(m_MainBoxSizer, 0);
	}

	UpdateDialog();
}

//----------------------------------------------------------------------------
void appGUIDialogProsthesisSelection::UpdateDialog()
{
	if (m_Gui)
	{
		// wxString imagePath = appUtils::GetConfigDirectory().c_str();
		// imagePath += "\\Config\\" + m_CurrentProducer.brandImage;
		// imagePath += ".bmp";

		wxString imagePath = m_CurrentProducer.image;

		// Update Image
		if (wxFileExists(imagePath))
		{
			if (m_ProducerImageButton != NULL)
			{
				m_ImageSizer->Remove(m_ProducerImageButton);
				delete m_ProducerImageButton;
			}

			// Load and show the image
			wxImage *previewImage = new wxImage();
			previewImage->LoadFile(imagePath.c_str(), wxBITMAP_TYPE_ANY);

			wxBitmap *previewBitmap;

			// if (flipImage)
			// previewBitmap = new wxBitmap(previewImage->Mirror(true));
			// else
			previewBitmap = new wxBitmap(previewImage);

			m_ProducerImageButton = new albaGUIPicButton(this, previewBitmap, -1);
			m_ImageSizer->Add(m_ProducerImageButton);
			
			m_ImageSizer->Fit(this);
			m_MainBoxSizer->Fit(this);
			m_Gui->Fit();

			delete previewBitmap;
			delete previewImage;
		}

		m_ProducerName_textCtrl->SetValue(m_CurrentProducer.name);
	}
}