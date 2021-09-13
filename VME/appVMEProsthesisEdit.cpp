/*=========================================================================
Program:   AssemblerPro
Module:    appVMEProsthesisEdit.cpp
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Gianluigi Crimi
==========================================================================
Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "appDecl.h"
#include "appVMEProsthesisEdit.h"

#include "mmaMaterial.h"
#include "albaTransform.h"
#include "albaGUI.h"
#include "albaVMEOutput.h"
#include "albaVMESurface.h"

#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"
#include "vtkLookupTable.h"
#include "albaProsthesesDBManager.h"
#include "appGUIDialogProsthesis.h"


//-------------------------------------------------------------------------
albaCxxTypeMacro(appVMEProsthesisEdit)

//-------------------------------------------------------------------------
appVMEProsthesisEdit::appVMEProsthesisEdit()
{
	m_DBManager = NULL;
	
	m_ProsthesisComboBox = NULL;
	m_SelectedProsthesis = 0;
}
//-------------------------------------------------------------------------
appVMEProsthesisEdit::~appVMEProsthesisEdit()
{
}

//-------------------------------------------------------------------------
int appVMEProsthesisEdit::InternalInitialize()
{
	if (Superclass::InternalInitialize() == ALBA_OK)
	{
		m_DBManager = GetLogicManager()->GetProsthesesDBManager();

		UpdateGui();
		//SelectProsthesis();

		return ALBA_OK;
	}

	return ALBA_ERROR;
}

//-------------------------------------------------------------------------
albaGUI* appVMEProsthesisEdit::CreateGui()
{
	m_Gui = new albaGUI(this);

	m_ProsthesisComboBox = m_Gui->Combo(ID_PROSTHESIS_SELECTION, "", &m_SelectedProsthesis);

	m_Gui->TwoButtons(ID_PROSTHESIS_CHANGE, ID_PROSTHESIS_EDIT, "Change", "Edit");
	m_Gui->Divider(1);

	m_Gui->Divider();
	m_Gui->FitGui();

	return m_Gui;
}
//-------------------------------------------------------------------------
void appVMEProsthesisEdit::UpdateGui()
{
	if (m_Gui)
	{
		std::vector<albaProDBProshesis *> DBprosthesis = m_DBManager->GetProstheses();

		if (m_ProsthesisComboBox)
		{
			m_ProsthesisComboBox->Clear();
			for (int m = 0; m < DBprosthesis.size(); m++)
			{
				wxString name = wxString::Format("%s | %s", DBprosthesis[m]->GetProducer().GetCStr(), DBprosthesis[m]->GetName().GetCStr());
				m_ProsthesisComboBox->Append(name);
			}

			if (DBprosthesis.size() > 0)
				m_ProsthesisComboBox->Select(m_SelectedProsthesis);
		}

// 		m_Gui->Divider(0);
// 		m_Gui->Button(ID_GROUP_CREATE, "New Component Group");
	}
}

//-------------------------------------------------------------------------
void appVMEProsthesisEdit::OnEvent(albaEventBase *alba_event)
{
	// events to be sent up or down in the tree are simply forwarded
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch (e->GetId())
		{
		case ID_PROSTHESIS_SELECTION: SelectProsthesis(); break;
		case ID_PROSTHESIS_EDIT: EditProsthesis(); break;

		case ID_START:
		{
	
		}
		break;
			default:
			albaVME::OnEvent(alba_event);
		}
	}
	else
	{
		Superclass::OnEvent(alba_event);
	}
}

//----------------------------------------------------------------------------
void appVMEProsthesisEdit::SelectProsthesis()
{
	std::vector<albaProDBProshesis *> DBprosthesis = m_DBManager->GetProstheses();

	SetProsthesis(DBprosthesis[m_SelectedProsthesis]);

// 	if (m_Gui) 
// 	{
// 		m_Gui->Divider(0);
// 		m_Gui->Button(ID_GROUP_CREATE, "New Component Group");
// 	}

//
// 	m_GroupGui->FitGui();
// 	m_ContentGui->FitGui();
	for (int c=0; c<m_ComponentGui.size(); c++)
	{
		m_ComponentGui[c]->Update();
		m_ComponentGui[c]->FitGui();
	}

	m_Gui->FitGui();

	m_Gui->Update();
}
//----------------------------------------------------------------------------
void appVMEProsthesisEdit::EditProsthesis()
{
	if (m_Prosthesis)
	{
		// Show dialog
		appGUIDialogProsthesis md(_("Edit Prosthesis"));
		md.SetProsthesis(m_Prosthesis);
		md.ShowModal();

		if (md.OkClosed())
		{
			// Save DB
			m_DBManager->SaveDB();
			UpdateGui();
		}
		else
		{
			// Reload DB
			m_DBManager->LoadDB();
		}
	}
}