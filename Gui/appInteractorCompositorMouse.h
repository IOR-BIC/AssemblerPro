/*=========================================================================

 Program: ALBA
 Module: appInteractorCompositorMouse
 Authors: Nicola Vanella
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __appInteractorCompositorMouse_h
#define __appInteractorCompositorMouse_h

#include "albaInteractorCompositorMouse.h"

//----------------------------------------------------------------------------
//forward ref
//----------------------------------------------------------------------------
class albaInteractorCameraMove;
class appInteractorGenericMouse;

//----------------------------------------------------------------------------
/** Class name: appInteractorCompositorMouse
This class derived from albaInteractorCompositorMouse customize its OnEvent method: in some cases,
depending on current view and its camera position, rotational transformation axis are VME axis.
*/
class ALBA_EXPORT appInteractorCompositorMouse : public albaInteractorCompositorMouse
{
public:
  albaTypeMacro(appInteractorCompositorMouse, albaInteractorCompositorMouse);

  /**  Process events coming from the mouse */
  virtual void OnEvent(albaEventBase *event);

	void InitInteraction(int buttonPressed, albaEventInteraction *e);

	void OnButtonDown(albaEventInteraction *e);

	void OnButtonUp(albaEventInteraction *e);
	
	/**  Create a behavior given the activator*/
	albaInteractorGenericMouse *CreateBehavior(MMI_ACTIVATOR activator);
	  
protected:
	typedef std::map<int, albaAutoPointer<appInteractorGenericMouse> > mmuActivatorArbitraryMap;
	mmuActivatorArbitraryMap m_ActivatorInteractorMap;

private:

	int m_MousePose[2];
	int m_LastMousePose[2];
	int m_ButtonPressed;
};
#endif

