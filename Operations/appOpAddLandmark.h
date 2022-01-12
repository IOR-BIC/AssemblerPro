/*=========================================================================
Program:   AssemblerPro
Module:    appOpAddLandmark.h
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

#ifndef __appOpAddLandmark_H__
#define __appOpAddLandmark_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appOperationsDefines.h"
#include "albaOpAddLandmark.h"
#include "appDecl.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class albaGUIDictionaryWidget;
class albaVMELandmark;
class albaVMELandmarkCloud;

//----------------------------------------------------------------------------
// Class Name: appOpAddLandmark
//----------------------------------------------------------------------------
class APP_OPERATIONS_EXPORT appOpAddLandmark : public albaOpAddLandmark
{
public:
	/** Constructor. */
	appOpAddLandmark(wxString label = "Add Landmark");

	/** Destructor. */
	~appOpAddLandmark();

	/** RTTI macro. */
	albaTypeMacro(appOpAddLandmark, albaOpAddLandmark);

	/** Return a copy of the operation */
	/*virtual*/ albaOp* Copy();

	/** Builds operation's interface. */
	/*virtual*/ void OpRun();

	/** Execute the operation. */
	///*virtual*/ void OpDo();

	/** Receive events coming from the user interface.*/
	//void OnEvent(albaEventBase *alba_event);

protected:

	/** Return true for the acceptable vme type. */
	///*virtual*/ bool InternalAccept(albaVME *node);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	///*virtual*/ void OpStop(int result);	

	/** Create the Operation GUI */
	virtual void CreateGui();
};
#endif
