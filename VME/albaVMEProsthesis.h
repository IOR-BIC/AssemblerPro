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

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mmaMaterial;
class vtkAppendPolyData;
class vtkOutlineSource;

/** albaVMEProsthesis - 
*/
class APP_VME_EXPORT albaVMEProsthesis : public albaVMEGeneric
{
public:

	albaTypeMacro(albaVMEProsthesis,albaVMEGeneric);

	/** Precess events coming from other objects */ 
	/*virtual*/ void OnEvent(albaEventBase *alba_event);

	
	/** Return the suggested pipe-typename for the visualization of this vme */
	/*virtual*/ albaString GetVisualPipe() {return albaString("albaPipeGenericPolydata");};

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

	//----------------------------------------------------------------------------
	// Widgets ID's
	//----------------------------------------------------------------------------
	enum PROSTHESIS_COMPONENT_GUI_ID
	{
		ID_START = albaVME::ID_LAST,
	};

protected:
	albaVMEProsthesis();
	virtual ~albaVMEProsthesis();

  /** this creates the Material attribute at the right time... */
  /*virtual*/ int InternalInitialize();

	/** Internally used to create a new instance of the GUI.*/
	/*virtual*/ albaGUI *CreateGui();

private:
	albaVMEProsthesis(const albaVMEProsthesis&); // Not implemented
	void operator=(const albaVMEProsthesis&); // Not implemented
};
#endif
