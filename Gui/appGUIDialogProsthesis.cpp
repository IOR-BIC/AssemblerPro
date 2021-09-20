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
#include "albaProsthesesDBManager.h"
#include "albaServiceClient.h"

#include "wx\statline.h"
#include "wx\image.h"
#include "wx\combobox.h"
#include "wx\window.h"
#include <vector>

enum PROSTHESIS_DIALOG_ID
{
	ID_DIALOG_NAME = MINID,
	ID_DIALOG_IMM,
	ID_DIALOG_PRODUCER,
	ID_DIALOG_PRODUCER_ADD,
	ID_DIALOG_PRODUCER_DEL,
	ID_DIALOG_PRODUCER_EDIT,
	ID_DIALOG_TYPE,
	ID_DIALOG_TYPE_ADD,
	ID_DIALOG_TYPE_DEL,
	ID_DIALOG_TYPE_EDIT,
	ID_DIALOG_SIDE,
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

	m_DBManager = ((appLogic*)GetLogicManager())->GetProsthesesDBManager();

	m_CurrentProsthesis = NULL;

	m_ProsthesisName = "";
	m_ProsthesisProducerName = "";
	m_ProsthesisImageName = "";
	m_ProsthesisImageFullName = "";
	m_ProsthesisType = "";
	m_ProsthesisSide = albaProDBProsthesis::PRO_LEFT;
	m_IsChanged = false;

	m_SelectedProducer = -1;
	m_SelectedSide = -1;
	m_SelectedType = -1;

	Init();
	CreateDialog();
}
//----------------------------------------------------------------------------
appGUIDialogProsthesis::~appGUIDialogProsthesis()
{
	m_ProducerNameList.clear();
	m_TypeNameList.clear();
}

