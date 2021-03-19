/*=========================================================================
Program:   AssemblerPro
Module:    appViewImage.cpp
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

#include "appDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "appDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "appViewImage.h"
#include "appDecl.h"
#include "albaVMEImage.h"
#include "albaVMESurface.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(appViewImage);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
appViewImage::appViewImage(wxString label, int camera_position, bool show_axes, bool show_grid, int stereo)
	:albaViewImage(label, camera_position, show_axes, show_grid, stereo)
{
	m_MutexEnabled = false;
}
//----------------------------------------------------------------------------
appViewImage::~appViewImage()
{

}

//----------------------------------------------------------------------------
albaView *appViewImage::Copy(albaObserver *Listener, bool lightCopyEnabled)
{
	m_LightCopyEnabled = lightCopyEnabled;

	appViewImage *v = new appViewImage(m_Label, m_CameraPositionId, m_ShowAxes, m_ShowGrid, m_StereoType);
	v->m_Listener = Listener;
	v->m_Id = m_Id;
	v->m_PipeMap = m_PipeMap;
	v->m_LightCopyEnabled = lightCopyEnabled;
	v->Create();

	//v->CreateGuiView();

	return v;
}

//----------------------------------------------------------------------------
void appViewImage::Create()
{
	Superclass::Create();

	PlugVisualPipe("albaVMEImage", "albaPipeImage3D", VISIBLE);
}

//-------------------------------------------------------------------------
int appViewImage::GetNodeStatus(albaVME *vme)
{
	albaSceneNode *n = NULL;
	if (m_Sg != NULL)
	{
		n = m_Sg->Vme2Node(vme);
		if (vme->IsALBAType(albaVMEImage) || vme->IsA("albaVMEAdvancedProber") || vme->IsALBAType(albaVMESurface))
		{
			if (n != NULL)
			{
				n->SetMutex(m_MutexEnabled);
			}
		}
		else
		{
			if (n != NULL)
			{
				n->SetPipeCreatable(false);
			}
		}
	}

	return m_Sg ? m_Sg->GetNodeStatus(vme) : NODE_NON_VISIBLE;
}

//----------------------------------------------------------------------------
albaPipeImage3D * appViewImage::GetPipeImage(albaVME* vme)
{
	if (vme->IsA("albaVMEImage"))
	{
		albaPipeImage3D *pipe = (albaPipeImage3D *)GetNodePipe(vme);
		return pipe;
	}
	return NULL;
}
