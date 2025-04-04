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
#include "appLogic.h"

#include "albaDecl.h"
#include "albaGUIButton.h"
#include "albaGUILab.h"
#include "albaGUIValidator.h"
#include "albaOpImporterSTL.h"
#include "albaProsthesesDBManager.h"
#include "albaVME.h"
#include "albaVMEManager.h"
#include "albaVMEOutput.h"
#include "albaVMESurface.h"

#include "vtkALBASmartPointer.h"
#include "vtkDataSetReader.h"
#include "wx\filename.h"
#include "wx\statline.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"
#include "albaMatrix.h"

enum COMPONENT_DIALOG_ID
{
	ID_COMPONENT_DIALOG_TEXT = MINID,
	ID_COMPONENT_DIALOG_SET_VTKDATA_FROM_TREE,
	ID_COMPONENT_DIALOG_SET_VTKDATA_FROM_FILE,
	ID_COMPONENT_DIALOG_OK_PRESSED,
};

//----------------------------------------------------------------------------
appGUIDialogComponent::appGUIDialogComponent(const wxString& title, long style)
: albaGUIDialog(title, style)
{
	m_Gui = NULL;
	m_CurrentComponent = NULL;
	m_ComponentName = "";
	m_HasVtkData = false;
	m_IsChanged = false;

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
		m_ComponentName = m_ComponentName_textCtrl->GetValue();
		UpdateComponentDialog();
		break;

	case ID_COMPONENT_DIALOG_SET_VTKDATA_FROM_TREE:
		AddVTKFromTree(NULL);
		UpdateComponentDialog();
		break;

	case ID_COMPONENT_DIALOG_SET_VTKDATA_FROM_FILE:
		AddVTKFromFile();
		UpdateComponentDialog();
		break;

	case ID_COMPONENT_DIALOG_OK_PRESSED:
		m_CurrentComponent->SetName(m_ComponentName_textCtrl->GetValue());
		m_IsChanged = true;
		this->Close();
		break;

	default:
		albaGUIDialog::OnEvent(alba_event);
	}
}

//----------------------------------------------------------------------------
void appGUIDialogComponent::SetComponent(albaProDBComponent *component)
{
	if (component)
	{
		m_CurrentComponent = component;
		m_ComponentName = component->GetName();
		m_HasVtkData = component->GetVTKData() != NULL;

		UpdateComponentDialog();
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
	if (m_Gui == NULL)
	{
		m_Gui = new albaGUI(this);

		//////////////////////////////////////////////////////////////////////////

		wxBoxSizer *mainBoxSizer = new wxBoxSizer(wxVERTICAL);

		int panelWidth = 330;

		//////////////////////////////////////////////////////////////////////////
		// Info Sizer
		wxBoxSizer *infoBoxSizer = new wxBoxSizer(wxVERTICAL);

		// TEXT - Component Name
		wxStaticBoxSizer *labelSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, "Name");
		m_ComponentName_textCtrl = new wxTextCtrl(this, ID_COMPONENT_DIALOG_TEXT, m_ComponentName, wxPoint(-1, -1), wxSize(panelWidth, 25), wxALL | wxEXPAND);
		m_ComponentName_textCtrl->SetValidator(albaGUIValidator(this, ID_COMPONENT_DIALOG_TEXT, m_ComponentName_textCtrl, &m_ComponentName, true));
		m_ComponentName_textCtrl->SetEditable(true);
		m_ComponentName_textCtrl->SetMaxLength(64);
		labelSizer1->Add(m_ComponentName_textCtrl, 0, wxALL | wxEXPAND, 0);
		infoBoxSizer->Add(labelSizer1, 0, wxALL | wxEXPAND, 5);

		wxStaticBoxSizer *vtkDataBoxSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, "Vtk Data");

		// TEXT - Prosthesis Name
		m_VtkDataTextCtrl = new wxTextCtrl(this, NULL, "NO VTK DATA", wxPoint(-1, -1), wxSize(140, 20), wxALL | wxEXPAND);
		m_VtkDataTextCtrl->SetEditable(false);
		vtkDataBoxSizer->Add(m_VtkDataTextCtrl, 0, wxALL | wxEXPAND, 0);

		// BUTTON - Type Add
		albaGUIButton *m_VtkDataTreeBtn = new albaGUIButton(this, ID_COMPONENT_DIALOG_SET_VTKDATA_FROM_TREE, "Select from Tree", wxPoint(-1, -1), wxSize(-1, 23));
		m_VtkDataTreeBtn->SetListener(this);
		vtkDataBoxSizer->Add(m_VtkDataTreeBtn, 0, wxALIGN_RIGHT, 5);

		// BUTTON - Type Add
		albaGUIButton *m_VtkDataFileBtn = new albaGUIButton(this, ID_COMPONENT_DIALOG_SET_VTKDATA_FROM_FILE, "Import from file", wxPoint(-1, -1), wxSize(-1, 23));
		m_VtkDataFileBtn->SetListener(this);
		vtkDataBoxSizer->Add(m_VtkDataFileBtn, 0, wxALIGN_RIGHT, 5);

		infoBoxSizer->Add(vtkDataBoxSizer, 0, wxALL | wxEXPAND, 5);

		mainBoxSizer->Add(infoBoxSizer, 0, wxALL, 5);

		// LINE Separator
		mainBoxSizer->Add(new wxStaticLine(this, -1, wxPoint(-1, -1), wxSize(panelWidth + 20, 1)), 0, wxALL, 5);

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

	UpdateComponentDialog();
}
//----------------------------------------------------------------------------
void appGUIDialogComponent::UpdateComponentDialog()
{
	if (m_Gui)
	{
		m_ComponentName_textCtrl->SetValue(m_ComponentName);

		wxString message = m_HasVtkData ? "VTK DATA [OK]" : "VTK DATA [NOT FOUND]";
		m_VtkDataTextCtrl->SetValue(message);
		
		m_OkBtn->Enable(!m_ComponentName.IsEmpty() && m_HasVtkData);
		m_Gui->Update();
	}
}