//----------------------------------------------------------------------------
void appGUIDialogProsthesis::OnEvent(albaEventBase *alba_event)
{
	switch (alba_event->GetId())
	{
	case ID_DIALOG_NAME: UpdateDialog(); break;
	case ID_DIALOG_IMM: SelectImage(); break;

	case ID_DIALOG_PRODUCER: break;
	case ID_DIALOG_PRODUCER_EDIT: EditProducer(); break;
	case ID_DIALOG_PRODUCER_ADD: AddProducer(); break;
	case ID_DIALOG_PRODUCER_DEL: RemoveProducer(); break;

	case ID_DIALOG_TYPE: break;
	case ID_DIALOG_TYPE_EDIT: EditType(); break;
	case ID_DIALOG_TYPE_ADD: AddType(); break;
	case ID_DIALOG_TYPE_DEL: RemoveType(); break;
	case ID_DIALOG_SIDE: break;

	case ID_DIALOG_OK_PRESSED:
	{
		m_ProsthesisName = m_NameTextCtrl->GetValue();
		m_ProsthesisSide = (albaProDBProsthesis::PRO_SIDES)m_SelectedSide;

		// Update Prosthesis
		m_CurrentProsthesis->SetName(m_ProsthesisName);
		m_CurrentProsthesis->SetImgFileName(m_ProsthesisImageName);
		m_CurrentProsthesis->SetProducer(m_ProducerNameList[m_SelectedProducer]);
		m_CurrentProsthesis->SetType(m_TypeNameList[m_SelectedType]);
		m_CurrentProsthesis->SetSide(m_ProsthesisSide);
		m_IsChanged = true;

		this->Close();
	}
	break;
	case ID_DIALOG_CANCEL_PRESSED:
		m_IsChanged = false;
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

		int dialogWidth = 300;
		int imageWidth = 0;

		m_MainBoxSizer = new wxBoxSizer(wxVERTICAL);
		wxBoxSizer *mainBoxSizer2 = new wxBoxSizer(wxVERTICAL);
		wxBoxSizer *mainBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);

		wxString imagesPath = appUtils::GetConfigDirectory().c_str();
		wxString imgPath = imagesPath + "/Wizard/Model.bmp"; // Default

		if (wxFileExists(m_ProsthesisImageFullName))
		{
			imgPath = m_ProsthesisImageFullName;
		}

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
		m_NameTextCtrl = new wxTextCtrl(this, ID_DIALOG_NAME, m_ProsthesisName, wxPoint(-1, -1), wxSize(panelWidth, 20), wxALL | wxEXPAND);
		m_NameTextCtrl->SetValidator(albaGUIValidator(this, ID_DIALOG_NAME, m_NameTextCtrl, &m_ProsthesisName, true));
		m_NameTextCtrl->SetEditable(true);
		m_NameTextCtrl->SetMaxLength(64);
		labelSizer1->Add(m_NameTextCtrl, 0, wxALL | wxEXPAND, 0);

		infoBoxSizer->Add(labelSizer1, 0, wxALL | wxEXPAND, 5);

		//////////////////////////////////////////////////////////////////////////

		wxStaticBoxSizer *producerBoxSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, "Producer");

		// COMBO - Producer
		m_ProducerComboBox = new wxComboBox(this, ID_DIALOG_PRODUCER, "", wxPoint(-1, -1), wxSize(panelWidth, 20));
		m_ProducerComboBox->SetValidator(albaGUIValidator(this, ID_DIALOG_PRODUCER, m_ProducerComboBox, &m_SelectedProducer));
		producerBoxSizer->Add(m_ProducerComboBox, 0, wxALL | wxEXPAND, 0);

		wxBoxSizer *producerBtnSizer = new wxBoxSizer(wxHORIZONTAL);

		// BUTTON - Producer Add
		m_ProducerAddBtn = new albaGUIButton(this, ID_DIALOG_PRODUCER_ADD, "+", wxPoint(-1, -1), wxSize(50, 23));
		m_ProducerAddBtn->SetListener(this);
		producerBtnSizer->Add(m_ProducerAddBtn, 0, wxALIGN_RIGHT, 5);

		// BUTTON - Producer Rem
		m_ProducerDelBtn = new albaGUIButton(this, ID_DIALOG_PRODUCER_DEL, "-", wxPoint(-1, -1), wxSize(50, 23));
		m_ProducerDelBtn->SetListener(this);
		producerBtnSizer->Add(m_ProducerDelBtn, 0, wxALIGN_RIGHT, 5);

		// BUTTON - Producer Edit
		m_ProducerEditBtn = new albaGUIButton(this, ID_DIALOG_PRODUCER_EDIT, "Edit", wxPoint(-1, -1), wxSize(80, 23));
		m_ProducerEditBtn->SetListener(this);
		producerBtnSizer->Add(m_ProducerEditBtn, 0, wxALIGN_RIGHT, 5);

		producerBoxSizer->Add(producerBtnSizer, 0, wxALL | wxEXPAND);

		infoBoxSizer->Add(producerBoxSizer, 0, wxALL | wxEXPAND, 5);
		
		//////////////////////////////////////////////////////////////////////////

		wxStaticBoxSizer *typeBoxSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, "Type");

		// COMBO - Type
		m_TypeComboBox = new wxComboBox(this, ID_DIALOG_TYPE, "", wxPoint(-1, -1), wxSize(panelWidth, 20));// , m_TypeNameList.size(), m_TypeNameList);
		m_TypeComboBox->SetValidator(albaGUIValidator(this, ID_DIALOG_TYPE, m_TypeComboBox, &m_SelectedType));
		typeBoxSizer->Add(m_TypeComboBox, 0, wxALL | wxEXPAND, 0);

		// BUTTON - Type Add
		m_TypeAddBtn = new albaGUIButton(this, ID_DIALOG_TYPE_ADD, "+", wxPoint(-1, -1), wxSize(50, 23));
		m_TypeAddBtn->SetListener(this);
		typeBoxSizer->Add(m_TypeAddBtn, 0, wxALIGN_RIGHT, 5);

		// BUTTON - Type Del
		m_TypeDelBtn = new albaGUIButton(this, ID_DIALOG_TYPE_DEL, "-", wxPoint(-1, -1), wxSize(50, 23));
		m_TypeDelBtn->SetListener(this);
		typeBoxSizer->Add(m_TypeDelBtn, 0, wxALIGN_RIGHT, 5);

		// BUTTON - Type Edit
		m_TypeEditBtn = new albaGUIButton(this, ID_DIALOG_TYPE_EDIT, "Edit", wxPoint(-1, -1), wxSize(80, 23));
		m_TypeEditBtn->SetListener(this);
		typeBoxSizer->Add(m_TypeEditBtn, 0, wxALIGN_RIGHT, 5);

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
		m_OkBtn = new albaGUIButton(this, ID_DIALOG_OK_PRESSED, "OK", wxPoint(-1, -1));
		m_OkBtn->SetListener(this);
		btnBoxSizer->Add(m_OkBtn, 0, wxALIGN_RIGHT, 15);

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
		m_ProducerEditBtn->Enable(m_ProducerNameList.size() > 0);
		m_ProducerAddBtn->Enable(true);
		m_ProducerDelBtn->Enable(m_ProducerNameList.size() > 0);
		
		m_TypeEditBtn->Enable(m_TypeNameList.size() > 0);
		m_TypeAddBtn->Enable(true);
		m_TypeDelBtn->Enable(m_TypeNameList.size() > 0);
		
		bool hasName = !m_NameTextCtrl->GetValue().IsEmpty();
		bool hasProducer = m_ProducerNameList.size() > 0;
		bool hasType = m_TypeNameList.size() > 0;
		m_OkBtn->Enable(hasName && hasProducer && hasType);
			
		// Update Image
		if (wxFileExists(m_ProsthesisImageFullName))
		{
			if (m_ImageButton != NULL)
			{
				m_ImageSizer->Remove(m_ImageButton);
				delete m_ImageButton;
			}

			// Load and show the image
			wxImage *previewImage = new wxImage();
			previewImage->LoadFile(m_ProsthesisImageFullName, wxBITMAP_TYPE_ANY);

			wxBitmap *previewBitmap;
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

		m_Gui->Update();
	}
}

