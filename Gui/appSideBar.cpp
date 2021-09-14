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
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "appSideBar.h"
#include "appDecl.h"

#include "albaGUI.h"
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
#include "albaVME.h"
#include "albaVMEOutput.h"
#include "albaVMERoot.h"
#include "albaView.h"
#include "albaViewVTK.h"

//----------------------------------------------------------------------------
appSideBar::appSideBar(albaGUIMDIFrame* parent, int id, albaObserver *Listener, long style)
{
	m_SelectedVme = NULL;
	m_SelectedView = NULL;
	m_CurrentVmeGui = NULL;
	m_CurrentVmeOutputGui = NULL;
	m_CurrentPipeGui = NULL;
	m_AppendingGUI = NULL;

	m_LeftNotebook = NULL;
	m_InfoPanel = NULL;

	m_Parent = parent;
	m_Id = id;
	m_Listener = Listener;
	m_Style = style;

	InitMainPanel();
	InitLeftPanel();
}

//----------------------------------------------------------------------------
void appSideBar::InitMainPanel()
{
	// Main panel  
	m_Notebook = new wxNotebook(m_Parent, m_Id);
	m_Notebook->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT)));

	// Tree Panel
	m_Tree = new albaGUICheckTree(m_Notebook, -1, false, true);
	m_Tree->SetListener(m_Listener);
	m_Tree->SetSize(-1, 300);
	m_Tree->SetTitle(" vme hierarchy: ");
	m_Notebook->AddPage(m_Tree, _("Data tree"), true);

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

	//
	m_Parent->AddDockPane(m_Notebook, wxPaneInfo()
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
void appSideBar::InitLeftPanel()
{
	// Left Notebook  
	m_LeftNotebook = new wxNotebook(m_Parent, MENU_VIEW_INFO_SIDEBAR);
	m_LeftNotebook->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT)));

	// Info panel
	// 	m_InfoPanel = new albaGUIHolder(m_LeftNotebook, -1, false, true);
	// 	m_LeftNotebook->AddPage(m_InfoPanel, _("Info"));

	if (m_Style == DOUBLE_NOTEBOOK)
	{
		m_SideSplittedPanel = new wxSplitterWindow(m_LeftNotebook, -1, wxDefaultPosition, wxSize(-1, -1),/*wxSP_3DSASH |*/ wxSP_FULLSASH);

		m_VmeNotebook = new wxNotebook(m_SideSplittedPanel, -1);
		m_VmeNotebook->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT)));

		m_VmePanel = new albaGUIHolder(m_VmeNotebook, -1, false, true);
		m_VmeNotebook->AddPage(m_VmePanel, _("VME"));

		m_VisualNotebook = new wxNotebook(m_SideSplittedPanel, -1);
		m_VisualNotebook->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT)));

		m_VmePipePanel = new albaGUIHolder(m_VisualNotebook, -1, false, true);
		m_VisualNotebook->AddPage(m_VmePipePanel, _("Visual props"));

		m_SideSplittedPanel->SetMinimumPaneSize(50);
		m_SideSplittedPanel->SplitHorizontally(m_VmeNotebook, m_VisualNotebook);

		m_LeftNotebook->AddPage(m_SideSplittedPanel, _("Info Panel"), true);
	}
	else
	{
		// VME panel
		m_VmePanel = new albaGUIHolder(m_LeftNotebook, -1, false, true);
		m_LeftNotebook->AddPage(m_VmePanel, _("VME"));

		// VME panel
		m_VmePipePanel = new albaGUIHolder(m_LeftNotebook, -1, false, true);
		m_LeftNotebook->AddPage(m_VmePipePanel, _(" Visual props "));
	}

	//
	m_Parent->AddDockPane(m_LeftNotebook, wxPaneInfo()
		.Name("sidebarLeft")
		.Caption(wxT("Info Panel"))
		.Left()
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
void appSideBar::OpShowGui(bool push_gui, albaGUIPanel *panel)
{
	m_Notebook->Show(true);
	if (push_gui)
	{
		m_Notebook->SetSelection(2);
		m_OpPanel->Push(panel);
	}
}
//----------------------------------------------------------------------------
void appSideBar::OpHideGui(bool view_closed)
{
	if (view_closed)
	{
		this->m_Notebook->SetSelection(0);
		this->m_ViewPropertyPanel->RemoveCurrentGui();
		this->m_ViewPropertyPanel->SetTitle("");
	}
	else
	{
		m_OpPanel->Pop();
		m_Notebook->SetSelection(0);
	}
}
//----------------------------------------------------------------------------
void appSideBar::ViewSelect(albaView *view)
{
	m_Tree->ViewSelected(view);
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
	UpdateVmePanel();
}
//----------------------------------------------------------------------------
void appSideBar::ViewDeleted(albaView *view)
{
	m_Tree->ViewDeleted(view);
	ViewSelect(NULL);
}
//----------------------------------------------------------------------------
void appSideBar::EnableSelect(bool enable)
{
	m_Tree->EnableSelect(enable);
}
//----------------------------------------------------------------------------
void appSideBar::VmeAdd(albaVME *vme)
{
	m_Tree->VmeAdd(vme);
}
//----------------------------------------------------------------------------
void appSideBar::VmeRemove(albaVME *vme)
{
	m_Tree->VmeRemove(vme);
	if (vme == m_SelectedVme)
	{
		m_SelectedVme = NULL;
		UpdateVmePanel();
	}
}
//----------------------------------------------------------------------------
void appSideBar::VmeModified(albaVME *vme)
{
	m_Tree->VmeModified(vme);
	if (vme == m_SelectedVme)
		UpdateVmePanel();
}
//----------------------------------------------------------------------------
void appSideBar::VmeShow(albaVME *vme, bool visibility)
{
	m_Tree->VmeShow(vme, visibility);
	if (vme == m_SelectedVme)
		UpdateVmePanel();
}
//----------------------------------------------------------------------------
void appSideBar::VmeSelected(albaVME *vme)
{
	m_SelectedVme = vme;
	UpdateVmePanel();
	m_Tree->VmeSelected(vme);
	m_Tree->SetFocus();

	m_LeftNotebook->SetSelection(0);
}

//----------------------------------------------------------------------------
void appSideBar::Show()
{
	m_Notebook->Show(!m_Notebook->IsShown());
}

//----------------------------------------------------------------------------
void appSideBar::UpdateVmePanel()
{
	albaVMEOutput *vme_out = NULL;
	albaPipe      *vme_pipe = NULL;
	albaGUI       *vme_gui = NULL;
	albaGUI       *vme_out_gui = NULL;
	albaGUI       *vme_pipe_gui = NULL;

	if (m_AppendingGUI && m_CurrentVmeGui)
		m_AppendingGUI->Remove(m_CurrentVmeGui);

	if (m_AppendingGUI && m_CurrentVmeOutputGui)
		m_AppendingGUI->Remove(m_CurrentVmeOutputGui);

	m_AppendingGUI = NULL;

	if (m_SelectedVme)
	{
		vme_gui = m_SelectedVme->GetGui();

		albaVME *v = m_SelectedVme;
		vme_out = v->GetOutput();
		if (!vme_out->IsA("albaVMEOutputNULL")) // Paolo 2005-05-05
		{
			vme_out_gui = vme_out->GetGui();
			if (!v->IsDataAvailable())
				vme_out->Update();
		}
		else
			vme_out = NULL;

		if (m_SelectedView)
		{
			vme_pipe = m_SelectedView->GetNodePipe(m_SelectedVme);

			if (vme_pipe)
				vme_pipe_gui = vme_pipe->GetGui();
		}

		m_CurrentPipeGui = vme_pipe_gui;
		m_CurrentVmeGui = vme_gui;
		m_CurrentVmeOutputGui = vme_out_gui;

		m_AppendingGUI = new albaGUI(NULL);

		if (vme_gui)
		{
			m_AppendingGUI->AddGui(vme_gui);
		}

		if (m_Style == SINGLE_NOTEBOOK && vme_pipe_gui)
		{
			m_AppendingGUI->Label(_("GUI Visual Pipes"), true);
			m_AppendingGUI->AddGui(vme_pipe_gui);
		}

		if (vme_out_gui)
		{
			m_AppendingGUI->Divider(1);
			m_AppendingGUI->Label(_("Output"), true);
			m_AppendingGUI->AddGui(vme_out_gui);
		}

		m_AppendingGUI->FitGui();
		m_AppendingGUI->Update();
	}

	if (m_Style == DOUBLE_NOTEBOOK)
	{
		m_VmePipePanel->Put(vme_pipe_gui);
	}
	if (m_AppendingGUI)
		m_VmePanel->Put(m_AppendingGUI);
	else
		m_VmePanel->Put(new albaGUI(NULL));
}

//----------------------------------------------------------------------------
std::vector<albaVME*> appSideBar::VmeChoose(void *vme_accept_function, long style, albaString title, bool multiSelect, albaVME *vme)
{
	albaGUIVMEChooser vc(m_Tree, title.GetCStr(), vme_accept_function, style, multiSelect, vme);
	return vc.ShowChooserDialog();
}

//----------------------------------------------------------------------------
void appSideBar::FindVME()
{
	albaGUICheckTree *tree = m_Tree;
	albaGUIDialogFindVme fd(_("Find VME"));
	fd.SetTree(tree);
	fd.ShowModal();
}
