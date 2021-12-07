/*=========================================================================
Program:   AssemblerPro
Module:    appOpCreateProsthesisVME.h
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

#ifndef __appOpCreateProsthesisVME_H__
#define __appOpCreateProsthesisVME_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appOperationsDefines.h"
#include "appDecl.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Class Name: appOpCreateProsthesisVME
//----------------------------------------------------------------------------
class APP_OPERATIONS_EXPORT appOpCreateProsthesisVME : public albaOp
{
public:
	/** Constructor. */
	appOpCreateProsthesisVME(wxString label = "Create Prosthesis VME");

	/** Destructor. */
	~appOpCreateProsthesisVME();

	/** RTTI macro. */
	albaTypeMacro(appOpCreateProsthesisVME, albaOp);

	/** Return a copy of the operation */
	/*virtual*/ albaOp* Copy();

	/** Builds operation's interface. */
	/*virtual*/ void OpRun();

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char** GetIcon();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME *node);
};
#endif