//----------------------------------------------------------------------------
void appGUIDialogProsthesis::SetProsthesis(albaProDBProsthesis *prosthesis)
{
	if (prosthesis)
	{
		wxString DBDir = m_DBManager->GetDBDir();

		m_CurrentProsthesis = prosthesis;

		m_ProsthesisName = m_CurrentProsthesis->GetName();
		m_ProsthesisProducerName = m_CurrentProsthesis->GetProducer();
		m_ProsthesisImageName = m_CurrentProsthesis->GetImgFileName();
		m_ProsthesisImageFullName = DBDir + m_CurrentProsthesis->GetImgFileName();
		m_ProsthesisType = m_CurrentProsthesis->GetType();
		m_ProsthesisSide = m_CurrentProsthesis->GetSide();

		// Load Info
		m_NameTextCtrl->SetValue(m_ProsthesisName);
		m_ImagePath = m_ProsthesisImageName;

		// Load Producers Info
		std::vector<albaProDBProducer *> DBproducers = m_DBManager->GetProducers();

		m_ProducerNameList.clear();
		m_ProducerComboBox->Clear();
		m_SelectedProducer = 0;

		for (int p = 0; p < DBproducers.size(); p++)
		{
			m_ProducerNameList.push_back(DBproducers[p]->GetName().GetCStr());
			m_ProducerComboBox->Append(DBproducers[p]->GetName().GetCStr());

			if (m_ProducerNameList[p] == m_CurrentProsthesis->GetProducer())
				m_SelectedProducer = p;
		}
		
		// Load Types Info
		std::vector<albaProDBType *> DBTypes = m_DBManager->GetTypes();

		m_TypeNameList.clear();
		m_TypeComboBox->Clear();
		m_SelectedType = 0;
		
		for (int t = 0; t < DBTypes.size(); t++)
		{
			m_TypeNameList.push_back(DBTypes[t]->GetName().GetCStr());
			m_TypeComboBox->Append(DBTypes[t]->GetName().GetCStr());

			if (m_TypeNameList[t] == m_CurrentProsthesis->GetType())
				m_SelectedType = t;
		}

		// Load Side Info
		m_SelectedSide = m_ProsthesisSide;

		UpdateDialog();
	}
}

//----------------------------------------------------------------------------
void appGUIDialogProsthesis::SelectImage()
{
	albaString fileNameFullPath = albaGetDocumentsDirectory().c_str();
	albaString wildc = "Image file (*.bmp)|*.bmp";
	wxString imagePath = albaGetOpenFile(fileNameFullPath.GetCStr(), wildc, "Select file").c_str();

	if (wxFileExists(imagePath))
	{
		m_ProsthesisImageFullName = imagePath;
		m_ProsthesisImageName = wxFileNameFromPath(m_ProsthesisImageFullName);
		m_IsChanged = true;

		// Copy Image in DB Directory
		wxString DBDir = ((appLogic*)GetLogicManager())->GetProsthesesDBManager()->GetDBDir();
		wxCopyFile(m_ProsthesisImageFullName, DBDir + m_ProsthesisImageName);

		UpdateDialog();
	}
}

