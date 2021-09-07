/*=========================================================================
Program:   AssemblerPro
Module:    appGUIDialogMatrix.cpp
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

#include "appGUIDialogMatrix.h"

#include "albaDecl.h"
#include "albaGUIButton.h"
#include "albaGUILab.h"
#include "albaGUIValidator.h"
#include "albaProsthesesDBManager.h"

#include "wx\statline.h"

enum COMPONENT_DIALOG_ID
{
	ID_COMPONENT_DIALOG_MATRIX = MINID,
	ID_COMPONENT_DIALOG_OK_PRESSED,
};

//----------------------------------------------------------------------------
appGUIDialogMatrix::appGUIDialogMatrix(const wxString& title, long style)
: albaGUIDialog(title, style)
{
	m_Gui = NULL;
	m_CurrentComponent = NULL;
	m_IsChanged = false;

	CreateMatrixDialog();
}
//----------------------------------------------------------------------------
appGUIDialogMatrix::~appGUIDialogMatrix()
{
}

//----------------------------------------------------------------------------
void appGUIDialogMatrix::OnEvent(albaEventBase *alba_event)
{
	switch (alba_event->GetId())
	{
	case ID_COMPONENT_DIALOG_MATRIX:
	{
		for (int i=0; i<4; i++)
			for (int j = 0; j < 4; j++)
			{
				double value;
				if (m_TextValues[i][j].ToDouble(&value))
					m_ComponentMatrix.SetElement(i, j, value);
			}

		UpdateMatrixDialog();
	}
	break;

	case ID_COMPONENT_DIALOG_OK_PRESSED:
	{
		m_CurrentComponent->SetMatrix(m_ComponentMatrix);
		m_IsChanged = true;
		this->Close();
	}
	break;

	default:
		albaGUIDialog::OnEvent(alba_event);
	}
}

//----------------------------------------------------------------------------
void appGUIDialogMatrix::SetComponent(albaProDBComponent *component)
{
	if (component)
	{
		m_CurrentComponent = component;
		m_ComponentMatrix = m_CurrentComponent->GetMatrix();

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
			{
				m_TextValues[i][j] = wxString::Format("%f", m_ComponentMatrix.GetElement(i, j));
			}

		UpdateMatrixDialog();
	}
}

//----------------------------------------------------------------------------
void appGUIDialogMatrix::Show()
{
	CreateMatrixDialog();
	ShowModal();
}

//----------------------------------------------------------------------------
void appGUIDialogMatrix::CreateMatrixDialog()
{
	if (m_Gui == NULL)
	{
		m_Gui = new albaGUI(this);

		//////////////////////////////////////////////////////////////////////////

		wxBoxSizer *mainBoxSizer = new wxBoxSizer(wxVERTICAL);

		int panelWidth = 330;

		//////////////////////////////////////////////////////////////////////////
		// Info Sizer
		wxBoxSizer *infoBoxSizer = new wxBoxSizer(wxVERTICAL);

		// TEXT - Component Matrix
		wxGridSizer *matrixGridSizer = new wxGridSizer(4, 4);

		for (int i=0; i<4; i++)
			for (int j = 0; j < 4; j++)
			{				
				wxTextCtrl *text = new wxTextCtrl(this, ID_COMPONENT_DIALOG_MATRIX, "", wxPoint(-1, -1), wxSize(80, 25), wxALL | wxEXPAND);
				text->SetValidator(albaGUIValidator(this, ID_COMPONENT_DIALOG_MATRIX, text, &m_TextValues[i][j], true));
				text->SetEditable(true);
				text->SetMaxLength(10);
				matrixGridSizer->Add(text, 0, wxALL | wxEXPAND, 0);
			}

		infoBoxSizer->Add(matrixGridSizer, 0, wxALL | wxEXPAND, 5);
		mainBoxSizer->Add(infoBoxSizer, 0, wxALL, 5);

		// LINE Separator
		mainBoxSizer->Add(new wxStaticLine(this, -1, wxPoint(-1, -1), wxSize(panelWidth, 1)), 0, wxALL, 5);

		// BUTTON - Ok
		m_OkBtn = new albaGUIButton(this, ID_COMPONENT_DIALOG_OK_PRESSED, "OK", wxPoint(-1, -1));
		m_OkBtn->SetListener(this);
		mainBoxSizer->Add(m_OkBtn, 0, wxALIGN_RIGHT, 0);

		//////////////////////////////////////////////////////////////////////////

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

	UpdateMatrixDialog();
}

//----------------------------------------------------------------------------
void appGUIDialogMatrix::UpdateMatrixDialog()
{
	if (m_Gui)
	{
		m_Gui->Update();
	}
}
