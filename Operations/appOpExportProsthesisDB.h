/*=========================================================================
Program:   AssemblerPro
Module:    appOpExportProsthesisDB.h
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

#ifndef __appOpExportProsthesisDB_H__
#define __appOpExportProsthesisDB_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appOperationsDefines.h"
#include "appDecl.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class albaGUICheckListBox;

//----------------------------------------------------------------------------
// Class Name: appOpExportProsthesisDB
//----------------------------------------------------------------------------
class APP_OPERATIONS_EXPORT appOpExportProsthesisDB : public albaOp
{
public:
	/** Constructor. */
	appOpExportProsthesisDB(wxString label = "Export Prosthesis DB");

	/** Destructor. */
	~appOpExportProsthesisDB();

	/** RTTI macro. */
	albaTypeMacro(appOpExportProsthesisDB, albaOp);

	/** Return a copy of the operation */
	/*virtual*/ albaOp* Copy();

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char** GetIcon();

	/** Builds operation's interface. */
	/*virtual*/ void OpRun();

	/** Execute the operation. */
	/*virtual*/ void OpDo();

	/** Receive events coming from the user interface.*/
	void OnEvent(albaEventBase *alba_event);
  
protected:


	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME *node);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	/*virtual*/ void OpStop(int result);	

	/** Create the Operation GUI */
	virtual void CreateGui();

	wxString m_DBName;
	wxString m_DBVersion;

	int m_SelectAllProducers;
	int m_SelectAllModels;
};
#endif
