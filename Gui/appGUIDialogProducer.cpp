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
#include "albaServiceClient.h"
#include "appLogic.h"

enum PRODUCER_DIALOG_ID
{
	ID_PRODUCER_DIALOG_TEXT = MINID,
	ID_PRODUCER_DIALOG_IMM,
	ID_PRODUCER_DIALOG_LINK,
	ID_PRODUCER_DIALOG_OK_PRESSED,
};

//----------------------------------------------------------------------------
appGUIDialogProducer::appGUIDialogProducer(const wxString& title, long style)
	: albaGUIDialog(title, style)
{
	m_Gui = NULL;
	m_MainBoxSizer = NULL;
	m_ImageSizer = NULL;

	m_ProducerName_textCtrl = NULL;
	m_ProducerSite_textCtrl = NULL;
	m_ProducerImageButton = NULL;

	m_ProducerName = "";
	m_ProducerWebSite = "";
	m_ProducerImageName = "";
	m_ProducerImageFullName = "";
	m_IsChanged = false;

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
		m_ProducerName = m_ProducerName_textCtrl->GetValue();
		m_ProducerWebSite = m_ProducerSite_textCtrl->GetValue();
		m_IsChanged = true;

		UpdateProducerDialog();
	}
	break;

	case ID_PRODUCER_DIALOG_IMM:
	{
		SelectImage();
	}
	break;

	case ID_PRODUCER_DIALOG_OK_PRESSED:
	{
		m_ProducerName = m_ProducerName_textCtrl->GetValue();
		m_ProducerWebSite = m_ProducerSite_textCtrl->GetValue();

		m_CurrentProducer->SetName(m_ProducerName);
		m_CurrentProducer->SetWebSite(m_ProducerWebSite);
		m_CurrentProducer->SetImgFileName(m_ProducerImageName);
		m_IsChanged = true;

		this->Close();
	}
	break;

	default:
		albaGUIDialog::OnEvent(alba_event);
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
	if (m_Gui == NULL)
	{
		int panelWidth = 400;

		m_Gui = new albaGUI(this);
		//////////////////////////////////////////////////////////////////////////

		m_MainBoxSizer = new wxBoxSizer(wxVERTICAL);
		
		wxString imagesPath = appUtils::GetConfigDirectory().c_str();
		wxString imgPath = imagesPath + "/Wizard/Producer.bmp"; // Default

		if (wxFileExists(m_ProducerImageFullName))
		{
			imgPath = m_ProducerImageFullName;
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

			// BUTTON - Change Producer Brand Image
			albaGUIButton *immBtn = new albaGUIButton(this, ID_PRODUCER_DIALOG_IMM, "Change Image", wxPoint(-1, -1));
			immBtn->SetListener(this);
			m_MainBoxSizer->Add(immBtn, 0, wxALIGN_RIGHT, 0);

			delete previewBitmap;
			delete previewImage;
		}

		//////////////////////////////////////////////////////////////////////////
		// Info Sizer
		wxBoxSizer *infoBoxSizer = new wxBoxSizer(wxVERTICAL);

		// TEXT - Producer Name
		wxStaticBoxSizer *labelSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, "Producer Name");
		m_ProducerName_textCtrl = new wxTextCtrl(this, ID_PRODUCER_DIALOG_TEXT, *m_ProducerName, wxPoint(-1, -1), wxSize(panelWidth, 20), wxALL | wxEXPAND);
		m_ProducerName_textCtrl->SetValidator(albaGUIValidator(this, ID_PRODUCER_DIALOG_TEXT, m_ProducerName_textCtrl, &m_ProducerName, true));
		m_ProducerName_textCtrl->SetEditable(true);
		m_ProducerName_textCtrl->SetMaxLength(64);
		labelSizer1->Add(m_ProducerName_textCtrl, 0, wxALL | wxEXPAND, 0);

		infoBoxSizer->Add(labelSizer1, 0, wxALL | wxEXPAND, 5);

		// TEXT - Producer Web Site
		wxStaticBoxSizer *labelSizer2 = new wxStaticBoxSizer(wxVERTICAL, this, "Web Site");
		m_ProducerSite_textCtrl = new wxTextCtrl(this, ID_PRODUCER_DIALOG_TEXT, *m_ProducerWebSite, wxPoint(-1, -1), wxSize(panelWidth, 20), wxALL | wxEXPAND);
		m_ProducerSite_textCtrl->SetEditable(true);
		m_ProducerSite_textCtrl->SetMaxLength(64);
		labelSizer2->Add(m_ProducerSite_textCtrl, 0, wxALL | wxEXPAND, 0);

		infoBoxSizer->Add(labelSizer2, 0, wxALL | wxEXPAND, 5);

		// TEXT - Empty Separator
		infoBoxSizer->Add(new albaGUILab(this, -1, " "), 0, wxALIGN_LEFT, 5);

		m_MainBoxSizer->Add(infoBoxSizer, 0, wxALL, 5);

		// BUTTON - Ok
		m_OkBtn = new albaGUIButton(this, ID_PRODUCER_DIALOG_OK_PRESSED, "OK", wxPoint(-1, -1));
		m_OkBtn->SetListener(this);
		m_MainBoxSizer->Add(m_OkBtn, 0, wxALIGN_RIGHT, 0);

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

	UpdateProducerDialog();
}
//----------------------------------------------------------------------------
void appGUIDialogProducer::UpdateProducerDialog()
{
	if (m_Gui)
	{
		bool hasName = !m_ProducerName_textCtrl->GetValue().IsEmpty();
		m_OkBtn->Enable(hasName);

		// Update Image
		if (wxFileExists(m_ProducerImageFullName))
		{
			if (m_ProducerImageButton != NULL)
			{
				m_ImageSizer->Remove(m_ProducerImageButton);
				delete m_ProducerImageButton;
			}

			// Load and show the image
			wxImage *previewImage = new wxImage();
			previewImage->LoadFile(m_ProducerImageFullName, wxBITMAP_TYPE_ANY);

			wxBitmap *previewBitmap;
			previewBitmap = new wxBitmap(previewImage);

			m_ProducerImageButton = new albaGUIPicButton(this, previewBitmap, -1);
			m_ImageSizer->Add(m_ProducerImageButton);
			
			m_ImageSizer->Fit(this);
			m_MainBoxSizer->Fit(this);
			m_Gui->Fit();

			delete previewBitmap;
			delete previewImage;
		}

		m_Gui->Update();
	}
}

//----------------------------------------------------------------------------
void appGUIDialogProducer::SetProducer(albaProDBProducer *producer)
{
	if (producer)
	{
		wxString DBDir = ((appLogic*)GetLogicManager())->GetProsthesesDBManager()->GetDBDir();

		m_CurrentProducer = producer;

		m_ProducerName = m_CurrentProducer->GetName();
		m_ProducerWebSite = m_CurrentProducer->GetWebSite();
		m_ProducerImageName = m_CurrentProducer->GetImgFileName();
		m_ProducerImageFullName = DBDir + m_CurrentProducer->GetImgFileName();
		m_IsChanged = false;

		m_ProducerName_textCtrl->SetValue(m_ProducerName);
		m_ProducerSite_textCtrl->SetValue(m_ProducerWebSite);
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
		m_ProducerImageFullName = imagePath;		
		m_ProducerImageName = wxFileNameFromPath(m_ProducerImageFullName);
		m_IsChanged = true;

		// Copy Image in DB Directory
		wxString DBDir = ((appLogic*)GetLogicManager())->GetProsthesesDBManager()->GetDBDir();
		wxCopyFile(m_ProducerImageFullName, DBDir + m_ProducerImageName);

		UpdateProducerDialog();
	}
}