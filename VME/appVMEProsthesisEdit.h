/*=========================================================================
Program:   AssemblerPro
Module:    appVMEProsthesisEdit.h
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Gianluigi Crimi
==========================================================================
Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/
#ifndef __appVMEProsthesisEdit_h
#define __appVMEProsthesisEdit_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "appVMEDefines.h"
#include "albaVMEProsthesis.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

/** appVMEProsthesisEdit - 
*/
class APP_VME_EXPORT appVMEProsthesisEdit : public albaVMEProsthesis
{
public:

	albaTypeMacro(appVMEProsthesisEdit, albaVMEProsthesis);

	/** Precess events coming from other objects */ 
	/*virtual*/ void OnEvent(albaEventBase *alba_event);


	//----------------------------------------------------------------------------
	// Widgets ID's
	//----------------------------------------------------------------------------
	enum PROSTHESIS_COMPONENT_GUI_ID
	{
		ID_START = ID_LAST,
		ID_PROSTHESIS_SELECTION,
		ID_PROSTHESIS_EDIT,
		ID_GROUP_CREATE,
		ID_GROUP_REMOVE,
	};

protected:
	appVMEProsthesisEdit();
	virtual ~appVMEProsthesisEdit();

	/*virtual*/ int InternalInitialize();

	void UpdateGui();

	/** Internally used to create a new instance of the GUI.*/
	/*virtual*/ albaGUI *CreateGui();

	void SelectProsthesis();
	void EditProsthesis();

	albaProsthesesDBManager *m_DBManager;

	wxComboBox *m_ProsthesisComboBox;
	int m_SelectedProsthesis;

private:
	appVMEProsthesisEdit(const appVMEProsthesisEdit&); // Not implemented
	void operator=(const appVMEProsthesisEdit&); // Not implemented
};
#endif
