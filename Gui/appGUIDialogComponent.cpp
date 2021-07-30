/*=========================================================================
Program:   AssemblerPro
Module:    appGUIDialogComponent.cpp
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

#include "appGUIDialogComponent.h"
#include "appUtils.h"

#include "albaDecl.h"
#include "albaGUIButton.h"
#include "albaGUILab.h"
#include "albaGUIPicButton.h"
#include "albaGUIValidator.h"

#include "wx\image.h"
#include "wx\window.h"

enum COMPONENT_DIALOG_ID
{
	ID_COMPONENT_DIALOG_TEXT = MINID,
	ID_COMPONENT_DIALOG_OK_PRESSED,
};

//----------------------------------------------------------------------------
appGUIDialogComponent::appGUIDialogComponent(const wxString& title, long style)
: albaGUIDialog(title, style)
{
	m_Gui = NULL;

	CreateComponentDialog();
}
//----------------------------------------------------------------------------
appGUIDialogComponent::~appGUIDialogComponent()
{
}

//----------------------------------------------------------------------------
void appGUIDialogComponent::OnEvent(albaEventBase *alba_event)
{
	switch (alba_event->GetId())
	{
	case ID_COMPONENT_DIALOG_TEXT:
		break;
	case ID_COMPONENT_DIALOG_OK_PRESSED:
		break;

	default:
		albaGUIDialog::OnEvent(alba_event);
	}
}

//----------------------------------------------------------------------------
void appGUIDialogComponent::Show()
{
	CreateComponentDialog();
	ShowModal();
}

//----------------------------------------------------------------------------
void appGUIDialogComponent::CreateComponentDialog()
{
	wxString title = "Edit Component";

	//////////////////////////////////////////////////////////////////////////
	if (m_Gui == NULL)
	{
		m_Gui = new albaGUI(this);

		//////////////////////////////////////////////////////////////////////////

		wxBoxSizer *mainBoxSizer = new wxBoxSizer(wxVERTICAL);

		int panelWidth = 400;

		//////////////////////////////////////////////////////////////////////////
		// Info Sizer
		wxBoxSizer *infoBoxSizer = new wxBoxSizer(wxVERTICAL);

		// TEXT - Component Name
		wxStaticBoxSizer *labelSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, "Name");
		m_ComponentName_textCtrl = new wxTextCtrl(this, ID_COMPONENT_DIALOG_TEXT, m_CurrentComponent.name, wxPoint(-1, -1), wxSize(panelWidth, 25), wxALL | wxEXPAND);
		m_ComponentName_textCtrl->SetEditable(true);
		m_ComponentName_textCtrl->SetMaxLength(64);
		labelSizer1->Add(m_ComponentName_textCtrl, 0, wxALL | wxEXPAND, 0);
		infoBoxSizer->Add(labelSizer1, 0, wxALL | wxEXPAND, 5);

		// TEXT - Empty Separator
		infoBoxSizer->Add(new albaGUILab(this, -1, " "), 0, wxALIGN_LEFT, 5);

		mainBoxSizer->Add(infoBoxSizer, 0, wxALL, 5);

		// BUTTON - Ok
		albaGUIButton *okBtn = new albaGUIButton(this, wxID_OK, "OK", wxPoint(-1, -1));
		okBtn->SetListener(this);
		mainBoxSizer->Add(okBtn, 0, wxALIGN_RIGHT, 0);

		//////////////////////////////////////////////////////////////////////////
		//m_ComponentDialog->Add(mainBoxSizer, 0, wxALL, 5);
		m_Gui->Fit();

		// Show dialog
		wxSize s = albaGetFrame()->GetSize();
		wxPoint p = albaGetFrame()->GetPosition();
		int posX = p.x + s.GetWidth() * .5 - this->GetSize().GetWidth() * .5;
		int posY = p.y + s.GetHeight() * .5 - this->GetSize().GetHeight() * .5;

		this->SetPosition(wxPoint(posX, posY));

		Add(m_Gui, 1);
		Add(mainBoxSizer, 0);
	}

	UpdateComponentDialog();
}

//----------------------------------------------------------------------------
void appGUIDialogComponent::UpdateComponentDialog()
{
	if (m_Gui)
	{
		m_ComponentName_textCtrl->SetValue(m_CurrentComponent.name);
	}
}
