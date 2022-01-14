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
class albaProDBProsthesis;
class albaProDBType;
class albaProDBProducer;

//----------------------------------------------------------------------------
// Class Name: appOpExportProsthesisDB
//----------------------------------------------------------------------------
class APP_OPERATIONS_EXPORT appOpExportProsthesisDB : public albaOp
{
public:

	//Widgets ID's	
	enum OP_EXPORTDB
	{
		ID_PRODUCER_SELECTION = MINID,
		ID_TYPE_SELECTION,
		ID_SIDE_SELECTION,
		ID_MODEL_SELECTION,
	};

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
	void UpdateGui();

	void ExportDB();

	bool MakeZip(const albaString &zipname, wxArrayString *files);
	wxString m_DBName;
	wxString m_DBVersion;

	albaProsthesesDBManager *m_ProsthesesDBManager;

	albaGUICheckListBox *m_ProducerCheckBox;
	albaGUICheckListBox *m_TypeCheckBox;
	albaGUICheckListBox *m_SideCheckBox;
	albaGUICheckListBox *m_ProsthesisCheckBox;

	std::vector<albaProDBProducer*> m_ProducerVect;
	std::vector<albaProDBType*> m_TypeVect;
	std::vector<albaProDBProsthesis*> m_ProsthesisVect;

	int m_SelectAllProducers;
	int m_SelectAllTypes;
	int m_SelectAllSides;
	int m_SelectAllModels;
};
#endif
