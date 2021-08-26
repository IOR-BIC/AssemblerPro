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
#include "appGUIDialogProducer.h"
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
	ID_DIALOG_NAME = MINID,
	ID_DIALOG_PRODUCER,
	ID_DIALOG_PRODUCER_EDIT,
	ID_DIALOG_PRODUCER_ADD,
	ID_DIALOG_PRODUCER_REM,
	ID_DIALOG_TYPE,
	ID_DIALOG_TYPE_EDIT,
	ID_DIALOG_TYPE_ADD,
	ID_DIALOG_TYPE_REM,
	ID_DIALOG_SIDE,
	ID_DIALOG_IMM,
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
	m_NameTextCtrl = NULL;
	m_ProducerComboBox = NULL;
	m_TypeComboBox = NULL;

	m_DBManager = NULL;

	Init();
	CreateDialog();
}
//----------------------------------------------------------------------------
appGUIDialogProsthesis::~appGUIDialogProsthesis()
{
	m_ProducerNameList.clear();
	m_ProducersVect.clear();
	m_TypeNameList.clear();
}

//----------------------------------------------------------------------------
void appGUIDialogProsthesis::Init()
{
	albaProsthesesDBManager *DBManager = ((appLogic*)GetLogicManager())->GetProsthesesDBManager();

	// Load Producers Info
	std::vector<albaProDBProducer *> DBproducers = DBManager->GetProducers();

	m_ProducerNameList.clear();
	m_ProducersVect.clear();
	if (DBproducers.size() > 0)
	{
		for (int p = 0; p < DBproducers.size(); p++)
		{
			m_ProducerNameList.push_back(DBproducers[p]->GetName().GetCStr());

			AuxProducer producer;
			producer.index = p;
			producer.name = DBproducers[p]->GetName();
			producer.webSite = DBproducers[p]->GetWebSite();
			producer.image = DBproducers[p]->GetImgFileName();

			m_ProducersVect.push_back(producer);
		}
	}
	else
	{
		m_ProducerNameList.push_back("None");
	}
	m_SelectedProducer = 0;

	// Load Types Info
	std::vector<albaProDBType *> DBTypes = DBManager->GetTypes();

	m_TypeNameList.clear();

	if (DBTypes.size() > 0)
	{
		for (int t = 0; t < DBTypes.size(); t++)
		{
			m_TypeNameList.push_back(DBTypes[t]->GetName().GetCStr());
		}
	}
	else
	{
		m_TypeNameList.push_back("None");
	}

	m_SelectedType = 0;

	// Side
	m_SelectedSide = 0;
}

