/*=========================================================================
Program:   AssemblerPro
Module:    appViewImageCompound.h
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

#ifndef __appViewImageCompound_H__
#define __appViewImageCompound_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "appViewsDefines.h"

#include "albaPipeImage3D.h"
#include "albaViewImageCompound.h"

#include "vtkALBASimpleRulerActor2D.h"
#include "vtkRenderWindow.h"
#include <set>

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class albaGUI;
class albaGUIFloatSlider;
class albaGUILutSlider;
class albaGUILutSwatch;
class albaGUIPicButton;
class albaRWIBase;
class albaSceneGraph;
class albaViewImage;

class vtkALBASimpleRulerActor2D;
class vtkImageData;
class vtkImageExport;
class vtkLookupTable;
class vtkWindowLevelLookupTable;
class vtkWindowToImageFilter;

class wxBitmap;
class wxImage;

enum VIEW_OPTIONS
{
	VIEW_RULER = MINID,
};

/**
  Class Name: appViewImageCompound.
  Class that contain albaViewImage in order to add Gui view widget like windowing buttons.
*/
class APP_VIEWS_EXPORT appViewImageCompound: public albaViewImageCompound
{
public:
  /** constructor. */
  appViewImageCompound(wxString label = "View Image Compound", int num_row = 1, int num_col = 1);
  /** destructor. */
  virtual ~appViewImageCompound(); 

	/** RTTI macro.*/
	albaTypeMacro(appViewImageCompound, albaViewImageCompound);

  /**  Redefine this method to package the compounded view */
  virtual void PackageView();

	virtual albaView *Copy(albaObserver *Listener, bool lightCopyEnabled = false);

	bool RulerIsVisible() { return m_ShowRuler; };
  void ShowRuler(bool show);

	/**  Create the GUI on the bottom of the compounded view. */
	virtual void CreateGuiView();

	/** Precess events coming from other objects */
	void OnEvent(albaEventBase *alba_event);

	albaPipeImage3D *GetPipeImage(albaVME* vme);

	void EnableMutex(bool enable);

	/** Show/Hide VMEs into plugged sub-views*/
	virtual void VmeShow(albaVME *vme, bool show);

	/** Function called on VME remove */
	virtual void VmeRemove(albaVME *vme);
	
protected:
		
  void SetRendererByView();

  vtkALBASimpleRulerActor2D *m_Ruler;
	vtkRenderer *m_Renderer;

	albaGUIPicButton *m_RulerButton;

	std::set<vtkLookupTable *> m_LutList;

  bool m_ShowRuler;
};
#endif
