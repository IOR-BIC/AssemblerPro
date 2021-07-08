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

	void SelectModel();
	void AddModel();
	void EditModel();

	void SelectComponent();
	void AddComponent();
	void EditComponent();

	void UpdateProsthesis();

	wxComboBox *m_ProducerComboBox;
	int m_SelectedProducer;
	std::vector<wxString> m_ProducerList;

	wxComboBox *m_ModelComboBox;
	int m_SelectedModel;
	std::vector<wxString> m_ModelList;

	wxComboBox *m_ComponentsComboBox;
	int m_SelectedComponent;
	std::vector<wxString> m_ComponentList;

	void ShowProducerDialog();
	void HideProducerDialog();
	void UpdateProducerDialog();
	
	// Dialog
	wxString m_ProducerName;
	wxString m_ProducerSite;
	wxString m_ProducerImage;

	bool m_OkButtonPressed;
	bool m_IsProducerDialogOpened;
	albaGUIDialog	*m_ProducerDialog;

	wxComboBox *m_ImageComboBox;
	wxImage	*m_PreviewImage;
	albaGUIPicButton *m_PreviewImageButton;

	wxTextCtrl *m_ProducerName_textCtrl;
	wxTextCtrl *m_ProducerSite_textCtrl;

	albaGUILab *m_ImageNameLab;
	albaGUILab *m_SliceInfoLab;
	albaGUILab *m_TemplateInfoLab;
	albaGUILab *m_LesionInfoLab;
};
#endif
