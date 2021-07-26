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
class albaProsthesisDBManager;
class albaProDBProshesis;
class albaProDBProducer;

enum ModelType
{
	ACETABULAR,
	FEMORAL,
	MODULAR,
};
enum ModelSide
{
	LEFT,
	RIGHT,
	BOTH,
};

struct Component
{
	wxString name;

	bool isChanged = false;
};

struct Model
{
	int index;
	wxString name;
	wxString image;

	int type = ACETABULAR;
	int side = LEFT;

	bool isChanged = false;

	std::vector<Component> components;
};

struct Producer
{
	int index;
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
	};

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
	void UpdateProducer(Producer producer);

	void SelectModel();
	void AddModel();
	void EditModel();
	void UpdateModel(Model model);

	void SelectComponent();
	void AddComponent();
	void EditComponent();
	void UpdateComponent(Component component);

	albaProsthesisDBManager *m_DBManager;

	std::vector<Producer> m_ProsthesisVect;

	int m_SelectedProducer;
	int m_SelectedModel;
	int m_SelectedComponent;

	Producer m_CurrentProducer;
	Model m_CurrentModel;
	Component m_CurrentComponent;

	// Gui Widgets
	wxComboBox *m_ProducerComboBox;
	std::vector<wxString> m_ProducerNameList;

	wxComboBox *m_ModelComboBox;
	std::vector<wxString> m_ModelNameList;

	wxComboBox *m_ComponentComboBox;
	std::vector<wxString> m_ComponentNameList;
};
#endif
