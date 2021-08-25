/*=========================================================================
Program:   AssemblerPro
Module:    appOpManageProducer.cpp
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

#include "appOpManageProducer.h"
#include "appDecl.h"
#include "appGUI.h"
#include "appGUIDialogProducer.h"
#include "appLogic.h"
#include "appUtils.h"

#include "albaGUI.h"
#include "albaProsthesesDBManager.h"
#include "albaServiceClient.h"
#include "albaVME.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(appOpManageProducer);

//----------------------------------------------------------------------------
appOpManageProducer::appOpManageProducer(wxString label) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;

	m_DBManager = NULL;

	m_ProducerComboBox = NULL;
	m_SelectedProducer = 0;
}

//----------------------------------------------------------------------------
appOpManageProducer::~appOpManageProducer()
{
	m_ProducersVect.clear();
	m_ProducerNameList.clear();
}

//----------------------------------------------------------------------------
bool appOpManageProducer::Accept(albaVME *node)
{
	//return node->IsA("...");
	return true;
}

//----------------------------------------------------------------------------
char** appOpManageProducer::GetIcon()
{
#include "pic/MENU_OP_MANAGE_PRODUCER.xpm"
	return MENU_OP_MANAGE_PRODUCER_xpm;
}

//----------------------------------------------------------------------------
albaOp* appOpManageProducer::Copy()
{
	appOpManageProducer *cp = new appOpManageProducer(m_Label);
	return cp;
}
//----------------------------------------------------------------------------
void appOpManageProducer::OpRun()
{
	LoadInfo();
	
	if (!m_TestMode)
	{
		CreateGui();
	}
}

/// Load/Save Data
//----------------------------------------------------------------------------
void appOpManageProducer::LoadInfo()
{
	m_DBManager = ((appLogic*)GetLogicManager())->GetProsthesesDBManager();
	//////////////////////////////////////////////////////////////////////////
	wxString dbFilePath = appUtils::GetConfigDirectory().c_str();

	std::vector<albaProDBProducer *> DBproducers = m_DBManager->GetProducers();

	// Load Producers Info
	m_ProducerNameList.clear();
	m_ProducersVect.clear();

	for (int p = 0; p < DBproducers.size(); p++)
	{
		Producer producer;
		producer.index = p;
		producer.name = DBproducers[p]->GetName();
		producer.webSite = DBproducers[p]->GetWebSite();
		producer.image = DBproducers[p]->GetImgFileName();

		m_ProducersVect.push_back(producer);

		m_ProducerNameList.push_back(m_ProducersVect[p].name);
	}

	if (DBproducers.size() == 0)
		AddProducer();
}
//----------------------------------------------------------------------------
void appOpManageProducer::SaveInfo()
{
	int pChanges = 0;

	m_DBManager = GetLogicManager()->GetProsthesesDBManager();

	std::vector<albaProDBProducer *> DBproducers = m_DBManager->GetProducers();

	//////////////////////////////////////////////////////////////////////////

	// Update Producers
	for (int p=0; p< m_ProducersVect.size(); p++)
	{
		if (m_ProducersVect[p].index < 0)
		{
			// Add New Producer
			albaProDBProducer *newProducer = new albaProDBProducer();
			newProducer->SetName(m_ProducersVect[p].name);
			newProducer->SetImgFileName(m_ProducersVect[p].image);
			newProducer->SetWebSite(m_ProducersVect[p].webSite);

			m_DBManager->GetProducers().push_back(newProducer);
		}
		else
		{
			if (m_ProducersVect[p].isChanged)
			{
				m_DBManager->GetProducers()[p]->SetName(m_ProducersVect[p].name);
				m_DBManager->GetProducers()[p]->SetImgFileName(m_ProducersVect[p].image);
				m_DBManager->GetProducers()[p]->SetWebSite(m_ProducersVect[p].webSite);
				pChanges++;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
// 	wxString dbFilePath = appUtils::GetConfigDirectory().c_str();
// 	dbFilePath += "/SavedDB.xml";
// 
// 	m_DBManager->SaveDBToFile(dbFilePath);

	wxString message = wxString::Format("Producer: %d changes.", pChanges);
	wxMessageBox(message);
}

//----------------------------------------------------------------------------
void appOpManageProducer::OpStop(int result)
{
	if (!m_TestMode)
	{
		HideGui();
	}

	if (result == OP_RUN_OK)
	{
		SaveInfo();
	}

	albaEventMacro(albaEvent(this, result));
}
//----------------------------------------------------------------------------
void appOpManageProducer::OpDo()
{
}

//----------------------------------------------------------------------------
void appOpManageProducer::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		//if (e->GetSender() == m_Gui)
		{
			switch (e->GetId())
			{
				// GUI Events
			case wxOK:
				OpStop(OP_RUN_OK);
				break;

			case wxCANCEL:
				OpStop(OP_RUN_CANCEL);
				break;

			case ID_SELECT_PRODUCER:
				SelectProducer();
				break;

			case ID_EDIT_PRODUCER:
				EditProducer();
				break;

			case ID_ADD_PRODUCER:
				AddProducer();
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
void appOpManageProducer::CreateGui()
{
	// Interface:
	m_Gui = new appGUI(this);

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("Producer", true);
	m_ProducerComboBox = m_Gui->Combo(ID_SELECT_PRODUCER, "", &m_SelectedProducer, m_ProducerNameList.size(), &m_ProducerNameList[0], "");
	m_Gui->TwoButtons(ID_EDIT_PRODUCER, ID_ADD_PRODUCER, "Edit", "Add New");
	m_Gui->Divider(1);

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();
	m_Gui->Label("");
	
	UpdateGui();
	ShowGui();
}
//----------------------------------------------------------------------------
void appOpManageProducer::UpdateGui()
{
	if (m_Gui)
	{
		// Enable Producer Gui
		m_Gui->Enable(ID_EDIT_PRODUCER, m_SelectedProducer >= 0);
		m_Gui->Update();
	}
}

//////////////////////////////////////////////////////////////////////////
/// Prosthesis Management

// Producer
//----------------------------------------------------------------------------
void appOpManageProducer::SelectProducer()
{
	UpdateGui();
}
//----------------------------------------------------------------------------
void appOpManageProducer::AddProducer()
{
	// Create New Producer
	m_CurrentProducer.name = wxString::Format("newProducer(%d)", m_ProducerNameList.size());
	m_CurrentProducer.webSite = "";
	m_CurrentProducer.image = "";

	// Add to Combobox
	m_ProducerNameList.push_back(m_CurrentProducer.name);
	if(m_ProducerComboBox)
		m_ProducerComboBox->Append(m_CurrentProducer.name);
	
	// Add to Vector
	Producer producer;
	producer.index = -1;
	producer.name = m_CurrentProducer.name;
	producer.webSite = m_CurrentProducer.webSite;
	producer.image = m_CurrentProducer.image;
	m_ProducersVect.push_back(producer);

	// Select
	m_SelectedProducer = m_ProducerNameList.size() - 1;

	if (m_ProducerComboBox)
	m_ProducerComboBox->Select(m_SelectedProducer);

	SelectProducer();

	EditProducer();
}
//----------------------------------------------------------------------------
void appOpManageProducer::EditProducer()
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
void appOpManageProducer::UpdateProducer(Producer producer)
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

	UpdateGui();
}