//----------------------------------------------------------------------------
void appGUIDialogProsthesis::EditType()
{
	if (m_SelectedType >= 0 && m_SelectedType < m_TypeNameList.size())
	{
		wxTextEntryDialog *dlg = new wxTextEntryDialog(this, "Name", "Edit Type", m_TypeNameList[m_SelectedType]);
		int result = dlg->ShowModal();
		wxString name = dlg->GetValue();

		if (result == wxID_OK && !name.IsEmpty())
		{
			m_DBManager->GetTypes()[m_SelectedType]->SetName(name);

			m_TypeNameList[m_SelectedType] = name;
			m_TypeComboBox->SetString(m_SelectedType, name);
			m_TypeComboBox->Select(m_SelectedType);
			m_TypeComboBox->Update();

			m_IsChanged = true;

			UpdateDialog();
		}
	}
}
//----------------------------------------------------------------------------
void appGUIDialogProsthesis::AddType()
{
	wxTextEntryDialog *dlg = new wxTextEntryDialog(this, "Name", "Add Type", wxString::Format("newType(%d)", m_TypeNameList.size()));
	int result = dlg->ShowModal();
	wxString name = dlg->GetValue();

	if (result == wxID_OK && !name.IsEmpty())
	{
		// Add to DB
		albaProDBType *newType = new albaProDBType();
		newType->SetName(name);
		m_DBManager->GetTypes().push_back(newType);

		m_TypeNameList.push_back(name);
		m_SelectedType = m_TypeNameList.size() - 1;
		m_TypeComboBox->Insert(name, m_SelectedType);
		m_TypeComboBox->Select(m_SelectedType);

		UpdateDialog();
	}
}
//----------------------------------------------------------------------------
void appGUIDialogProsthesis::RemoveType()
{
	if (m_TypeNameList.size() > 0 && m_SelectedType < m_TypeNameList.size())
	{
		m_TypeNameList.erase(m_TypeNameList.begin() + m_SelectedType);
		m_TypeComboBox->Delete(m_SelectedType);

		m_DBManager->GetTypes().erase(m_DBManager->GetTypes().begin() + m_SelectedType);

		m_SelectedType--;

		m_TypeComboBox->Select(m_SelectedType);
		m_TypeComboBox->Update();

		UpdateDialog();
	}
}

//----------------------------------------------------------------------------
void appGUIDialogProsthesis::EditProducer()
{
	if (m_SelectedProducer >= 0 && m_SelectedProducer < m_ProducerNameList.size())
	{
		m_CurrentProducer = m_DBManager->GetProducers()[m_SelectedProducer];

		appGUIDialogProducer pd(_("Edit Producer"));
		pd.SetProducer(m_CurrentProducer);
		pd.Show();

		if (pd.OkClosed())
		{
			m_ProducerNameList[m_SelectedProducer] = m_CurrentProducer->GetName();

			if (m_ProducerComboBox)
			{
				m_ProducerComboBox->SetString(m_SelectedProducer, m_CurrentProducer->GetName().GetCStr());
				m_ProducerComboBox->Select(m_SelectedProducer);
				m_ProducerComboBox->Update();
			}

			// Update DB
			m_DBManager->GetProducers()[m_SelectedProducer]->SetName(m_CurrentProducer->GetName());
			m_DBManager->GetProducers()[m_SelectedProducer]->SetImgFileName(m_CurrentProducer->GetImgFileName());
			m_DBManager->GetProducers()[m_SelectedProducer]->SetWebSite(m_CurrentProducer->GetWebSite());

			m_IsChanged = true;

			UpdateDialog();
		}
	}
}
//----------------------------------------------------------------------------
void appGUIDialogProsthesis::AddProducer()
{
	// Add new Producer
	albaProDBProducer *newProducer = new albaProDBProducer();
	newProducer->SetName(wxString::Format("newProducer(%d)", m_ProducerNameList.size()));
	newProducer->SetWebSite("");
	newProducer->SetImgFileName("");

	appGUIDialogProducer pd(_("Add Producer"));
	pd.SetProducer(newProducer);
	pd.Show();

	if (pd.OkClosed())
	{
		// Add to DB
		m_DBManager->GetProducers().push_back(newProducer);

		m_ProducerNameList.push_back(newProducer->GetName().GetCStr());
		m_SelectedProducer = m_ProducerNameList.size() - 1;

		if (m_ProducerComboBox)
		{
			// Add to Combobox
			m_ProducerComboBox->Insert(newProducer->GetName().GetCStr(), m_SelectedProducer);
			m_ProducerComboBox->Select(m_SelectedProducer);
			m_ProducerComboBox->Update();
		}

		m_CurrentProducer = newProducer;

		UpdateDialog();
	}
}
//----------------------------------------------------------------------------
void appGUIDialogProsthesis::RemoveProducer()
{
	if (m_ProducerNameList.size() > 0 && m_SelectedProducer < m_ProducerNameList.size())
	{
		m_ProducerNameList.erase(m_ProducerNameList.begin() + m_SelectedProducer);
		m_ProducerComboBox->Delete(m_SelectedProducer);

		m_DBManager->GetProducers().erase(m_DBManager->GetProducers().begin() + m_SelectedProducer);

		m_SelectedProducer--;

		m_ProducerComboBox->Select(m_SelectedProducer);
		m_ProducerComboBox->Update();

		UpdateDialog();
	}
}
