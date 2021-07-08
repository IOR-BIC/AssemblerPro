/*=========================================================================
Program:   AssemblerPro
Module:    AssemblerPro.cpp
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

//Enable This for memory leak detection
//#include <vld.h>

#include "appDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "appDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "AssemblerPro.h"
#include "appDecl.h"

#include "appOpCreateProsthesis.h"
#include "appOpEmpty.h"

#include "appUtils.h"
#include "appViewImageCompound.h"

#include "albaGUIDicomSettings.h"
#include "albaOpImporterDicom.h"
#include "albaOpTransform.h"
#include "albaPipeFactoryVME.h"
#include "albaPipeVolumeSliceBlend.h"
#include "albaServiceLocator.h"
#include "albaVMEFactory.h" 
#include "albaViewVTK.h"

#include <vtkTimerLog.h>

//Comment this line to disable wizard sample
//#define USE_WIZARD

#ifdef USE_WIZARD
#include "albaWizard.h"
#include "appWizardSample.h"
#endif

//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------

IMPLEMENT_APP(AssemblerPro)

//--------------------------------------------------------------------------------
bool AssemblerPro::OnInit()
{
	InitializeIcons();
	//InitializeReg();

	int result = albaVMEFactory::Initialize();
	assert(result == ALBA_OK);

	result = albaPipeFactoryVME::Initialize();
	assert(result == ALBA_OK);

	// Plug custom VME
	// ...

	// Create Logic Manager
	m_Logic = new appLogic();
	albaServiceLocator::SetLogicManager(m_Logic);
	m_Logic->GetTopWin()->SetTitle("AssemblerPro");

#ifdef USE_WIZARD
	m_Logic->PlugWizardManager(true);
#endif

	// Plug and Enable Snapshot Manager
	m_Logic->PlugSnapshotManager(true);
	m_Logic->Configure();

	SetTopWindow(albaGetFrame());

	//////////////////////////////////////////////////////////////////////////
	//Importers
	//////////////////////////////////////////////////////////////////////////

	// DICOM Importer
	albaGUIDicomSettings *dicomSettings = new albaGUIDicomSettings(NULL, "DICOM");
	m_Logic->Plug(new albaOpImporterDicom("DICOM", true), "", true, dicomSettings);
	dicomSettings->SetSkipCrop(true);
	dicomSettings->SetAutoVMEType(true);
	dicomSettings->SetOutputType(1); //Type 0=Volume 1=Image

	//////////////////////////////////////////////////////////////////////////
	//Exporters
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//Operations
	//////////////////////////////////////////////////////////////////////////	

	// Empty Op
	//m_Logic->Plug(new appOpEmpty("Empty"), "");

	// Create Prosthesis Op
	m_Logic->Plug(new appOpCreateProsthesis("Create Prosthesis"), "");
		
	//////////////////////////////////////////////////////////////////////////
	//Views
	//////////////////////////////////////////////////////////////////////////

	// Image View
	appViewImageCompound *view_image = new appViewImageCompound("Image");
	view_image->PackageView();
	m_Logic->Plug(view_image);

	// VTK View (Surface)
	m_Logic->Plug(new albaViewVTK("Surface"));

#ifdef USE_WIZARD
	//////////////////////////////////////////////////////////////////////////
	//Wizard
	//////////////////////////////////////////////////////////////////////////

	//A simple wizard sample
	m_Logic->Plug(new appWizardSample("Wizard Sample"), "");

#endif //USE_WIZARD

	//////////////////////////////////////////////////////////////////////////

	// Splash Screen
	albaString splashImageName = "AppSplash.bmp";

	wxString splashDir = appUtils::GetConfigDirectory().c_str();
	wxBitmap splashBitmap;
	splashBitmap.LoadFile(splashDir + "\\" + splashImageName, wxBITMAP_TYPE_BMP);
	m_Logic->ShowSplashScreen(splashBitmap);

	//////////////////////////////////////////////////////////////////////////
	wxHandleFatalExceptions();

	m_Logic->Show();
	m_Logic->Init(argc, argv); // Calls FileNew - which create the root

	return TRUE;
}

//--------------------------------------------------------------------------------
int AssemblerPro::OnExit()
{
  cppDEL(m_Logic);

  // This hack is fixing VTK internal memory leak
  vtkTimerLog::CleanupLog();
  return 0;
}

//--------------------------------------------------------------------------------
void AssemblerPro::InitializeIcons()
{
	albaPictureFactory::GetPictureFactory()->Initialize();

	// App Icons
#include "pic/FRAME_ICON16x16.xpm"
	albaADDPIC(FRAME_ICON16x16);
#include "pic/FRAME_ICON32x32.xpm"
	albaADDPIC(FRAME_ICON32x32);
#include "pic/FRAME_ICON64x64.xpm"
	albaADDPIC(FRAME_ICON64x64);
#include "pic/FRAME_ICON128x128.xpm"
	albaADDPIC(FRAME_ICON128x128);
#include "pic/FRAME_ICON256x256.xpm"
	albaADDPIC(FRAME_ICON256x256);
#include "pic/MDICHILD_ICON.xpm"
	albaADDPIC(MDICHILD_ICON);

	// View Icons
#include "pic/CAMERA.xpm"
	albaADDPIC(CAMERA);
#include "pic/VIEW_IMAGE_ICON.xpm"
	albaADDPIC(VIEW_IMAGE_ICON);
#include "pic/SHOW_RULER_ICON.xpm"
	albaADDPIC(SHOW_RULER_ICON);
#include "pic/HIDE_RULER_ICON.xpm"
	albaADDPIC(HIDE_RULER_ICON);
#include "pic/GIZMO_TRANSLATE_ICON.xpm"
	albaADDPIC(GIZMO_TRANSLATE_ICON);
#include "pic/GIZMO_ROTATE_ICON.xpm"
	albaADDPIC(GIZMO_ROTATE_ICON);

	// Operation Icons
#include "pic/IMPORT_DICOM.xpm"
	albaADDPIC(IMPORT_DICOM);	
#include "pic/OP_CREATE_PROSTHESIS.xpm"
	albaADDPIC(OP_CREATE_PROSTHESIS);
}

//--------------------------------------------------------------------------------
// void AssemblerPro::InitializeReg()
// {
//   wxConfig *config = new wxConfig(wxEmptyString);
// 
//   long long_item;
//   wxString string_item;
// 
//   config->SetPath("Importer Dicom"); 
// 
//   if(!config->Read("EnableTimeBar", &long_item))
//   {
//     config->Write("EnableTimeBar",FALSE);
//   }
// 		  
//   if(!config->Read("EnableTypeImage", &long_item))
//   {
//     config->Write("EnableTypeImage",FALSE);
//   }
// 	  
//   cppDEL(config);
// }

//--------------------------------------------------------------------------------
void AssemblerPro::OnFatalException()
{
	m_Logic->HandleException();
}

//--------------------------------------------------------------------------------
int AssemblerPro::FilterEvent(wxEvent& event)
{
	int keyCode = ((wxKeyEvent&)event).GetKeyCode();
	bool controlDown = ((wxKeyEvent&)event).ControlDown();

	// Press Button
	if (event.GetEventType() == wxEVT_KEY_DOWN)
	{
// 		if (keyCode == 'A' && controlDown) // Check CTRL+A Key  
// 		{
// 			//
// 			return true;
// 		}
// 		else if (keyCode == 'O' && !m_Logic->IsOperationRunning()) // Check O Key  
// 		{
// 			//
// 			return true;
// 		}
 	}

	// Release Button
	if (event.GetEventType() == wxEVT_KEY_UP)
	{
// 		if (keyCode == 'O' && !m_Logic->IsOperationRunning()) // Check O Key  
// 		{
// 			//
// 			return true;
// 		}
	}

	return -1;
}
