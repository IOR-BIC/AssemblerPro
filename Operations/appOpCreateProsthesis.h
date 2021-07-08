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
class albaGUIButton;
class albaGUILab;
class albaGUIDialog;
class albaGUIPicButton;


struct Component
{
	wxString name;

	bool isChanged = false;
};

struct Model
{
	wxString name;
	wxString image;

	bool isChanged = false;

	std::vector<Component> components;
};

struct Producer
{
	wxString name;
	wxString webSite;
	wxString brandImage;

	bool isChanged = false;

	std::vector<Model> models;
};


//----------------------------------------------------------------------------
// Class Name: appOpCreateProsthesis
//----------------------------------------------------------------------------
class APP_OPERATIONS_EXPORT appOpCreateProsthesis : public albaOp
{
public:

	//Widgets ID's	
	enum OP_CREATE_PROSTHESIS_ID
	{
		ID_PRODUCER_GROUP_START = MINID,
		ID_SELECT_PRODUCER,
		ID_EDIT_PRODUCER,
		ID_ADD_PRODUCER,
		ID_PRODUCER_GROUP_END,

		ID_MODEL_GROUP_START,
		ID_SELECT_MODEL,
		ID_EDIT_MODEL,
		ID_ADD_MODEL,
		ID_MODEL_GROUP_END,

		ID_COMPONENT_GROUP_START,
		ID_SELECT_COMPONENT,
		ID_EDIT_COMPONENT,
		ID_ADD_COMPONENT,
		ID_COMPONENT_GROUP_END,

		ID_PRODUCER_DIALOG_OK_PRESSED,
		ID_PRODUCER_DIALOG_TEXT,
		ID_PRODUCER_DIALOG_IMM,

		ID_MODEL_DIALOG_OK_PRESSED,
		ID_MODEL_DIALOG_TEXT,
		ID_MODEL_DIALOG_IMM,

		ID_COMPONENT_DIALOG_OK_PRESSED,
		ID_COMPONENT_DIALOG_TEXT,
	};

	/** Constructor. */
	appOpCreateProsthesis(wxString label = "Create Prosthesis");

	/** Destructor. */
	~appOpCreateProsthesis();

	/** RTTI macro. */
	albaTypeMacro(appOpCreateProsthesis, albaOp);

	/** Return a copy of the operation */
	/*virtual*/ albaOp* Copy();

	/** Return true for the acceptable vme type. */
	/*virtual*/ bool Accept(albaVME *node);

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

	/** Create the Operation GUI */
	virtual void CreateGui();

	void UpdateGui();

	void SelectProducer();
	void AddProducer();
	void EditProducer();
	void UpdateProducer();

	void SelectModel();
	void AddModel();
	void EditModel();
	void UpdateModel();

	void SelectComponent();
	void AddComponent();
	void EditComponent();
	void UpdateComponent();


	wxComboBox *m_ProducerComboBox;
	int m_SelectedProducer;
	std::vector<wxString> m_ProducerNameList;

	wxComboBox *m_ModelComboBox;
	int m_SelectedModel;
	std::vector<wxString> m_ModelNameList;

	wxComboBox *m_ComponentComboBox;
	int m_SelectedComponent;
	std::vector<wxString> m_ComponentNameList;

	void ShowProducerDialog();
	void HideProducerDialog();
	void UpdateProducerDialog();
	
	void ShowModelDialog();
	void HideModelDialog();
	void UpdateModelDialog();

	void ShowComponentDialog();
	void HideComponentDialog();
	void UpdateComponentDialog();

	std::vector<Producer> m_ProsthesisVect;

	// Product Dialog
	wxString m_CurrentProducerName;
	wxString m_CurrentProducerSite;
	wxString m_CurrentProducerImage;
	
	albaGUIDialog	*m_ProducerDialog;

	wxComboBox *m_ProducerImageComboBox;
	wxImage	*m_ProducerImage;
	albaGUIPicButton *m_ProducerImageButton;

	wxTextCtrl *m_ProducerName_textCtrl;
	wxTextCtrl *m_ProducerSite_textCtrl;

	bool m_ProducerOkButtonPressed;
	bool m_IsProducerDialogOpened;

	// Model Dialog
	wxString m_CurrentModelName;
	wxString m_CurrentModelImage;

	albaGUIDialog	*m_ModelDialog;

	wxComboBox *m_ModelImageComboBox;
	wxImage	*m_ModelImage;
	albaGUIPicButton *m_ModelImageButton;

	wxTextCtrl *m_ModelName_textCtrl;

	bool m_ModelOkButtonPressed;
	bool m_IsModelDialogOpened;

	// Component Dialog
	wxString m_CurrentComponentName;
	wxString m_CurrentComponentImage;

	albaGUIDialog	*m_ComponentDialog;

	wxTextCtrl *m_ComponentName_textCtrl;

	bool m_ComponentOkButtonPressed;
	bool m_IsComponentDialogOpened;
};
#endif
