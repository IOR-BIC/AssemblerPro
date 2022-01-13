/*=========================================================================
Program:   AssemblerPro
Module:    appOpCreateProsthesis.h
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

#ifndef __appOpCreateProsthesis_H__
#define __appOpCreateProsthesis_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appOperationsDefines.h"
#include "appDecl.h"
#include "albaOp.h"
#include "appVMEProsthesisEdit.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Class Name: appOpCreateProsthesis
//----------------------------------------------------------------------------
class APP_OPERATIONS_EXPORT appOpCreateProsthesis : public albaOp
{
public:
	/** Constructor. */
	appOpCreateProsthesis(wxString label = "Create Prosthesis VME");

	/** Destructor. */
	~appOpCreateProsthesis();

	/** RTTI macro. */
	albaTypeMacro(appOpCreateProsthesis, albaOp);

	/** Return a copy of the operation */
	/*virtual*/ albaOp* Copy();

	/** Builds operation's interface. */
	/*virtual*/ void OpRun();

	void CreateDBProsthesis(appVMEProsthesisEdit * VmeProsthesis);

	appVMEProsthesisEdit * CreateVMEProshesis();

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char** GetIcon();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME *node);

	albaProsthesesDBManager * m_ProsthesesDBManager;

	appVMEProsthesisEdit *GetVMEProsthesisFromTree();

	appVMEProsthesisEdit *m_VmeProsthesis;

	bool m_VMEProsthesisIsCreated;;
};

#endif
