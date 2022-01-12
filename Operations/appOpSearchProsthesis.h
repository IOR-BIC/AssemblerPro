/*=========================================================================
Program:   AssemblerPro
Module:    appOpSearchProsthesis.h
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

#ifndef __appOpSearchProsthesis_H__
#define __appOpSearchProsthesis_H__

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
class albaProsthesesDBManager;
class albaProDBProducer;

//----------------------------------------------------------------------------
// Class Name: appOpSearchProsthesis
//----------------------------------------------------------------------------
class APP_OPERATIONS_EXPORT appOpSearchProsthesis : public albaOp
{
public:

	//Widgets ID's	
	enum OP_TEST_PROSTHESIS_ID
	{
		ID_SEARCH_PROSTHESIS,
	};

	/** Constructor. */
	appOpSearchProsthesis(wxString label = "Search Prosthesis");

	/** Destructor. */
	~appOpSearchProsthesis();

	/** RTTI macro. */
	albaTypeMacro(appOpSearchProsthesis, albaOp);

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
	void UpdateGui();

	void Find();

	wxString m_SearchString;

	albaProsthesesDBManager *m_DBManager;

	wxListBox *m_ResultsListBox;
};
#endif
