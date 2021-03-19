/*=========================================================================
Program:   AssemblerPro
Module:    appViewImage.h
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

#ifndef __appViewImage_H__
#define __appViewImage_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "appViewsDefines.h"
#include "albaViewImage.h"
#include "albaPipeImage3D.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class albaVME;

//----------------------------------------------------------------------------
// Class: appViewImage
//----------------------------------------------------------------------------

class APP_VIEWS_EXPORT appViewImage : public albaViewImage
{
public:
	appViewImage(wxString label = "View Image", int camera_position = CAMERA_FRONT, bool show_axes = false, bool show_grid = false, int stereo = 0);
	virtual ~appViewImage();

	albaTypeMacro(appViewImage, albaViewImage);

	virtual albaView* Copy(albaObserver *Listener, bool lightCopyEnabled = false);
	
	virtual void Create();

	/**	Set the visualization status for the vme (visible, not visible, mutex, ...) */
	virtual int GetNodeStatus(albaVME *vme);

	albaPipeImage3D *GetPipeImage(albaVME* vme);

	void EnableMutex(bool enable) { m_MutexEnabled = enable; };

protected:
	
	bool m_MutexEnabled;
};
#endif