//----------------------------------------------------------------------------
void appGUIDialogProsthesis::OnEvent(albaEventBase *alba_event)
{
	switch (alba_event->GetId())
	{
	case ID_DIALOG_NAME: break;
	case ID_DIALOG_IMM: SelectImage(); break;
	case ID_DIALOG_PRODUCER: break;
	case ID_DIALOG_PRODUCER_EDIT: EditProducer(); break;
	case ID_DIALOG_PRODUCER_ADD: AddProducer(); break;
	case ID_DIALOG_PRODUCER_REM: RemoveProducer(); break;
	case ID_DIALOG_TYPE: break;
	case ID_DIALOG_TYPE_EDIT: EditType(); break;
	case ID_DIALOG_TYPE_ADD: AddType(); break;
	case ID_DIALOG_TYPE_REM: RemoveType(); break;
	case ID_DIALOG_SIDE: break;

	case ID_DIALOG_OK_PRESSED:
	{
		// Update Prosthesis
		m_CurrentProsthesis.name = m_NameTextCtrl->GetValue();
		m_CurrentProsthesis.image = m_ImagePath;
		m_CurrentProsthesis.producer = m_ProducerNameList[m_SelectedProducer];
		m_CurrentProsthesis.type = m_TypeNameList[m_SelectedType];
		m_CurrentProsthesis.side = m_SelectedSide;

		m_CurrentProsthesis.componentGroup.clear();
		for (int c = 0; c < m_TypeNameList.size(); c++)
			m_CurrentProsthesis.componentGroup.push_back(m_TypeNameList[c]);

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
		m_NameTextCtrl = new wxTextCtrl(this, ID_DIALOG_NAME, m_CurrentProsthesis.name, wxPoint(-1, -1), wxSize(panelWidth, 20), wxALL | wxEXPAND);
		m_NameTextCtrl->SetEditable(true);
		m_NameTextCtrl->SetMaxLength(64);
		labelSizer1->Add(m_NameTextCtrl, 0, wxALL | wxEXPAND, 0);

		infoBoxSizer->Add(labelSizer1, 0, wxALL | wxEXPAND, 5);

		//////////////////////////////////////////////////////////////////////////

		wxStaticBoxSizer *producerBoxSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, "Producer");

		// COMBO - Producer
		m_ProducerComboBox = new wxComboBox(this, ID_DIALOG_PRODUCER, "", wxPoint(-1, -1), wxSize(panelWidth, 20), m_ProducerNameList.size(), &m_ProducerNameList[0]);
		m_ProducerComboBox->SetValidator(albaGUIValidator(this, ID_DIALOG_PRODUCER, m_ProducerComboBox, &m_SelectedProducer));
		producerBoxSizer->Add(m_ProducerComboBox, 0, wxALL | wxEXPAND, 0);

		wxBoxSizer *producerBtnSizer = new wxBoxSizer(wxHORIZONTAL);

		// BUTTON - Producer Edit
		albaGUIButton *producerEditBtn = new albaGUIButton(this, ID_DIALOG_PRODUCER_EDIT, "Edit", wxPoint(-1, -1), wxSize(80, 23));
		producerEditBtn->SetListener(this);
		producerBtnSizer->Add(producerEditBtn, 0, wxALIGN_RIGHT, 5);

		// BUTTON - Producer Add
		albaGUIButton *producerAddBtn = new albaGUIButton(this, ID_DIALOG_PRODUCER_ADD, "+", wxPoint(-1, -1), wxSize(50, 23));
		producerAddBtn->SetListener(this);
		producerBtnSizer->Add(producerAddBtn, 0, wxALIGN_RIGHT, 5);

		// BUTTON - Producer Rem
		albaGUIButton *producerRemBtn = new albaGUIButton(this, ID_DIALOG_PRODUCER_REM, "-", wxPoint(-1, -1), wxSize(50, 23));
		producerRemBtn->SetListener(this);
		producerBtnSizer->Add(producerRemBtn, 0, wxALIGN_RIGHT, 5);

		producerBoxSizer->Add(producerBtnSizer, 0, wxALL | wxEXPAND);

		infoBoxSizer->Add(producerBoxSizer, 0, wxALL | wxEXPAND, 5);
		
		//////////////////////////////////////////////////////////////////////////

		wxStaticBoxSizer *typeBoxSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, "Type");

		// COMBO - Component Type
		m_TypeComboBox = new wxComboBox(this, ID_DIALOG_TYPE, "", wxPoint(-1, -1), wxSize(panelWidth, 20), m_TypeNameList.size(), &m_TypeNameList[0]);
		m_TypeComboBox->SetValidator(albaGUIValidator(this, ID_DIALOG_TYPE, m_TypeComboBox, &m_SelectedType));
		typeBoxSizer->Add(m_TypeComboBox, 0, wxALL | wxEXPAND, 0);

		// BUTTON - Component Type Edit
		albaGUIButton *componentTypeEditBtn = new albaGUIButton(this, ID_DIALOG_TYPE_EDIT, "Edit", wxPoint(-1, -1), wxSize(80, 23));
		componentTypeEditBtn->SetListener(this);
		typeBoxSizer->Add(componentTypeEditBtn, 0, wxALIGN_RIGHT, 5);

		// BUTTON - Component Type Add
		albaGUIButton *componentTypeAddBtn = new albaGUIButton(this, ID_DIALOG_TYPE_ADD, "+", wxPoint(-1, -1), wxSize(50, 23));
		componentTypeAddBtn->SetListener(this);
		typeBoxSizer->Add(componentTypeAddBtn, 0, wxALIGN_RIGHT, 5);

		// BUTTON - Component Type Del
		albaGUIButton *componentTypeDelBtn = new albaGUIButton(this, ID_DIALOG_TYPE_REM, "-", wxPoint(-1, -1), wxSize(50, 23));
		componentTypeDelBtn->SetListener(this);
		typeBoxSizer->Add(componentTypeDelBtn, 0, wxALIGN_RIGHT, 5);

		infoBoxSizer->Add(typeBoxSizer, 0, wxALL | wxEXPAND, 5);

		//////////////////////////////////////////////////////////////////////////
		wxBoxSizer *sideBoxSizer = new wxBoxSizer(wxHORIZONTAL);

		// RADIO - Side
		wxString sideChoices[]{ "Left","Right","Both" };
		wxRadioBox *sideRadioBox = new wxRadioBox(this, ID_DIALOG_SIDE, "Side", wxPoint(-1, -1), wxSize(-1, -1), 3, sideChoices);
		sideRadioBox->SetValidator(albaGUIValidator(this, ID_DIALOG_SIDE, sideRadioBox, &m_SelectedSide));
		sideBoxSizer->Add(sideRadioBox, 0, wxALL | wxEXPAND, 5);

		infoBoxSizer->Add(sideBoxSizer, 0, wxALL | wxEXPAND, 0);

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

		m_NameTextCtrl->SetValue(m_CurrentProsthesis.name);
	}
}

