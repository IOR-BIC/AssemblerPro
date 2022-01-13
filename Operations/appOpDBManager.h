/*=========================================================================
Program:   AssemblerPro
Module:    appOpDBManager.h
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

#ifndef __appOpDBManager_H__
#define __appOpDBManager_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appOperationsDefines.h"
#include "appDecl.h"
#include "albaOp.h"
#include "albaProsthesesDBManager.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Class Name: appOpDBManager
//----------------------------------------------------------------------------
class APP_OPERATIONS_EXPORT appOpDBManager : public albaOp
{
public:
	//Widgets ID's	
	enum OP_DBMANAGER
	{		 
		ID_PRODUCER_SELECTION = MINID,
		ID_TYPE_SELECTION,
		ID_PROSTHESIS_SELECTION,
		ID_DELETE_PRODUCER,
		ID_DELETE_TYPE,
		ID_DELETE_PROSTHESIS,
		ID_LOAD,
		ID_SAVE,
		ID_CLEAR,
	};

	/** Constructor. */
	appOpDBManager(wxString label = "DB Manager");

	/** Destructor. */
	~appOpDBManager();

	/** RTTI macro. */
	albaTypeMacro(appOpDBManager, albaOp);

	/** Return a copy of the operation */
	/*virtual*/ albaOp* Copy();

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

	albaProsthesesDBManager * m_ProsthesesDBManager;

	wxComboBox *m_ProducerComboBox;
	int m_SelectedProducer;
	
	wxComboBox *m_TypeComboBox;
	int m_SelectedType;

	wxComboBox *m_ProsthesisComboBox;
	int m_SelectedProsthesis;

};
#endif
