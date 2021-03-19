/*=========================================================================
Program:   AssemblerPro
Module:    appSideBar.cpp
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2021 (http://www.ltmsoftware.org/alba.htm)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "appDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "appDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "appSideBar.h"
#include "appDecl.h"
#include "appLogic.h"
#include "appUtils.h"

#include "albaGUI.h"
#include "albaGUIApplicationSettings.h"
#include "albaGUIDialogFindVme.h"
#include "albaGUIDockManager.h"
#include "albaGUIHolder.h"
#include "albaGUIMDIFrame.h"
#include "albaGUIPanel.h"
#include "albaGUIPanelStack.h"
#include "albaGUISplittedPanel.h"
#include "albaGUITree.h"
#include "albaGUIVMEChooser.h"
#include "albaPipe.h"
#include "albaTagArray.h"
#include "albaTransform.h"
#include "albaVME.h"
#include "albaVMEGroup.h"
#include "albaVMEIterator.h"
#include "albaVMEOutput.h"
#include "albaVMERoot.h"
#include "albaView.h"
#include "albaViewVTK.h"

//----------------------------------------------------------------------------
appSideBar::appSideBar(albaGUIMDIFrame* parent, int id, albaObserver *Listener)
{
	m_SelectedVme = NULL;
	m_OldSelectedVme = NULL;
	m_SelectedView = NULL;
	m_CurrentStatusGui = NULL;
	m_AppendingGUI = NULL;

	m_Listener = Listener;

	// Notebook
	m_Notebook = new wxNotebook(parent, id);
	m_Notebook->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT)));

	// VME panel
	m_StatusPanel = new albaGUIHolder(m_Notebook, -1, false, true);
	m_Notebook->AddPage(m_StatusPanel, _("Status"));

	// View property panel
	m_ViewPropertyPanel = new albaGUIHolder(m_Notebook, -1, false, true);
	m_ViewPropertyPanel->SetTitle(_("No view selected:"));
	m_Notebook->AddPage(m_ViewPropertyPanel, _("View settings"));

	// Operation panel
	m_OpPanel = new albaGUIPanelStack(m_Notebook, -1);
	albaGUINamedPanel *empty_op = new albaGUINamedPanel(m_OpPanel, -1, false, true);
	empty_op->SetTitle(_(" No operation running:"));
	m_OpPanel->Push(empty_op);
	m_Notebook->AddPage(m_OpPanel, _("Operation"));

	// VME Tree
	//m_Tree = NULL;
	//m_Tree = new albaGUICheckTree(m_Notebook, 100, false, true);
	//m_Tree->SetListener(Listener);
	//m_Tree->SetSize(-1, 300);
	//m_Tree->SetTitle(" vme hierarchy: ");

	parent->AddDockPane(m_Notebook, wxPaneInfo()
		.Name("sidebar")
		.Caption(wxT("Control Panel"))
		.Right()
		.Layer(2)
		.MinSize(240, 450)
		.TopDockable(false)
		.BottomDockable(false)
	);
}
//----------------------------------------------------------------------------
appSideBar::~appSideBar()
{
}

//----------------------------------------------------------------------------
void appSideBar::Show()
{
	m_Notebook->Show(!m_Notebook->IsShown());
}

/// OPERATION ////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
void appSideBar::OpShowGui(bool push_gui, albaGUIPanel *panel)
{
	m_Notebook->Show(true);

	if(push_gui)
	{
		m_Notebook->SetSelection(2);
		m_OpPanel->Push(panel);
	}

	if (m_CurrentStatusGui)
	{
		// Disable Status Panel during operation
		m_CurrentStatusGui->Disable();
	}
}
//----------------------------------------------------------------------------
void appSideBar::OpHideGui(bool view_closed)
{
	if(view_closed)
	{
		m_Notebook->SetSelection(0);
		m_ViewPropertyPanel->RemoveCurrentGui();
		m_ViewPropertyPanel->SetTitle("");
	}
	else
	{
		m_OpPanel->Pop();
		m_Notebook->SetSelection(0);
	}

	m_OldSelectedVme = NULL;
	UpdateStatusPanel();
}

/// VIEW /////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
void appSideBar::ViewSelect(albaView *view)
{
	//if(m_Tree)	m_Tree->ViewSelected(view);

	if (view)
	{
		wxString s = " ";
		s += wxStripMenuCodes(view->GetLabel());
		s += " props:";
		m_ViewPropertyPanel->SetTitle(s);
		albaGUI *gui = view->GetGui();
		if (gui)
			m_ViewPropertyPanel->Put(gui);
		else
			m_ViewPropertyPanel->RemoveCurrentGui();
	}
	else
	{
		m_ViewPropertyPanel->SetTitle(_("No view selected:"));
		m_ViewPropertyPanel->RemoveCurrentGui();
	}

	m_SelectedView = view;
	UpdateStatusPanel();

	//m_Notebook->SetSelection(1);
}
//----------------------------------------------------------------------------
void appSideBar::ViewDeleted(albaView *view)
{
	//if (m_Tree)	m_Tree->ViewDeleted(view);
	ViewSelect(NULL);
}

//----------------------------------------------------------------------------
void appSideBar::EnableSelect(bool enable)
{
	//if (m_Tree)	m_Tree->EnableSelect(enable);
}

/// VME //////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void appSideBar::VmeAdd(albaVME *vme)
{
	//if(m_Tree)	m_Tree->VmeAdd(vme);
	UpdateStatusPanel();
}
//----------------------------------------------------------------------------
void appSideBar::VmeRemove(albaVME *vme)
{
	//if (m_Tree)	m_Tree->VmeRemove(vme);
	UpdateStatusPanel();
}
//----------------------------------------------------------------------------
void appSideBar::VmeModified(albaVME *vme)
{
	//m_Tree->VmeModified(vme);
	UpdateStatusPanel();
}
//----------------------------------------------------------------------------
void appSideBar::VmeShow(albaVME *vme, bool visibility)
{
	//if (m_Tree)	m_Tree->VmeShow(vme, visibility);
	UpdateStatusPanel();
}
//----------------------------------------------------------------------------
void appSideBar::VmeSelected(albaVME *vme)
{
  m_SelectedVme = vme;

  /*if (m_Tree) {
	  m_Tree->VmeSelected(vme);
	  m_Tree->SetFocus();
  }*/

  UpdateStatusPanel();
}
//----------------------------------------------------------------------------
std::vector<albaVME*> appSideBar::VmeChoose(void *vme_accept_function, long style, albaString title, bool multiSelect, albaVME *vme)
{
	//albaGUIVMEChooser vc(m_Tree, title.GetCStr(), vme_accept_function, style, multiSelect, vme);
	//return vc.ShowChooserDialog();

	albaErrorMessage("Error on choose VME");
	std::vector<albaVME*> emptyList;
	return emptyList;
}
//----------------------------------------------------------------------------
void appSideBar::FindVME()
{
	//albaGUICheckTree *tree = m_Tree;
	//albaGUIDialogFindVme fd(_("Find VME"));
	//fd.SetTree(tree);
	//fd.ShowModal();

	albaErrorMessage("Error on find VME");
}

/// STATUS ///////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void appSideBar::UpdateStatusPanel()
{
	if (m_SelectedVme == m_OldSelectedVme) 
		return;

	m_OldSelectedVme = m_SelectedVme;

 	if (m_AppendingGUI && m_CurrentStatusGui)
 		m_AppendingGUI->Remove(m_CurrentStatusGui);

	m_AppendingGUI = NULL;
	m_AppendingGUI = new albaGUI(NULL);
	
	if (m_SelectedVme)
	{
		//vme_gui = m_SelectedVme->GetGui();	

		UpdateStatus();		
		m_AppendingGUI->AddGui(m_CurrentStatusGui);
	}

	//m_CurrentVmeGui = vme_gui;
		
	m_AppendingGUI->Label("");
	m_AppendingGUI->FitGui();
	m_AppendingGUI->Update();

	m_StatusPanel->Put(m_AppendingGUI);
	m_AppendingGUI->SetListener(m_Listener);
}
//----------------------------------------------------------------------------
void appSideBar::UpdateStatus()
{
	
}
