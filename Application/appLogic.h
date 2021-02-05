/*=========================================================================
Program:   Assembler
Module:    appLogic.h
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

#ifndef __appLogic_H__
#define __appLogic_H__

//----------------------------------------------------------------------------
// Includes :
//----------------------------------------------------------------------------
#include "albaLogicWithManagers.h"

//----------------------------------------------------------------------------
// Forward reference :
//----------------------------------------------------------------------------
class albaOp;
class albaGUIApplicationSettings;

//----------------------------------------------------------------------------
// Class Name: appLogic
//----------------------------------------------------------------------------
class appLogic: public albaLogicWithManagers
{
public:

  /** Constructor. */
	appLogic();

  /** Destructor. */
	virtual ~appLogic(); 

	/*virtual*/ void OnEvent(albaEventBase *alba_event);
		
	/** Program Initialization */
	/*virtual*/ void Init(int argc, char **argv);

	void RunOp(albaOp *op);

	void OpRunTerminated(int runOk);

	albaOp* GetOp(int opId);

	/** Respond to a VME_SELECT event. Update the selection on the tree and view representation.*/
	virtual void VmeSelect(albaVME *vme);

	/** Respond to a VME_ADDED event. propagate event. to SideBar, ViewManager, ecc.. */
	virtual void VmeAdded(albaVME *vme);

	albaDeviceButtonsPadMouse* GetMouse() { return m_Mouse; };

	albaGUIApplicationSettings* GetSettings() { return m_ApplicationSettings; };

	albaView* OpenView(int viewId);

protected:
  
	/** Respond to a VME_SHOW event. Show/Hide the vme */
	virtual void VmeShow(albaVME *vme, bool visibility);
	  	
  /** Redefined to add Meter controls*/
  virtual void CreateToolbar();
  
	/** This method creates and add the Toolbar, for specific app toolbar override the CreateToolBar method*/
	void CreateAndPlugToolbar();

	/** Enable/Disable menu items when an operation start/end running */
	virtual void EnableMenuAndToolbar();

	/** Show arbitrary view vertical tiled with blend ct view */
  void ShowVMEOnView();
	
	/** A new View has Been Created - Logic must create the View Frame */
	virtual void ViewCreate(int viewId);

	/** Virtual method to create the side bar */
	virtual void CreateControlPanel();

	/** Redefined to add View,Op, Import, Export menu */
	virtual void CreateMenu();

	/** Show contextual menu for views when right mouse click arrives */
	virtual void ViewContextualMenu(bool vme_menu) {};

	/* Initialize About Dialog */
	void InitAboutDialog();

  bool m_OpeningMSF;

	wxToolBar *m_ViewToolbar;
	wxToolBar *m_OperationToolbar;
	wxToolBar *m_ExtraToolbar;

private:
	//
};
#endif
