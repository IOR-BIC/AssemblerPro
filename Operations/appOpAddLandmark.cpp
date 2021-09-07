/*=========================================================================
Program:   AssemblerPro
Module:    appOpAddLandmark.cpp
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

#include "appOpAddLandmark.h"
#include "albaOpAddLandmark.h"
#include "albaGUI.h"
#include "albaGUIDictionaryWidget.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(appOpAddLandmark);

//----------------------------------------------------------------------------
appOpAddLandmark::appOpAddLandmark(wxString label) :albaOpAddLandmark(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;
}

//----------------------------------------------------------------------------
appOpAddLandmark::~appOpAddLandmark()
{}

//----------------------------------------------------------------------------
albaOp* appOpAddLandmark::Copy()
{
	appOpAddLandmark *cp = new appOpAddLandmark(m_Label);
	return cp;
}
//----------------------------------------------------------------------------
void appOpAddLandmark::OpRun()
{
	Superclass::OpRun();

	// Find Existent Cloud
	if (m_Input)
	{
		albaVMELandmarkCloud *cloud = (albaVMELandmarkCloud*)m_Input->GetRoot()->FindInTreeByName("Cloud");
		if (cloud) 
			LoaDictionaryFromCloud(cloud);
	}
}

//----------------------------------------------------------------------------
void appOpAddLandmark::CreateGui()
{
	// Setup Gui
	m_Gui = new albaGUI(this);
	m_Gui->SetListener(this);

	const wxString choices[] = { "Show All" };
	m_GroupComboBox = m_Gui->Combo(ID_SHOW_LANDMARK_GROUP, "", &m_SelectedGroup, 1, choices);
	m_Gui->Divider(2);

	// Setup dictionary
	m_LandmarkGuiDict = new albaGUIDictionaryWidget(m_Gui, -1);
	m_LandmarkGuiDict->SetListener(this);
	m_LandmarkGuiDict->SetCloud(m_Cloud);
	m_LandmarkGuiDict->InitDictionary(NULL);
	m_LandmarkGuiDict->SetTitle("Landmarks List");
	m_Gui->Add(m_LandmarkGuiDict->GetWidget(), wxEXPAND);
	m_Gui->Divider(2);

	m_Gui->Button(ID_LANDMARK_REMOVE, "Remove");
	m_Gui->Divider(1);

	m_Gui->Label(_("Landmark Props"));
	m_Gui->String(ID_LANDMARK_NAME, _("Name"), &m_LandmarkName);
	m_Gui->Vector(ID_LANDMARK_POSITION, _("Position"), m_LandmarkPosition, MINFLOAT, MAXFLOAT, 2, _("Landmark position"));
	m_Gui->Double(ID_LANDMARK_RADIUS, "Radius:", &m_LandmarkRadius);
 	m_Gui->Divider(1);

 	m_Gui->Label(_("Dictionary"));
 	m_Gui->Button(ID_LOAD_DICTIONARY_FROM_CLOUD, _("Load Dictionary from Cloud"));

	m_Gui->Enable(ID_LANDMARK_REMOVE, false);
	m_Gui->Enable(ID_LANDMARK_NAME, false);
	m_Gui->Enable(ID_LANDMARK_POSITION, false);

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();
	m_Gui->Label("");

	ShowGui();
}