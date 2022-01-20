/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: appInteractorGenericMouse.h,v $
Language:  C++
Date:      $Date: 2011-05-27 07:52:12 $
Version:   $Revision: 1.1.2.2 $
Authors:   Gianluigi Crimi, Nicola Vanella
==========================================================================
Copyright (c) 2010
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __appInteractorGenericMouse_h
#define __appInteractorGenericMouse_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaInteractorGenericMouse.h"

//----------------------------------------------------------------------------
//forward ref
//----------------------------------------------------------------------------
class albaDeviceButtonsPadMouse;


class ALBA_EXPORT appInteractorGenericMouse : public albaInteractorGenericMouse
{
public:
  albaTypeMacro(appInteractorGenericMouse, albaInteractorGenericMouse);
  
  /** Set the current camera */
  //virtual void SetCurrentCamera(vtkCamera *camera) {m_CurrentCamera=camera;}

protected:

  appInteractorGenericMouse();
  ~appInteractorGenericMouse();

  //----------------------------------------------------------------------------
  // trackball interaction style stuff 
  //----------------------------------------------------------------------------

  virtual void TrackballRotate();
  //void TrackballTranslate();
	virtual void TrackballRoll();

private:

};
#endif
