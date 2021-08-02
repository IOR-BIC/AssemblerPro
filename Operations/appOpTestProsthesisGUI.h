/*=========================================================================
Program:   AssemblerPro
Module:    appOpTestProsthesisGUI.h
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

#ifndef __appOpTestProsthesisGUI_H__
#define __appOpTestProsthesisGUI_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appOperationsDefines.h"
#include "appDecl.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class albaProsthesesDBManager;
class albaProDBProshesis;
class albaProDBProducer;
class appGUIDialogComponent;
class appGUIDialogProsthesisSelection;
//----------------------------------------------------------------------------
// Class Name: appOpTestProsthesisGUI
//----------------------------------------------------------------------------
class APP_OPERATIONS_EXPORT appOpTestProsthesisGUI : public albaOp
{
public:

	//Widgets ID's	
	enum OP_TEST_PROSTHESIS_ID
	{
		ID_PROSTHESIS_GROUP_START,
		ID_PROSTHESIS_NAME,
		ID_PROSTHESIS_CHANGE,
		ID_PROSTHESIS_EDIT,
		ID_GROUP_SHOW,
		ID_COMPONENT_SELECT,
		ID_COMPONENT_ADD,
		ID_COMPONENT_DEL,
		ID_COMPONENT_EDIT,
		ID_PROSTHESIS_GROUP_END,
	};

	/** Constructor. */
	appOpTestProsthesisGUI(wxString label = "Test Prosthesis GUI");

	/** Destructor. */
	~appOpTestProsthesisGUI();

	/** RTTI macro. */
	albaTypeMacro(appOpTestProsthesisGUI, albaOp);

	/** Return a copy of the operation */
	/*virtual*/ albaOp* Copy();

	/** Return true for the acceptable vme type. */
	/*virtual*/ bool Accept(albaVME *node);

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char** GetIcon();

	/** Builds operation's interface. */
	/*virtual*/ void OpRun();

	void LoadInfo();
	void SaveInfo();

	/** Execute the operation. */
	/*virtual*/ void OpDo();

	/** Receive events coming from the user interface.*/
	void OnEvent(albaEventBase *alba_event);

protected:

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	/*virtual*/ void OpStop(int result);	

	/** Create the Operation GUI */
	virtual void CreateGui();

	void UpdateGui();

	bool m_EditMode;
	
	albaProsthesesDBManager *m_DBManager;
	wxString m_ProsthesisName;
	int m_NumComponentGroups;
	int m_Show;
};
#endif
