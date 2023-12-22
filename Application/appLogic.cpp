/*=========================================================================
Program:   AssemblerPro
Module:    appLogic.cpp
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

#include "appDecl.h"
#include "appUtils.h"
#include "appLogic.h"
#include "appSideBar.h"

#include "albaGUIAboutDialog.h"
#include "albaGUIApplicationSettings.h"
#include "albaLogicWithManagers.h"
#include "albaOpManager.h"
#include "albaSnapshotManager.h"
#include "albaVME.h"
#include "albaVMEGroup.h"
#include "albaVMEIterator.h"
#include "albaVMEVolumeGray.h"
#include "albaView.h"
#include "albaViewManager.h"
#include "albaOp.h"

#ifdef USE_WIZARD
#include "albaWizardManager.h"
#endif

#include "vtkObject.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "appVMEProsthesisEdit.h"

#define IDM_WINDOWNEXT 4004
#define IDM_WINDOWPREV 4006

//----------------------------------------------------------------------------
appLogic::appLogic() : albaLogicWithManagers()
{
	m_Win->Maximize();

	m_ProsthesisDBManager = NULL;

	// Set project file extension
	//m_Extension = "alb";
}
//----------------------------------------------------------------------------
appLogic::~appLogic()
{
	// must be deleted after m_VMEManager
	delete m_SideBar;

	delete m_ProsthesisDBManager;
}

/// INIT
//----------------------------------------------------------------------------
void appLogic::Init(int argc, char **argv)
{
	albaLogicWithManagers::Init(argc, argv);
	m_Win->ShowDockPane("logbar", false);

	if (!wxDirExists(albaGetAppDataDirectory()))
		wxMkDir(albaGetAppDataDirectory());

#ifdef _DEBUG
	albaSetAppDebugDir(ASB_SOURCE_DIR);
#endif

	InitAboutDialog();
	
	PlugProsthesisDBManager("d-FUgg&9ogWB,3?UiPb~'sQY@%AsQ");

	// Create and Open View
	ViewCreate(VIEW_SURFACE);

	InitVMEProsthesis();
}
//----------------------------------------------------------------------------
void appLogic::InitAboutDialog()
{
	// Init About Dialog
	m_AboutDialog->SetTitle(m_AppTitle);
	m_AboutDialog->SetBuildNum(m_BuildNum.GetCStr());
	m_AboutDialog->SetVersion("1.0");
	m_AboutDialog->SetWebSite("https://github.com/IOR-BIC");
	//m_AboutDialog->SetLicenseURL("https://github.com/IOR-BIC");
	//m_AboutDialog->SetExtraMessage("");

	// About Image
	wxString imageName = "AppAbout";
	wxString imagesPath = albaGetConfigDirectory();
	m_AboutDialog->SetImagePath(imagesPath + "/" + imageName + ".bmp");
}


//----------------------------------------------------------------------------
void appLogic::InitVMEProsthesis()
{
	albaVMERoot *root = this->m_VMEManager->GetRoot();

	if (root && root->GetLink("VMEProsthesis") == NULL)
		RunOp(GetOp(OP_CREATE_PROSTHESIS));
}
//----------------------------------------------------------------------------
void appLogic::RefreshVMEProsthesis()
{
	appVMEProsthesisEdit *prosthesis = GetVMEProthesis();

	if (prosthesis != NULL)
		prosthesis->Resfresh();
}

//----------------------------------------------------------------------------
appVMEProsthesisEdit * appLogic::GetVMEProthesis()
{
	albaVMERoot *root = this->m_VMEManager->GetRoot();
	appVMEProsthesisEdit *prosthesis = (appVMEProsthesisEdit*)root->GetLink("VMEProsthesis");

	return prosthesis;
}

/// EVENTS
//----------------------------------------------------------------------------
void appLogic::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch (e->GetId())
		{
		case VME_PICKED:
		{
			albaVME* selectedVme = m_OpManager->GetSelectedVme();
		}
		break;

		// View Events
		case ID_SHOW_SURFACE_VIEW:
		{
			ViewCreate(VIEW_SURFACE);
		}
		break;

		// Operations Events - From toolbar
		case OP_IMPORT_PROSTHESIS_DB:
		case OP_EXPORT_PROSTHESIS_DB:
		case OP_CREATE_PROSTHESIS:
		case OP_SEARCH_PROSTHESIS:
 		{
 			m_OpManager->OpRun(e->GetId() - MENU_VIEW_USER_FIRST + OP_USER);
 		}
 		break;
		
		// WX Events
		case wxID_FILE1:
		case wxID_FILE2:
		case wxID_FILE3:
		case wxID_FILE4:
		case wxID_FILE5:
		case wxID_FILE6:
		case wxID_FILE7:
		case wxID_FILE8:
		case wxID_FILE9:
		{
			OnFileHistory(e->GetId());

			ShowVMEOnView();
		}
		break;
		case MENU_FILE_NEW:
			OnFileNew();
			InitVMEProsthesis();
			break;
		case MENU_FILE_OPEN:
		{
			albaString *filename = e->GetString();
			if (filename)
			{
				OnFileOpen((*filename).GetCStr());
			}
			else
			{
				OnFileOpen();
			}

			UpdateFrameTitle();

			InitVMEProsthesis();
			ShowVMEOnView();
		}
		break;
		case ID_GET_FILENAME:
		{
			e->SetString(&(m_VMEManager->GetFileName()));
		}
		break;

		// Toolbar events
		case ID_VIEW_TOOLBAR:
			m_Win->ShowDockPane("viewToolbar", !m_Win->DockPaneIsShown("viewToolbar"));
			break;
		case ID_OPERATIONS_TOOLBAR:
			m_Win->ShowDockPane("operationToolbar", !m_Win->DockPaneIsShown("operationToolbar"));
			break;
		case ID_EXTRA_TOOLBAR:
			m_Win->ShowDockPane("extraToolbar", !m_Win->DockPaneIsShown("extraToolbar"));
			break;
		case MENU_VIEW_INFO_SIDEBAR:
			m_Win->ShowDockPane("sidebarLeft", !m_Win->DockPaneIsShown("sidebarLeft"));
			break;

		default:
			albaLogicWithManagers::OnEvent(alba_event);
			break;
		} // end switch case
	} // end if SafeDowncast
	else
		albaLogicWithManagers::OnEvent(alba_event);
}

/// OPERATIONS
//----------------------------------------------------------------------------
albaOp* appLogic::GetOp(int opId)
{
	return m_OpManager->GetOperationById(opId - MENU_VIEW_USER_FIRST + OP_USER);
}
//----------------------------------------------------------------------------
void appLogic::RunOp(albaOp *op)
{
	m_OpManager->OpRun(op);
}
//----------------------------------------------------------------------------
void appLogic::OpRunTerminated(int runOk)
{
	albaLogicWithManagers::OpRunTerminated(runOk);
}

/// VIEWS
//----------------------------------------------------------------------------
albaView* appLogic::OpenView(int viewId)
{
	ViewCreate(viewId);
	return m_ViewManager->GetSelectedView();
}
//----------------------------------------------------------------------------
void appLogic::ViewCreate(int viewId)
{
	albaView *currentView = NULL;
	
	albaString viewLabel;

	switch (viewId)
	{
	case VIEW_SURFACE: viewLabel = "Surface"; break;
	}

	currentView = m_ViewManager->GetFromList(viewLabel);;
	
	if (!currentView)
	{
		currentView = m_ViewManager->ViewCreate(viewId);
		currentView->GetFrame()->SetMaxSize(currentView->GetFrame()->GetSize());
		ShowVMEOnView();
	}

	currentView->GetFrame()->SetFocus();
}

/// VMEs
//----------------------------------------------------------------------------
void appLogic::ShowVMEOnView()
{
	albaVMERoot *root = this->m_VMEManager->GetRoot();
	albaVMEIterator *iter = root->NewIterator();
	albaVME *volume = NULL;

	for (albaVME *iNode = iter->GetFirstNode(); iNode; iNode = iter->GetNextNode())
	{
		if (iNode->IsA("albaVMEVolumeGray"))
		{
			volume = iNode;
		}
	}
	iter->Delete();

	if (volume)
	{
		VmeShow(volume, true);
	}

	EnableMenuAndToolbar();
}
//----------------------------------------------------------------------------
void appLogic::VmeShow(albaVME *vme, bool visibility)
{
	albaView *currentView = m_ViewManager->GetSelectedView();

	if (currentView)
	{
		if (currentView->IsA("albaViewArbitrarySlice") || currentView->IsA("appViewArbitraryOrthoSlice"))
		{
			if (vme && vme->IsA("albaVMEImage")) return;
		}
	}

	albaLogicWithManagers::VmeShow(vme, visibility);
}
//----------------------------------------------------------------------------
void appLogic::VmeSelect(albaVME *vme)
{
	albaLogicWithManagers::VmeSelect(vme);
}
//----------------------------------------------------------------------------
void appLogic::VmeAdded(albaVME *vme)
{
	albaLogicWithManagers::VmeAdded(vme);

	if (vme->IsA("albaVMEVolumeGray"))
	{
		VmeSelect(vme);
	}
}

/// MENU / TOOLBAR / CONTROL PANEL
//----------------------------------------------------------------------------
void appLogic::CreateMenu()
{
	// Include Icons
#include "pic/menu/EDIT_COPY.xpm"
#include "pic/menu/EDIT_CUT.xpm"
#include "pic/menu/EDIT_DELETE.xpm"
#include "pic/menu/EDIT_PASTE.xpm"
#include "pic/menu/EDIT_REPARENT.xpm"
#include "pic/menu/EDIT_FIND.xpm"
#include "pic/menu/EDIT_REDO.xpm"
#include "pic/menu/EDIT_SETTINGS.xpm"
#include "pic/menu/EDIT_UNDO.xpm"
#include "pic/menu/FILE_EXIT.xpm"
#include "pic/menu/FILE_NEW.xpm"
#include "pic/menu/FILE_OPEN.xpm"
#include "pic/menu/FILE_PRINT.xpm"
#include "pic/menu/FILE_PRINT_PREVIEW.xpm"
#include "pic/menu/FILE_SAVE.xpm"
#include "pic/menu/HELP_ABOUT.xpm"
#include "pic/menu/HELP_HELP.xpm"
#include "pic/menu/HELP_LICENCE.xpm"
#include "pic/menu/WINDOW_CASCADE.xpm"
#include "pic/menu/WINDOW_HORIZONTALLY.xpm"
#include "pic/menu/WINDOW_PREV.xpm"
#include "pic/menu/WINDOW_NEXT.xpm"
#include "pic/menu/WINDOW_VERTICALLY.xpm"

	m_MenuBar = new wxMenuBar;

	// File Menu //////////////////////////////////////////////////////////////////////////
	wxMenu *file_menu = new wxMenu;
	albaGUI::AddMenuItem(file_menu, MENU_FILE_NEW, _("&New  \tCtrl+N"), FILE_NEW_xpm);
	albaGUI::AddMenuItem(file_menu, MENU_FILE_OPEN, _("&Open   \tCtrl+O"), FILE_OPEN_xpm);
	albaGUI::AddMenuItem(file_menu, MENU_FILE_SAVE, _("&Save  \tCtrl+S"), FILE_SAVE_xpm);
	file_menu->Append(MENU_FILE_SAVEAS, _("Save &As  \tCtrl+Shift+S"));

	//file_menu->AppendSeparator();

	// Import menu item
	m_ImportMenu = new wxMenu;
	file_menu->Append(0, _("Import"), m_ImportMenu);

	// Export menu item
	m_ExportMenu = new wxMenu;
	file_menu->Append(0, _("Export"), m_ExportMenu);

	// Print menu item
	file_menu->AppendSeparator();
	albaGUI::AddMenuItem(file_menu, MENU_FILE_PRINT, _("&Print  \tCtrl+P"), FILE_PRINT_xpm);
	albaGUI::AddMenuItem(file_menu, MENU_FILE_PRINT_PREVIEW, _("Print Preview"), FILE_PRINT_PREVIEW_xpm);
	file_menu->Append(MENU_FILE_PRINT_SETUP, _("Printer Setup"));
	file_menu->Append(MENU_FILE_PRINT_PAGE_SETUP, _("Page Setup"));

	// Recent file item
	m_RecentFileMenu = new wxMenu;
	file_menu->AppendSeparator();
	file_menu->Append(0, _("Recent Files"), m_RecentFileMenu);
	file_menu->AppendSeparator();
	albaGUI::AddMenuItem(file_menu, MENU_FILE_QUIT, _("&Quit  \tCtrl+Q"), FILE_EXIT_xpm);
	
	m_MenuBar->Append(file_menu, _("&File"));

	// Edit Menu //////////////////////////////////////////////////////////////////////////
	wxMenu *edit_menu = new wxMenu;
	albaGUI::AddMenuItem(edit_menu, OP_UNDO, _("Undo  \tCtrl+Z"), EDIT_UNDO_xpm);
	albaGUI::AddMenuItem(edit_menu, OP_REDO, _("Redo  \tCtrl+Shift+Z"), EDIT_REDO_xpm);

	edit_menu->AppendSeparator();
	albaGUI::AddMenuItem(edit_menu, OP_CUT, _("Cut   \tCtrl+Shift+X"), EDIT_CUT_xpm);
	albaGUI::AddMenuItem(edit_menu, OP_COPY, _("Copy  \tCtrl+Shift+C"), EDIT_COPY_xpm);
	albaGUI::AddMenuItem(edit_menu, OP_PASTE, _("Paste \tCtrl+Shift+V"), EDIT_PASTE_xpm);
	albaGUI::AddMenuItem(edit_menu, OP_DELETE, _("Delete  \tCtrl+Shift+Del"), EDIT_DELETE_xpm);

	edit_menu->AppendSeparator();
	albaGUI::AddMenuItem(edit_menu, OP_REPARENT, _("Reparent to... \tCtrl+Shift+R"), EDIT_REPARENT_xpm);
	albaGUI::AddMenuItem(edit_menu, MENU_EDIT_FIND_VME, _("Find VME \tCtrl+Shift+F"), EDIT_FIND_xpm);

	albaGUI::AddMenuItem(edit_menu, ID_APP_SETTINGS, _("Settings..."), EDIT_SETTINGS_xpm);
	m_MenuBar->Append(edit_menu, _("&Edit"));

	// View Menu //////////////////////////////////////////////////////////////////////////
	m_ViewMenu = new wxMenu;
	m_MenuBar->Append(m_ViewMenu, _("&View"));

	// Operations Menu ////////////////////////////////////////////////////////////////////
	m_OpMenu = new wxMenu;
	m_MenuBar->Append(m_OpMenu, _("&Operations"));

	// Wizard Menu ////////////////////////////////////////////////////////////////////

	if (m_UseWizardManager)
	{
		m_WizardMenu = new wxMenu;
		m_MenuBar->Append(m_WizardMenu, _("&Wizard"));
	}

	// Window Menu ////////////////////////////////////////////////////////////////////
	wxMenu *windowMenu = new wxMenu;

	albaGUI::AddMenuItem(windowMenu, TILE_WINDOW_CASCADE, _("&Cascade"), WINDOW_CASCADE_xpm);
	albaGUI::AddMenuItem(windowMenu, TILE_WINDOW_HORIZONTALLY, _("Tile &Horizontally"), WINDOW_HORIZONTALLY_xpm);
	albaGUI::AddMenuItem(windowMenu, TILE_WINDOW_VERTICALLY, _("Tile &Vertically"), WINDOW_VERTICALLY_xpm);

	windowMenu->AppendSeparator();
	albaGUI::AddMenuItem(windowMenu, IDM_WINDOWNEXT, _("&Next"), WINDOW_NEXT_xpm);
	albaGUI::AddMenuItem(windowMenu, IDM_WINDOWPREV, _("&Previous"), WINDOW_PREV_xpm);
	
	m_MenuBar->Append(windowMenu, _("Window"));

	// Help Menu //////////////////////////////////////////////////////////////////////////
	wxMenu *help_menu = new wxMenu;
	albaGUI::AddMenuItem(help_menu, ABOUT_APPLICATION, _("About"), HELP_ABOUT_xpm);
	//albaGUI::AddMenuItem(help_menu, REGISTER_PRODUCT, _("Register Product"), HELP_LICENCE_xpm);
	albaGUI::AddMenuItem(help_menu, HELP_HOME, _("Help"), HELP_HELP_xpm);
	m_MenuBar->Append(help_menu, _("&Help"));

	m_Win->SetMenuBar(m_MenuBar);

	EnableItem(MENU_FILE_PRINT, false);
	EnableItem(MENU_FILE_PRINT_PREVIEW, false);
	EnableItem(MENU_FILE_PRINT_SETUP, false);
	EnableItem(MENU_FILE_PRINT_PAGE_SETUP, false);
}
//----------------------------------------------------------------------------
void appLogic::CreateToolbar()
{
	//////////////////////////////////////////////////////////////////////////
	// Create File Toolbar

	m_ToolBar = new wxToolBar(m_Win, MENU_VIEW_TOOLBAR, wxPoint(0, 0), wxSize(-1, -1), wxTB_FLAT | wxTB_NODIVIDER);
	m_ToolBar->SetMargins(0, 0);
	m_ToolBar->SetToolSeparation(2);
	m_ToolBar->SetToolBitmapSize(wxSize(20, 20));

	// File
	m_ToolBar->AddTool(MENU_FILE_NEW, _("New project"), albaPictureFactory::GetPictureFactory()->GetBmp("FILE_NEW"), _("New project"));
	m_ToolBar->AddTool(MENU_FILE_OPEN, _("Open project"), albaPictureFactory::GetPictureFactory()->GetBmp("FILE_OPEN"), _("Open project"));
	m_ToolBar->AddTool(MENU_FILE_SAVE, _("Save project"), albaPictureFactory::GetPictureFactory()->GetBmp("FILE_SAVE"), _("Save project"));
	m_ToolBar->Realize();

	m_Win->AddDockPane(m_ToolBar, wxPaneInfo()
		.Name("toolbar")
		.Caption(wxT("File Toolbar"))
		.Top()
		.Layer(2)
		.ToolbarPane()
		.LeftDockable(false)
		.RightDockable(true)
		.Floatable(false)
		.Movable(false)
		.Gripper(false)
	);

	//////////////////////////////////////////////////////////////////////////
	// Create Snapshot Toolbar

	if (m_UseSnapshotManager)
	{
		m_ExtraToolbar = NULL;
		m_ExtraToolbar = new wxToolBar(m_Win, ID_EXTRA_TOOLBAR, wxPoint(0, 0), wxSize(-1, -1), wxHORIZONTAL | wxTB_FLAT | wxTB_DOCKABLE | wxTB_NODIVIDER);
		m_ExtraToolbar->SetMargins(0, 0);
		m_ExtraToolbar->SetToolSeparation(2);
		m_ExtraToolbar->SetToolBitmapSize(wxSize(20, 20));

		// Snapshot 
		m_ExtraToolbar->AddTool(MENU_FILE_SNAPSHOT, _("Create Snapshot"), albaPictureFactory::GetPictureFactory()->GetBmp("CAMERA"), _("Create Snapshot"));
		m_ExtraToolbar->AddTool(MENU_FILE_MANAGE_SNAPSHOT, _("Manage Snapshots"), albaPictureFactory::GetPictureFactory()->GetBmp("IMAGE_PREVIEW"), _("Manage Snapshots"));
		m_ExtraToolbar->Realize();

		m_Win->AddDockPane(m_ExtraToolbar, wxPaneInfo()
			.Name("extraToolbar")
			.Caption(wxT("Shapshot Toolbar"))
			.Top()
			.Layer(2)
			.ToolbarPane()
			.LeftDockable(true)
			.RightDockable(false)
			.Floatable(false)
			.Movable(true)
			.Gripper(true)
		);
	}

	//////////////////////////////////////////////////////////////////////////
	// Create View Toolbar

	m_ViewToolbar = NULL;
	m_ViewToolbar = new wxToolBar(m_Win, ID_VIEW_TOOLBAR, wxPoint(0, 0), wxSize(-1, -1), wxHORIZONTAL | wxTB_FLAT | wxTB_DOCKABLE | wxTB_NODIVIDER);
	m_ViewToolbar->SetMargins(0, 0);
	m_ViewToolbar->SetToolSeparation(2);
	m_ViewToolbar->SetToolBitmapSize(wxSize(20, 20));

	// Views	
	m_ViewToolbar->AddTool(ID_SHOW_SURFACE_VIEW, _("View Surface"), albaPictureFactory::GetPictureFactory()->GetBmp("VIEW_IMAGE_ICON"), _("View Surface"));
	m_ViewToolbar->Realize();

	m_Win->AddDockPane(m_ViewToolbar, wxPaneInfo()
		.Name("viewToolbar")
		.Caption(wxT("ViewToolbar"))
		.Top()
		.Layer(2)
		.ToolbarPane()
		.LeftDockable(true)
		.RightDockable(false)
		.Floatable(false)
		.Movable(true)
		.Gripper(true)
	);

	//////////////////////////////////////////////////////////////////////////
	// Create Operations Toolbar
	m_OperationToolbar = NULL;
	m_OperationToolbar = new wxToolBar(m_Win, ID_OPERATIONS_TOOLBAR, wxPoint(0, 0), wxSize(-1, -1), wxHORIZONTAL | wxTB_FLAT | wxTB_DOCKABLE | wxTB_NODIVIDER);
	m_OperationToolbar->SetMargins(0, 0);
	m_OperationToolbar->SetToolSeparation(2);
	m_OperationToolbar->SetToolBitmapSize(wxSize(20, 20));

	// Operations
	m_OperationToolbar->AddTool(OP_IMPORT_PROSTHESIS_DB, _("Import Prosthesis DB"), albaPictureFactory::GetPictureFactory()->GetBmp("OP_IMPORT_PROSTHESIS"), _("Import Prosthesis DB"));
	m_OperationToolbar->AddTool(OP_EXPORT_PROSTHESIS_DB, _("Export Prosthesis DB"), albaPictureFactory::GetPictureFactory()->GetBmp("OP_EXPORT_PROSTHESIS"), _("Export Prosthesis DB"));
	m_OperationToolbar->AddSeparator();
	
	m_OperationToolbar->AddTool(OP_CREATE_PROSTHESIS, _("Create Prosthesis"), albaPictureFactory::GetPictureFactory()->GetBmp("OP_CREATE_PROSTHESIS"), _("Create Prosthesis"));
	m_OperationToolbar->AddTool(OP_SEARCH_PROSTHESIS, _("Search Prosthesis"), albaPictureFactory::GetPictureFactory()->GetBmp("OP_SEARCH_PROSTHESIS"), _("Search Prosthesis"));


	m_OperationToolbar->Realize();

	m_Win->AddDockPane(m_OperationToolbar, wxPaneInfo()
		.Name("operationToolbar")
		.Caption(wxT("Operations Toolbar"))
		.Top()
		.Layer(2)
		.ToolbarPane()
		.LeftDockable(true)
		.RightDockable(false)
		.Floatable(false)
		.Movable(true)
		.Gripper(true)
	);
}

//----------------------------------------------------------------------------
void appLogic::CreateAndPlugToolbar()
{
	CreateToolbar();

	m_Win->ShowDockPane("viewToolbar", true);
	m_Win->ShowDockPane("extraToolbar", true);
}
//----------------------------------------------------------------------------
void appLogic::EnableMenuAndToolbar()
{
	bool enable = !(m_RunningOperation || m_WizardRunning);
	
	m_OpManager->EnableOp(enable);

	albaLogicWithManagers::EnableMenuAndToolbar();

	albaVME *node = m_OpManager->GetSelectedVme();

	for (int opId = OP_IMPORT_PROSTHESIS_DB; opId < OP_LAST; opId++)
	{
		//m_ToolBar->EnableTool(opId, enable && node && GetOp(opId)->Accept(node));
		m_OperationToolbar->EnableTool(opId, enable && node && GetOp(opId)->Accept(node));
	}

	if (m_UseSnapshotManager && m_SnapshotManager)
	{
		EnableItem(MENU_FILE_MANAGE_SNAPSHOT, enable && m_SnapshotManager->HasSnapshots(m_VMEManager->GetRoot()));
		EnableItem(MENU_FILE_SNAPSHOT, enable && m_ViewManager->GetSelectedView() != NULL);
	}
}

//----------------------------------------------------------------------------
void appLogic::CreateControlPanel()
{
	m_SidebarStyle = albaSideBar::DOUBLE_NOTEBOOK;
	//m_SideBar = new albaSideBar(m_Win, MENU_VIEW_SIDEBAR, this, m_SidebarStyle); //Default SideBar

	// Create Custom SideBar
	m_SideBar = new appSideBar(m_Win, MENU_VIEW_SIDEBAR, this, m_SidebarStyle);
}