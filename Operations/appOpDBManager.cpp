/*=========================================================================
Program:   AssemblerPro
Module:    appOpDBManager.cpp
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

#include "appOpDBManager.h"
#include "appDecl.h"
#include "appGUI.h"
#include "appLogic.h"

#include "albaGUI.h"
#include "albaVME.h"
#include "albaServiceClient.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(appOpDBManager);

//----------------------------------------------------------------------------
appOpDBManager::appOpDBManager(wxString label) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;

	m_ProsthesesDBManager = NULL;

	m_ProducerComboBox = NULL;
	m_TypeComboBox = NULL;
	m_ProsthesisComboBox = NULL;

	m_SelectedProducer = 0;
	m_SelectedType = 0;
	m_SelectedProsthesis = 0;
}

//----------------------------------------------------------------------------
appOpDBManager::~appOpDBManager()
{
}

//----------------------------------------------------------------------------
bool appOpDBManager::InternalAccept(albaVME *node)
{
	return true;
}

//----------------------------------------------------------------------------
char** appOpDBManager::GetIcon()
{
#include "pic/MENU_OP_MANAGE_PRODUCER.xpm"
	return MENU_OP_MANAGE_PRODUCER_xpm;
}

//----------------------------------------------------------------------------
albaOp* appOpDBManager::Copy()
{
	appOpDBManager *cp = new appOpDBManager(m_Label);
	return cp;
}
//----------------------------------------------------------------------------
void appOpDBManager::OpRun()
{
	m_ProsthesesDBManager = GetLogicManager()->GetProsthesesDBManager();

	if (!m_TestMode)
	{
		CreateGui();
		UpdateGui();
	}
}
//----------------------------------------------------------------------------
void appOpDBManager::OpStop(int result)
{
	if (!m_TestMode)
	{
		HideGui();
	}

	((appLogic*)GetLogicManager())->RefreshVMEProsthesis();

	albaEventMacro(albaEvent(this, result));
}
//----------------------------------------------------------------------------
void appOpDBManager::OpDo()
{
}

//----------------------------------------------------------------------------
void appOpDBManager::OnEvent(albaEventBase *alba_event)
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

			case ID_LOAD:
				m_ProsthesesDBManager->LoadDB();
				UpdateGui();
				break;

			case ID_SAVE:
				m_ProsthesesDBManager->SaveDB();
				UpdateGui();
				break;

			case ID_CLEAR:
				m_ProsthesesDBManager->Clear();
				m_SelectedProsthesis = 0;
				m_SelectedProducer = 0;
				m_SelectedType = 0;
				UpdateGui();
				break;

			case ID_DELETE_PRODUCER: break;
			case ID_DELETE_TYPE: break;
			case ID_DELETE_PROSTHESIS:
			{
				albaProDBProsthesis *prosthesis = m_ProsthesesDBManager->GetProstheses().at(m_SelectedProsthesis);
				if (prosthesis)
					m_ProsthesesDBManager->DeleteProsthesis(prosthesis->GetName(), prosthesis->GetSide());
				UpdateGui();
			}
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
void appOpDBManager::CreateGui()
{
	m_Gui = new appGUI(this);

	m_Gui->Label("Producer");
	m_ProducerComboBox = m_Gui->Combo(ID_PRODUCER_SELECTION, "", &m_SelectedProducer);

	m_Gui->Label("Type");
	m_TypeComboBox = m_Gui->Combo(ID_TYPE_SELECTION, "", &m_SelectedType);
	
	m_Gui->Label("Prosthesis");
	m_ProsthesisComboBox = m_Gui->Combo(ID_PROSTHESIS_SELECTION, "", &m_SelectedProsthesis);
	m_Gui->Button(ID_DELETE_PROSTHESIS, "Delete");
	m_Gui->Divider(1);
	m_Gui->Label("");

	m_Gui->TwoButtons(ID_LOAD, ID_SAVE, "Load", "Save");
	m_Gui->Button(ID_CLEAR, "Clear");

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();
	m_Gui->Label("");

	ShowGui();
}

//----------------------------------------------------------------------------
void appOpDBManager::UpdateGui()
{
	if (m_Gui)
	{
		// Update Prosthesis Name List
		std::vector<albaProDBProsthesis *> DBprosthesis = m_ProsthesesDBManager->GetProstheses();

		m_ProsthesisComboBox->Clear();
		for (int m = 0; m < DBprosthesis.size(); m++)
		{
			wxString name = wxString::Format("%s - %s", DBprosthesis[m]->GetProducer().GetCStr(), DBprosthesis[m]->GetName().GetCStr());
			m_ProsthesisComboBox->Append(name);
		}
		m_ProsthesisComboBox->Select(m_SelectedProsthesis);

		// Update Producer Name List
		std::vector<albaProDBProducer *> DBproducer = m_ProsthesesDBManager->GetProducers();

		m_ProducerComboBox->Clear();
		for (int m = 0; m < DBproducer.size(); m++)
		{
			wxString name = wxString::Format("%s", DBproducer[m]->GetName().GetCStr());
			m_ProducerComboBox->Append(name);
		}
		m_ProducerComboBox->Select(m_SelectedProducer);

		// Update Producer Name List
		std::vector<albaProDBType *> DBtypes = m_ProsthesesDBManager->GetTypes();

		m_TypeComboBox->Clear();
		for (int m = 0; m < DBtypes.size(); m++)
		{
			wxString name = wxString::Format("%s", DBtypes[m]->GetName().GetCStr());
			m_TypeComboBox->Append(name);
		}
		m_TypeComboBox->Select(m_SelectedType);

		//
		m_Gui->Update();
	}
}