//----------------------------------------------------------------------------
void appGUIDialogComponent::AddVTKFromFile()
{
	albaString fileNameFullPath = albaGetLastUserFolder();
	albaString wildc = "STL file (*.stl)|*.stl|VTK file (*.vtk)|*.vtk";
	wxString imagePath = albaGetOpenFile(fileNameFullPath.GetCStr(), wildc, "Select file");

	if (wxFileExists(imagePath))
	{
		wxString name, path, ext;
		wxFileName::SplitPath(imagePath, &path, &name, &ext);
		
		vtkPolyData *polyData = NULL;

		if (ext == "stl")
		{
			// Import STL file
	 		albaOpImporterSTL *importer = new albaOpImporterSTL("Importer STL");
	 		importer->TestModeOn();
	 		importer->SetFileName(imagePath);
	 		importer->OpRun();
	 
	 		std::vector<albaVMESurface*> importedSTL;
	 		importer->GetImportedSTL(importedSTL);
	 		albaVMESurface *node = importedSTL[0];
			node->GetOutput()->Update();
			polyData = (vtkPolyData*)node->GetOutput()->GetVTKData();
			if (polyData == NULL)
			{
				albaErrorMessage("Load STL Failed!\nData is not updated.");
				albaDEL(importer);
				return;
			}
			else
			{
				m_CurrentComponent->SetVTKData(polyData);
				m_ComponentName = name;
				m_ComponentName.Replace("_", " ");

				albaDEL(importer);
				m_HasVtkData = true;
			}
		}
		else if (ext == "vtk")
		{
			// Import VTK file
			vtkALBASmartPointer<vtkDataSetReader> importer;
			importer->SetFileName(imagePath);
			importer->Update();

			polyData = (vtkPolyData*)importer->GetOutput();
			if (polyData == NULL)
			{
				albaErrorMessage("Load STL Failed!\nData is not updated.");
				return;
			}
			else
			{
				m_CurrentComponent->SetVTKData(polyData);
				m_ComponentName = name;
				m_ComponentName.Replace("_", " ");
				m_HasVtkData = true;
			}
		}
	}
}
//----------------------------------------------------------------------------
void appGUIDialogComponent::AddVTKFromTree(albaVME *node)
{
	if (node == NULL)
	{
		albaString title = _("Choose VTK Data");
		albaEvent e(this, VME_CHOOSE, &title);
		e.SetPointer(&appGUIDialogComponent::VTKDataAccept);
		//albaEventMacro(e);
		((appLogic*)GetLogicManager())->OnEvent(&e);

		node = e.GetVme();
	}

	if (node)
	{
		vtkPolyData *polyData = NULL;
		polyData = (vtkPolyData*)node->GetOutput()->GetVTKData();

		albaMatrix * absMatrix = node->GetOutput()->GetAbsMatrix();
		albaMatrix identityM;

		if (identityM.Equals(absMatrix))
		{
			m_CurrentComponent->SetVTKData(polyData);
		}
		else
		{
			vtkALBASmartPointer <vtkTransform> tra;
			tra->SetMatrix(absMatrix->GetVTKMatrix());

			vtkALBASmartPointer<vtkTransformPolyDataFilter> traFilter;
			traFilter->SetInput(polyData);
			traFilter->SetTransform(tra);
			traFilter->Update();

			m_CurrentComponent->SetVTKData(traFilter->GetOutput());
		}
		m_ComponentName = node->GetName();
		m_ComponentName.Replace("_", " ");

		m_HasVtkData = true;
	}
}
