/*=========================================================================
Program:   AssemblerPro
Module:    appOpManageProducer.h
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

#ifndef __appOpManageProducer_H__
#define __appOpManageProducer_H__

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
class albaProDBProducer;

struct Producer
{
	int index;
	wxString name;
	wxString webSite;
	wxString image;

	bool isChanged = false;
};

//----------------------------------------------------------------------------
// Class Name: appOpManageProducer
//----------------------------------------------------------------------------
class APP_OPERATIONS_EXPORT appOpManageProducer : public albaOp
{
public:

	//Widgets ID's	
	enum OP_CREATE_PROSTHESIS_ID
	{
		ID_PRODUCER_GROUP_START = MINID,
		ID_SELECT_PRODUCER,
		ID_EDIT_PRODUCER,
		ID_ADD_PRODUCER,
		ID_PRODUCER_GROUP_END,
	};

	/** Constructor. */
	appOpManageProducer(wxString label = "Manage Prosthesis Producer");

	/** Destructor. */
	~appOpManageProducer();

	/** RTTI macro. */
	albaTypeMacro(appOpManageProducer, albaOp);

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

	void SelectProducer();
	void AddProducer();
	void EditProducer();
	void UpdateProducer(Producer producer);

	albaProsthesesDBManager *m_DBManager;

	std::vector<Producer> m_ProducersVect;

	int m_SelectedProducer;

	Producer m_CurrentProducer;

	// Gui Widgets
	wxComboBox *m_ProducerComboBox;
	std::vector<wxString> m_ProducerNameList;
};
#endif