//----------------------------------------------------------------------------
void appGUIDialogProsthesis::EditType()
{
	wxTextEntryDialog *dlg = new wxTextEntryDialog(this, "Name", "Type", m_TypeNameList[m_SelectedType]);
	int result = dlg->ShowModal();
	wxString name = dlg->GetValue();

	if (!name.IsEmpty())
	{
		m_TypeNameList[m_SelectedType] = name;
		m_TypeComboBox->SetString(m_SelectedType, name);
		m_TypeComboBox->Select(m_SelectedType);
		m_TypeComboBox->Update();
	}
}
//----------------------------------------------------------------------------
void appGUIDialogProsthesis::AddType()
{
	wxTextEntryDialog *dlg = new wxTextEntryDialog(this, "Name", "Type", "");
	int result = dlg->ShowModal();
	wxString name = dlg->GetValue();

	if (!name.IsEmpty())
	{
		m_TypeNameList.push_back(name);
		m_SelectedType = m_TypeNameList.size() - 1;
		m_TypeComboBox->Insert(name, m_SelectedType);
		m_TypeComboBox->Select(m_SelectedType);
	}

	m_Gui->Enable(ID_DIALOG_TYPE_REM, m_TypeNameList.size() > 1);
}
//----------------------------------------------------------------------------
void appGUIDialogProsthesis::RemoveType()
{
	if (m_TypeNameList.size() > 1 && m_SelectedType < m_TypeNameList.size())
	{
		m_TypeNameList.erase(m_TypeNameList.begin() + m_SelectedType);
		m_TypeComboBox->Delete(m_SelectedType);

		m_SelectedType--;

		m_TypeComboBox->Select(m_SelectedType);
		m_TypeComboBox->Update();
	}

	m_Gui->Enable(ID_DIALOG_TYPE_REM, m_TypeNameList.size() > 1);
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
void appGUIDialogProsthesis::SetProsthesis(AuxProsthesis &prosthesis)
{
	m_CurrentProsthesis = prosthesis;

	m_NameTextCtrl->SetValue(m_CurrentProsthesis.name);
	m_ImagePath = m_CurrentProsthesis.image;

	m_SelectedProducer = 0;
	for (int p = 0; p < m_ProducerNameList.size(); p++)
		if (m_ProducerNameList[p] == m_CurrentProsthesis.producer)
			m_SelectedProducer = p;

	m_SelectedType = 0;
	for (int t = 0; t < m_TypeNameList.size(); t++)
		if (m_TypeNameList[t] == m_CurrentProsthesis.type)
			m_SelectedType = t;

	m_SelectedSide = m_CurrentProsthesis.side;
}

//----------------------------------------------------------------------------
void appGUIDialogProsthesis::EditProducer()
{
	m_CurrentProducer.index = m_ProducersVect[m_SelectedProducer].index;
	m_CurrentProducer.name = m_ProducersVect[m_SelectedProducer].name;
	m_CurrentProducer.webSite = m_ProducersVect[m_SelectedProducer].webSite;
	m_CurrentProducer.image = m_ProducersVect[m_SelectedProducer].image;

	appGUIDialogProducer pd(_("Edit Producer"));
	pd.SetProducer(m_CurrentProducer);
	pd.Show();

	UpdateProducer(pd.GetProducer());
}
//----------------------------------------------------------------------------
void appGUIDialogProsthesis::AddProducer()
{
	// Create New Producer
	m_CurrentProducer.name = wxString::Format("newProducer(%d)", m_ProducerNameList.size());
	m_CurrentProducer.webSite = "";
	m_CurrentProducer.image = "";

	// Add to Combobox
	m_ProducerNameList.push_back(m_CurrentProducer.name);
	if (m_ProducerComboBox)
		m_ProducerComboBox->Append(m_CurrentProducer.name);

	// Add to Vector
	AuxProducer producer;
	producer.index = -1;
	producer.name = m_CurrentProducer.name;
	producer.webSite = m_CurrentProducer.webSite;
	producer.image = m_CurrentProducer.image;
	m_ProducersVect.push_back(producer);

	// Select
	m_SelectedProducer = m_ProducerNameList.size() - 1;

	if (m_ProducerComboBox)
		m_ProducerComboBox->Select(m_SelectedProducer);

	//SelectProducer();

	EditProducer();
}
//----------------------------------------------------------------------------
void appGUIDialogProsthesis::RemoveProducer()
{

}
//----------------------------------------------------------------------------
void appGUIDialogProsthesis::UpdateProducer(AuxProducer producer)
{
	//////////////////////////////////////////////////////////////////////////

	// Update Combobox
	m_ProducerNameList[m_SelectedProducer] = producer.name;

	if (m_ProducerComboBox)
		m_ProducerComboBox->SetString(m_SelectedProducer, producer.name);

	// Update Vector Element
	m_ProducersVect[m_SelectedProducer].name = producer.name;
	m_ProducersVect[m_SelectedProducer].webSite = producer.webSite;
	m_ProducersVect[m_SelectedProducer].image = producer.image;
	m_ProducersVect[m_SelectedProducer].isChanged = true;

	//UpdateGui();
}