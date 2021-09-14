/*=========================================================================
Program:   AssemblerPro
Module:    albaVMEProsthesis.h
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
#ifndef __albaVMEProsthesis_h
#define __albaVMEProsthesis_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "appVMEDefines.h"
#include "albaVMEGeneric.h"
#include "appGUI.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mmaMaterial;
class vtkAppendPolyData;
class vtkOutlineSource;
class vtkTransform;
class vtkTransformPolyDataFilter;
class albaProDBCompGroup;
class albaProDBProsthesis;

/** albaVMEProsthesis - 
*/
class APP_VME_EXPORT albaVMEProsthesis : public albaVMEGeneric
{
public:

	albaTypeMacro(albaVMEProsthesis,albaVMEGeneric);

	/** Precess events coming from other objects */ 
	/*virtual*/ void OnEvent(albaEventBase *alba_event);

	
	/** Return the suggested pipe-typename for the visualization of this vme */
	virtual albaString GetVisualPipe() { return albaString("albaPipeSurface"); };

	/**
	Return the list of timestamps for this VME. Timestamps list is 
	obtained merging timestamps for matrixes and VME items*/
	/*virtual*/ void GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes);

	/** return always false since (currently) the slicer is not an animated VME (position 
	is the same for all timestamps). */
	/*virtual*/ bool IsAnimated();

	/** Return pointer to material attribute. */
	mmaMaterial *GetMaterial();
		
  /** Return VME output. */
	albaVMEOutput *GetOutput();

	/** return an xpm-icon that can be used to represent this node */
	static char ** GetIcon();

	void SetProsthesis(albaProDBProsthesis *prosthesis);

	//----------------------------------------------------------------------------
	// Widgets ID's
	//----------------------------------------------------------------------------
	enum PROSTHESIS_GUI_ID
	{
		ID_START = albaVME::ID_LAST,
		ID_PROSTHESIS_NAME,
		ID_PROSTHESIS_CHANGE,
		ID_LAST
	};

	enum PROSTHESIS_COMP_GUI_ID
	{
		ID_SHOW_COMPONENT,
		ID_SELECT_COMPONENT,
		ID_LAST_COMP_ID
	};

protected:
	albaVMEProsthesis();
	virtual ~albaVMEProsthesis();

  /** this creates the Material attribute at the right time... */
  /*virtual*/ int InternalInitialize();

	/** Internally used to create a new instance of the GUI.*/
	/*virtual*/ albaGUI *CreateGui();

	void UpdateGui();

	void AddComponentGroup(albaProDBCompGroup *componentGroup);

	void ClearComponentGroups();

	virtual void OnComponentEvent(int compGroup, int id);

	//Components vtkData
	std::vector <vtkTransformPolyDataFilter *> m_TransformFilters;
	std::vector <vtkTransform *> m_Transforms; 
	
	//Components Gui
	std::vector <albaGUI *> m_ComponentGui;
	appGUI *m_GroupGui;
	appGUI *m_ContentGui;

	int m_ShowComponents[100]; //need to use an array because vector pointers can change at runtime
	std::vector <wxListBox *> m_ComponentListBox;


	vtkAppendPolyData *m_AppendPolydata;
	albaProDBProsthesis *m_Prosthesis;
	albaString m_ProsthesisName;

private:
	albaVMEProsthesis(const albaVMEProsthesis&); // Not implemented
	void operator=(const albaVMEProsthesis&); // Not implemented
};
#endif
