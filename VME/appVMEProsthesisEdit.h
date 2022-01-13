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
class mmaMaterial;
class vtkAppendPolyData;
class vtkOutlineSource;
class vtkTransform;
class vtkTransformPolyDataFilter;

/** appVMEProsthesisEdit - 
*/
class APP_VME_EXPORT appVMEProsthesisEdit : public albaVMEProsthesis
{
public:

	albaTypeMacro(appVMEProsthesisEdit, albaVMEProsthesis);

	/** Precess events coming from other objects */ 
	/*virtual*/ void OnEvent(albaEventBase *alba_event);

	/** return an xpm-icon that can be used to represent this node */
	static char ** GetIcon();

	void Reset();
	void SetSelection(int selection);

	//----------------------------------------------------------------------------
	// Widgets ID's
	//----------------------------------------------------------------------------
	enum PROSTHESIS_EDIT_GUI_ID
	{
		ID_START = ID_LAST,
		ID_PROSTHESIS_SELECTION,
		ID_PROSTHESIS_EDIT,
		ID_GROUP_CREATE,
		ID_LAST,
	};

	enum PROSTHESIS_COMP_EDIT_GUI_ID
	{
		ID_ADD_COMPONENT = ID_LAST_COMP_ID,
		ID_NAME_COMPONENT,
		ID_REM_COMPONENT,
		ID_EDIT_COMPONENT,
		ID_MATRIX_COMPONENT,
		ID_REM_COMPONENT_GROUP,
		ID_LAST_COMP_ID
	};

protected:
	appVMEProsthesisEdit();
	virtual ~appVMEProsthesisEdit();

	/*virtual*/ int InternalInitialize();

	/** Internally used to create a new instance of the GUI.*/
	/*virtual*/ albaGUI *CreateGui();
	virtual void CreateComponentGui(int currGroup, albaProDBCompGroup * componentGroup);

	void UpdateGui();

	virtual void OnComponentEvent(int compGroup, int id);

	void SelectProsthesis();
	void EditProsthesis(albaProDBProsthesis *prosthesis);

	void CreateNewComponentGroup();
	void RenameComponentGroup(int compGroup);
	void DeleteComponentGroup(int compGroup);

	void AddNewComponent(int compGroup);
	void RemoveComponent(int compGroup);
	void EditComponent(int compGroup);
	void EditComponentMatrix(int compGroup);

	albaProsthesesDBManager *m_DBManager;

	wxComboBox *m_ProsthesisComboBox;
	int m_SelectedProsthesis;

private:
	appVMEProsthesisEdit(const appVMEProsthesisEdit&); // Not implemented
	void operator=(const appVMEProsthesisEdit&); // Not implemented
};
#endif
