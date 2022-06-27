/*=========================================================================
Program:   AssemblerPro
Module:    appOpExportOldProsthesis.h
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

#ifndef __appOpExportOldProsthesis_H__
#define __appOpExportOldProsthesis_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appOperationsDefines.h"
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/dom/DOM.hpp>
#include "appDecl.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class albaProDBProsthesis;
//----------------------------------------------------------------------------
// Class Name: appOpExportOldProsthesis
//----------------------------------------------------------------------------
class APP_OPERATIONS_EXPORT appOpExportOldProsthesis : public albaOp
{
public:
	/** Constructor. */
	appOpExportOldProsthesis(wxString label = "Export Old Prosthesis");

	/** Destructor. */
	~appOpExportOldProsthesis();

	/** RTTI macro. */
	albaTypeMacro(appOpExportOldProsthesis, albaOp);

	/** Return a copy of the operation */
	/*virtual*/ albaOp* Copy();

	/** Builds operation's interface. */
	/*virtual*/ void OpRun();

	void ExportProsthesis(wxString tmpFolder, albaProDBProsthesis * dbPro, int side);

	/** Execute the operation. */
	/*virtual*/ void OpDo();

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char** GetIcon();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME *node);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	/*virtual*/ void OpStop(int result);	

	void AddMatrix(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc, XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node, albaMatrix *matrix, const char *nextGroup);


	void ZIPSave(albaString filename, albaString folder);
	bool MakeZip(const albaString &zipname, wxArrayString *files);

	void ConvertFile(albaString source, albaString dest);
};
#endif
