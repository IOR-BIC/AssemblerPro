/*=========================================================================
Program:   AssemblerPro
Module:    appGUITransformMouse.h
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

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#ifndef __appGuiTransformMouse_H__
#define __appGuiTransformMouse_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "appCommonDefines.h"
#include "albaObserver.h"
#include "albaGUITransformMouse.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaView;
class appInteractorGenericMouse;
class appInteractorCompositorMouse;

//----------------------------------------------------------------------------
/** Class name: appGuiTransformMouse
This class derived from albaGUITransformMouse customize its OnEvent method: in some cases, 
depending on current view and its camera position, rotational transformation axis are VME axis.
*/

class APP_COMMON_EXPORT appGUITransformMouse : public albaGUITransformMouse
{
public:
  appGUITransformMouse(albaVME *input, albaObserver *listener = NULL);
  ~appGUITransformMouse(); 

	void UpdateISA();

	/** Override superclass */
  void OnEvent(albaEventBase *alba_event);

	appInteractorGenericMouse* CreateBehavior(MMI_ACTIVATOR activator);

protected:

	bool IsSliceView(albaView *v);
};
#endif
