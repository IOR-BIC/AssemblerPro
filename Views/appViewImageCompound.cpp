/*=========================================================================
Program:   AssemblerPro
Module:    appViewImageCompound.cpp
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

#include "appDecl.h"
#include "appViewImageCompound.h"
#include "appViewImage.h"
#include "appUtils.h"

#include "albaGUILutSwatch.h"
#include "albaVMEImage.h"
#include "albaVMEOutput.h"
#include "albaGUI.h"
#include "albaGUILutSlider.h"
#include "albaGUIPicButton.h"
#include "albaVME.h"
#include "albaViewImageCompound.h"

#include "vtkALBASmartPointer.h"
#include "vtkCamera.h"
#include "vtkDataSet.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkWindowToImageFilter.h"

//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
enum SUBVIEW_ID
{
  ID_VIEW_IMAGE = 0,
};

//----------------------------------------------------------------------------
albaCxxTypeMacro(appViewImageCompound);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
appViewImageCompound::appViewImageCompound( wxString label, int num_row, int num_col)
: albaViewImageCompound(label,num_row,num_col)
{
	m_LutWidget = NULL;
	m_LutSlider = NULL;
  m_Ruler = NULL;
  m_Renderer = NULL;

	m_RulerButton = NULL;
  m_ShowRuler = false;

	m_ViewImage = NULL;
	vtkNEW(m_ColorLUT);
}
//----------------------------------------------------------------------------
appViewImageCompound::~appViewImageCompound()
{
  if(m_Ruler)
  {
    m_Renderer->RemoveActor2D(m_Ruler);
    albaDEL(m_Ruler);
  }

	delete m_RulerButton;

	vtkDEL(m_ColorLUT);
}

//-------------------------------------------------------------------------
void appViewImageCompound::PackageView()
{
	m_ViewImage = new appViewImage("View Image", CAMERA_CT, false, false, 0);
	m_ViewImage->PlugVisualPipe("albaVMEVolumeGray", "albaPipeBox", NON_VISIBLE);
	m_ViewImage->PlugVisualPipe("albaVMESurface", "albaPipeSurface", NON_VISIBLE);

	PlugChildView(m_ViewImage);
}

//----------------------------------------------------------------------------
void appViewImageCompound::ShowRuler(bool show)
{
  m_ShowRuler = show;

  if(!m_Ruler)
  {
    SetRendererByView();

// 		m_RulerButton->SetBitmap("HIDE_RULER_ICON");
// 		m_RulerButton->SetToolTip("Show Ruler");

    m_Ruler = vtkALBASimpleRulerActor2D::New();
    m_Ruler->SetColor(0.5,1.0,1.0);
    m_Ruler->SetLabelAxesVisibility(false);
    m_Ruler->SetLegend("mm/tick");
    m_Ruler->CenterAxesOnScreen(false);
		m_Ruler->ShowFixedTick(true);
		
    m_Renderer->AddActor2D((vtkActor2D *)m_Ruler);

    m_Ruler->SetVisibility(show);
    m_Renderer->GetRenderWindow()->Render();
  }
  else
  {
// 		m_RulerButton->SetBitmap("SHOW_RULER_ICON");
// 		m_RulerButton->SetToolTip("Hide Ruler");

    m_Ruler->SetVisibility(show);
    m_Renderer->GetRenderWindow()->Render();
  }
}

//----------------------------------------------------------------------------
void appViewImageCompound::SetRendererByView()
{
  vtkRenderer *newRenderer=NULL;

  vtkRendererCollection *rc;
  rc = this->GetRWI()->GetRenderWindow()->GetRenderers();

  // Searching for a frontal renderer on render collection
  if (rc)
  {
    rc->InitTraversal();
    vtkRenderer *ren; 
    while (ren = rc->GetNextItem())
      if (ren->GetLayer() == 1)//Frontal Render
      {
        newRenderer = ren;
        break;
      }
  }

  m_Renderer = newRenderer;
}

//----------------------------------------------------------------------------
albaView * appViewImageCompound::Copy(albaObserver *Listener, bool lightCopyEnabled /*= false*/)
{
	m_LightCopyEnabled = lightCopyEnabled;
	appViewImageCompound *v = new appViewImageCompound(m_Label, m_ViewRowNum, m_ViewColNum);
	v->m_Listener = Listener;
	v->m_Id = m_Id;
	for (int i = 0; i < m_PluggedChildViewList.size(); i++)
	{
		v->m_PluggedChildViewList.push_back(m_PluggedChildViewList[i]->Copy(this));
	}
	v->m_NumOfPluggedChildren = m_NumOfPluggedChildren;
	v->Create();

	return v;
}

//----------------------------------------------------------------------------
void appViewImageCompound::CreateGuiView()
{
	m_GuiView = new albaGUI(this);

	wxBoxSizer *mainVertSizer = new wxBoxSizer(wxHORIZONTAL);
	m_LutSlider = new albaGUILutSlider(m_GuiView, -1, wxPoint(0, 0), wxSize(500, 24));
	m_LutSlider->SetListener(this);
 	m_LutSlider->SetSize(500, 24);
 	m_LutSlider->SetMinSize(wxSize(500, 24));
 	EnableWidgets(false);

	m_RulerButton = new albaGUIPicButton(m_GuiView, "SHOW_RULER_ICON", VIEW_RULER, this);
	m_RulerButton->SetToolTip("Show Ruler");
	m_RulerButton->SetListener(this);

	mainVertSizer->Add(m_LutSlider,wxEXPAND);
	mainVertSizer->Add(m_RulerButton);

	m_GuiView->Add(mainVertSizer,1 , wxEXPAND);
	
	m_GuiView->Reparent(m_Win);
}

//----------------------------------------------------------------------------
void appViewImageCompound::OnEvent(albaEventBase *alba_event)
{
	switch (alba_event->GetId())
	{
	case VIEW_RULER:
	{
		ShowRuler(!m_ShowRuler);

		if (!m_ShowRuler)
		{
			//m_RulerButton->SetBitmap("SHOW_RULER_ICON");
			m_RulerButton->SetToolTip("Show Ruler");
			//m_RulerButton->SetListener(this);
		}
		else
		{
			//m_RulerButton->SetBitmap("HIDE_RULER_ICON");
			m_RulerButton->SetToolTip("Hide Ruler");
			//m_RulerButton->SetListener(this);
		}
	}
	break;
	
	default:
		Superclass::OnEvent(alba_event);
	}
}

//----------------------------------------------------------------------------
albaPipeImage3D* appViewImageCompound::GetPipeImage(albaVME* vme)
{
	if (vme->IsA("albaVMEImage"))
	{
		albaPipeImage3D *pipe = (albaPipeImage3D *)m_ChildViewList[ID_VIEW_IMAGE]->GetNodePipe(vme);
		return pipe;
	}
	return NULL;
}

//----------------------------------------------------------------------------
void appViewImageCompound::EnableMutex(bool enable)
{
	((appViewImage*)m_ChildViewList[ID_VIEW_IMAGE])->EnableMutex(enable);
}

//----------------------------------------------------------------------------
void appViewImageCompound::VmeShow(albaVME *vme, bool show)
{
	Superclass::VmeShow(vme, show);
}
//----------------------------------------------------------------------------
void appViewImageCompound::VmeRemove(albaVME *vme)
{
	Superclass::VmeRemove(vme);
}