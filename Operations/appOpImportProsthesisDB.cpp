/*=========================================================================
Program:   AssemblerPro
Module:    appOpImportProsthesisDB.cpp
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

#include "appOpImportProsthesisDB.h"
#include "appDecl.h"
#include "appGUI.h"

#include "albaGUI.h"
#include "albaVME.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(appOpImportProsthesisDB);

//----------------------------------------------------------------------------
appOpImportProsthesisDB::appOpImportProsthesisDB(wxString label) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;
}

//----------------------------------------------------------------------------
appOpImportProsthesisDB::~appOpImportProsthesisDB()
{
}

//----------------------------------------------------------------------------
bool appOpImportProsthesisDB::InternalAccept(albaVME *node)
{
	//return node->IsA("...");
	return true;
}

//----------------------------------------------------------------------------
char** appOpImportProsthesisDB::GetIcon()
{
#include "pic/MENU_OP_IMPORT_PROSTHESIS.xpm"
	return MENU_OP_IMPORT_PROSTHESIS_xpm;
}

//----------------------------------------------------------------------------
albaOp* appOpImportProsthesisDB::Copy()
{
	appOpImportProsthesisDB *cp = new appOpImportProsthesisDB(m_Label);
	return cp;
}
//----------------------------------------------------------------------------
void appOpImportProsthesisDB::OpRun()
{
	if (!m_TestMode)
	{
		CreateGui();
	}
	
	//OpStop(OP_RUN_OK);
}
//----------------------------------------------------------------------------
void appOpImportProsthesisDB::OpStop(int result)
{
	if (!m_TestMode)
	{
		HideGui();
	}

	albaEventMacro(albaEvent(this, result));
}
//----------------------------------------------------------------------------
void appOpImportProsthesisDB::OpDo()
{
}

//----------------------------------------------------------------------------
void appOpImportProsthesisDB::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		m_Gui->Update();
		//if (e->GetSender() == m_Gui)
		{
			switch (e->GetId())
			{
			case wxOK:
				OpStop(OP_RUN_OK);
				break;

			case wxCANCEL:
				OpStop(OP_RUN_CANCEL);
				break;

			default:
				Superclass::OnEvent(alba_event);
				break;
			}
		}
// 		else
// 		{
// 			Superclass::OnEvent(alba_event);
// 		}
	}
}

//----------------------------------------------------------------------------
void appOpImportProsthesisDB::CreateGui()
{
	// Interface:
	m_Gui = new appGUI(this);

	m_Gui->HintBox(NULL, "This operation is empty.", "Hint");

//	((appGUI*)m_Gui)->HyperLink(NULL, "Link", "https://github.com/IOR-BIC");

// 	((appGUI*)m_Gui)->Separator(0, wxSize(1, 100));
//	((appGUI*)m_Gui)->Separator(2, wxSize(250, 1));
	
	// ToDO: add your custom widgets...

	//m_Gui->Label("Press a button");

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();
	m_Gui->Label("");

	ShowGui();
}