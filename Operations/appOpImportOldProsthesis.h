/*=========================================================================
Program:   AssemblerPro
Module:    appOpImportOldProsthesis.h
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

#ifndef __appOpImportOldProsthesis_H__
#define __appOpImportOldProsthesis_H__

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
// Class Name: appOpImportOldProsthesis
//----------------------------------------------------------------------------
class APP_OPERATIONS_EXPORT appOpImportOldProsthesis : public albaOp
{
public:
	/** Constructor. */
	appOpImportOldProsthesis(wxString label = "Op Import old Prosthesis");

	/** Destructor. */
	~appOpImportOldProsthesis();

	/** RTTI macro. */
	albaTypeMacro(appOpImportOldProsthesis, albaOp);

	/** Return a copy of the operation */
	/*virtual*/ albaOp* Copy();

	/** Return true for the acceptable vme type. */
	/*virtual*/ bool Accept(albaVME *node);

	/** Builds operation's interface. */
	/*virtual*/ void OpRun();

	/** Execute the operation. */
	/*virtual*/ void OpDo();

protected:

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	/*virtual*/ void OpStop(int result);	

};
#endif
