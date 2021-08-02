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

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class albaProsthesesDBManager;
class albaProDBProshesis;

struct Prosthesis
{
	wxString name;
	wxString image;
	wxString producer;

	int type = 0;
	int side = 0;

	bool isChanged = false;

	std::vector<wxString> componentGroup;
};

//----------------------------------------------------------------------------
// Class Name: appOpCreateProsthesis
//----------------------------------------------------------------------------
class APP_OPERATIONS_EXPORT appOpCreateProsthesis : public albaOp
{
public:
	
	/** Constructor. */
	appOpCreateProsthesis(wxString label = "Create Prosthesis DB");

	/** Destructor. */
	~appOpCreateProsthesis();

	/** RTTI macro. */
	albaTypeMacro(appOpCreateProsthesis, albaOp);

	/** Return a copy of the operation */
	/*virtual*/ albaOp* Copy();

	/** Return true for the acceptable vme type. */
	/*virtual*/ bool Accept(albaVME *node);

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char** GetIcon();

	/** Builds operation's interface. */
	/*virtual*/ void OpRun();
	
protected:

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	/*virtual*/ void OpStop(int result);	
	
	void AddProsthesis();
	void UpdateProsthesis(Prosthesis prosthesis);

	void SaveProsthesis();

	Prosthesis m_CurrentProsthesis;
};
#endif
