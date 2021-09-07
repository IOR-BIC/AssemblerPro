/*=========================================================================
Program:   AssemblerPro
Module:    appOpTestProsthesisGUI.h
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

#ifndef __appOpTestProsthesisGUI_H__
#define __appOpTestProsthesisGUI_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appOperationsDefines.h"
#include "appDecl.h"
#include "albaOp.h"
#include "appGUI.h"
#include <vector>
#include "albaEvent.h"
#include "appGUIDialogComponent.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class albaProsthesesDBManager;
class albaProDBProshesis;
class albaProDBProducer;
class albaProDBCompGruop;
class albaProDBComponent;
class appGUIDialogComponent;
class appGUIDialogProsthesisSelection;
class ComponentGroupGUI;

//----------------------------------------------------------------------------
// Class Name: appOpTestProsthesisGUI
//----------------------------------------------------------------------------
class APP_OPERATIONS_EXPORT appOpTestProsthesisGUI : public albaOp
{
public:

	//Widgets ID's	
	enum OP_TEST_PROSTHESIS_ID
	{
		ID_PROSTHESIS_SELECTION = MINID,
		ID_PROSTHESIS_NAME,
		ID_PROSTHESIS_CHANGE,
		ID_PROSTHESIS_EDIT,

		ID_GROUP_CREATE,
		ID_GROUP_REMOVE,

		ID_GROUP_SHOW,
		ID_GROUP_NAME,

		ID_COMPONENT_SELECT,
		ID_COMPONENT_ADD,
		ID_COMPONENT_DEL,
		ID_COMPONENT_EDIT,
		ID_COMPONENT_MATRIX,
	};

	/** Constructor. */
	appOpTestProsthesisGUI(wxString label = "Test Prosthesis GUI");

	/** Destructor. */
	~appOpTestProsthesisGUI();

	/** RTTI macro. */
	albaTypeMacro(appOpTestProsthesisGUI, albaOp);

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

	void OnEventFromGroup(albaEventBase *alba_event);
	int GetGroup(albaEvent * e);

	/** Create the Operation GUI */
	virtual void CreateGui();

	void UpdateGui();

	void SelectProsthesis();
	void EditProsthesis();
	
	void AddNewGroup();
	void DelGroup();
	void LoadGroups();
	void ClearGroups();
	void EditGroupName();

	albaProsthesesDBManager *m_DBManager;
	
	wxComboBox *m_ProsthesisComboBox;
	std::vector<wxString> m_ProsthesisNameList;
	int m_SelectedProsthesis;
	wxString m_ProsthesisName;

	std::vector<ComponentGroupGUI*> m_ComponentGroupList;
	int m_SelectedGroup;

	appGUI *m_ContentGui;
	appGUI *m_GroupGui;
};

//----------------------------------------------------------------------------
// Class Name: ComponentGroupGUI
//----------------------------------------------------------------------------
class ComponentGroupGUI : public albaObserver
{

public:

	/** Constructor. */
	ComponentGroupGUI();

	/** Destructor. */
	~ComponentGroupGUI();

	/** Receive events coming from the user interface.*/
	void OnEvent(albaEventBase *alba_event);;
	
	appGUI* GetGui();
	void GuiUpdate();

	void SetListener(albaObserver * listener) { m_Listener = listener; };
	
	void SetCompGroup(albaProDBCompGruop *compGroup);
	albaProDBCompGruop* GetCompGroup() { return m_CurrentCompGroup; };

	void SetName(wxString name) { m_GroupName = name; };
	wxString GetName() { return m_GroupName; };

	void ShowGroup();

	void AddNewComponent();
	void DelComponent();
	void EditComponent();

	void EditMatrix();
protected:

	albaObserver *m_Listener;
	appGUI *m_ComponentGui;
	
	albaProDBCompGruop *m_CurrentCompGroup;
	wxString m_GroupName;
	int m_ShowGroup;
	
	wxListBox *m_ComponentListBox;
	std::vector<wxString> m_ComponentsNameList;
	albaProDBComponent *m_CurrentComponent;

	int m_SelectedComponent;
};

#endif
