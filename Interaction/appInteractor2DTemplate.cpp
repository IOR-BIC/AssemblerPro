 /*=========================================================================
Program:   Assembler
Module:    appInteractor2DTemplate.cpp
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

#include "appInteractor2DTemplate.h"
#include "appDecl.h"
#include "appUtils.h"

#include "albaDecl.h"
#include "albaDefines.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaDeviceButtonsPadTracker.h"
#include "albaEventInteraction.h"
#include "albaRWI.h"
#include "albaVME.h"
#include "albaVMEOutput.h"
#include "albaView.h"

#include "vtkALBASmartPointer.h"
#include "vtkALBATextActorMeter.h"
#include "vtkActor.h"
#include "vtkActor2D.h"
#include "vtkCamera.h"
#include "vtkCellArray.h"
#include "vtkDataSet.h"
#include "vtkDataSetAttributes.h"
#include "vtkFloatArray.h"
#include "vtkLine.h"
#include "vtkLineSource.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPNGReader.h"
#include "vtkPointData.h"
#include "vtkPointSource.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkPolygon.h"
#include "vtkProperty.h"
#include "vtkProperty2D.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkTexture.h"
#include "vtkDiskSource.h"
//#include "vtkRegularPolygonSource.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkDataSetMapper.h"
#include "vtkActor.h"

#include <algorithm>
#include "albaVect3d.h"

#define POINT_UPDATE_DISTANCE 2.5
#define POINT_UPDATE_DISTANCE_2 (POINT_UPDATE_DISTANCE * POINT_UPDATE_DISTANCE)

//------------------------------------------------------------------------------
albaCxxTypeMacro(appInteractor2DTemplate)

//----------------------------------------------------------------------------
appInteractor2DTemplate::appInteractor2DTemplate()
{
	m_Coordinate = vtkCoordinate::New();
	m_Coordinate->SetCoordinateSystemToWorld();

	m_Mouse = NULL;
	m_Renderer = NULL;
	m_View = NULL;

	m_IsCreated = false;

	m_Bounds = NULL;
	m_IsInBound = false;
	m_ButtonDownInside = false;

	m_CurrentPointIndex = -1;
	m_TotalPoints = 0;
	m_TotalLines = 0;

	m_DraggingLeft = false;
	m_ParallelView = false;

	m_IsVisible = 1;
	m_Opacity = 1.0;
	m_ActorsOpacity = 1.0;
	m_ColorDefault[0] = 1;	m_ColorDefault[1] = 1;	m_ColorDefault[2] = 0;
	m_ColorSelection[0] = 0;	m_ColorSelection[1] = 1;	m_ColorSelection[2] = 0;

	m_Delta[X] = m_Delta[Y] = m_Delta[Z] = 0.0;
	m_CircleBorder = 0.5;

	// Text
	m_ShowActors = true;
}
//----------------------------------------------------------------------------
appInteractor2DTemplate::~appInteractor2DTemplate()
{
	vtkDEL(m_Coordinate);
	
	RemoveTemplate();	
}

/// EVENTS
//----------------------------------------------------------------------------
void appInteractor2DTemplate::OnLeftButtonDown(albaEventInteraction *e)
{
	albaEventMacro(albaEvent(this, CAMERA_UPDATE));

	if (!m_ParallelView)
	{
		InitRenderer(e);
	}

	int shiftPressed = e->GetModifier(ALBA_SHIFT_KEY) ? 1 : 0;
	m_AltPressed = e->GetModifier(ALBA_ALT_KEY) ? 1 : 0;

	if (shiftPressed)
	{
		Superclass::OnLeftButtonDown(e);
	}
	else if (m_ParallelView)// && m_IsInBound)
	{
		OnButtonDown(e);
		m_DraggingLeft = true;

		if (m_CurrentPointIndex >= 0)
		{
			double pos_2d[2];
			e->Get2DPosition(pos_2d);
			double pointCoord[3];
			ScreenToWorld(pos_2d, pointCoord);
			
			EditTemplate(m_CurrentPointIndex, pointCoord);
		}
	}

	m_ButtonDownInside = m_IsInBound;
}
//----------------------------------------------------------------------------
void appInteractor2DTemplate::OnLeftButtonUp(albaEventInteraction *e)
{
	m_DraggingLeft = false;
	OnButtonUp(e);

	if (m_ParallelView)// && m_IsInBound && m_ButtonDownInside)
	{
		double pos_2d[2];
		double pointCoord[4];
		e->Get2DPosition(pos_2d);
		ScreenToWorld(pos_2d, pointCoord);
		FindAndHighlightCurrentPoint(pointCoord);
	}
}
//----------------------------------------------------------------------------
void appInteractor2DTemplate::OnMove(albaEventInteraction *e)
{
	if (!m_ParallelView)
	{
		InitRenderer(e);
	}

	if (m_ParallelView)
	{
		double pos_2d[2];
		e->Get2DPosition(pos_2d);

		double pointCoord[3];
		ScreenToWorld(pos_2d, pointCoord);

		if (m_IsInBound != IsInBound(pointCoord))
		{
			if (m_Renderer)
				m_Renderer->GetRenderWindow()->Render();
		}

		//if (m_IsInBound)
		{
			if (!m_DraggingLeft)
			{
				FindAndHighlightCurrentPoint(pointCoord);
			}
			else
			{
				EditTemplate(m_CurrentPointIndex, pointCoord);
			}
		}
	}
}
//----------------------------------------------------------------------------
void appInteractor2DTemplate::OnEvent(albaEventBase *event)
{
	albaID ch = event->GetChannel();

	if (ch == MCH_INPUT)
	{
		albaID id = event->GetId();

		if (id == albaDeviceButtonsPadTracker::GetTracker3DMoveId() || id == albaDeviceButtonsPadMouse::GetMouse2DMoveId())
		{
			albaEventInteraction *e = albaEventInteraction::SafeDownCast(event);
			OnMove(e);
		}
	}

	Superclass::OnEvent(event);
}

/// TEMPLATE
//----------------------------------------------------------------------------
void appInteractor2DTemplate::AddTemplate(double center[3], double radius)
{
	if (m_IsCreated)
		RemoveTemplate();

	// Central Point
	m_ResetPointCen[X] = center[X];
	m_ResetPointCen[Y] = center[Y];
	m_ResetPointCen[Z] = 0.0;
	
	// Rotation Point
	m_ResetPointRot[X] = center[X] + radius;
	m_ResetPointRot[Y] = center[Y];
	m_ResetPointRot[Z] = 0.0;
				
	//////////////////////////////////////////////////////////////////////////

	// Add Actors
	AddPoint(m_ResetPointCen);
	AddPoint(m_ResetPointRot);

	AddLine(m_ResetPointCen, m_ResetPointRot, true); // Extra Line (Rotation)
	AddCircle(m_ResetPointCen, radius);
	
	m_IsCreated = true;
	
	//////////////////////////////////////////////////////////////////////////

	albaEventMacro(albaEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void appInteractor2DTemplate::RemoveTemplate()
{
	for (int i = 0; i < m_LineActorVector.size(); i++)
	{
		// Lines
		m_Renderer->RemoveActor2D(m_LineActorVector[i]);
		vtkDEL(m_LineActorVector[i]);
		vtkDEL(m_LineMapperVector[i]);
		vtkDEL(m_LineSourceVector[i]);
	}

	for (int i = 0; i < m_PointActorVector.size(); i++)
	{
		// Points
		m_Renderer->RemoveActor2D(m_PointActorVector[i]);
		vtkDEL(m_PointActorVector[i]);
		vtkDEL(m_PointMapperVector[i]);
		vtkDEL(m_PointSourceVector[i]);
	}
	
	m_PointSourceVector.clear();
	m_PointMapperVector.clear();
	m_PointActorVector.clear();

	m_LineSourceVector.clear();
	m_LineMapperVector.clear();
	m_LineActorVector.clear();


	m_Renderer->RemoveActor(m_CircleActor);
	vtkDEL(m_CircleActor);
	vtkDEL(m_CircleMapper);
	vtkDEL(m_CircleTransformFilter);
	vtkDEL(m_CircleTransform);
	vtkDEL(m_CircleSource);

	m_IsCreated = false;

	m_Renderer->GetRenderWindow()->Render();
}

//----------------------------------------------------------------------------
void appInteractor2DTemplate::AddPoint(double point[3])
{
	point[Z] = 0;

	// Add Points
	m_PointSourceVector.push_back(vtkPointSource::New());
	m_PointSourceVector[m_PointSourceVector.size() - 1]->SetNumberOfPoints(1);
	m_PointMapperVector.push_back(vtkPolyDataMapper2D::New());
	m_PointActorVector.push_back(vtkActor2D::New());
	m_PointActorVector[m_PointActorVector.size()-1]->GetProperty()->SetColor(m_ColorDefault[0], m_ColorDefault[1], m_ColorDefault[2]);

	m_Renderer->AddActor2D(m_PointActorVector[m_PointActorVector.size() - 1]);
	
	UpdatePointActor(m_PointActorVector.size() - 1, point);

	m_TotalPoints++;
}
//----------------------------------------------------------------------------
void appInteractor2DTemplate::AddLine(double point1[3], double point2[3], bool dashed)
{
	point1[Z] = 0;
	point2[Z] = 0;

	// Add Line
	m_LineSourceVector.push_back(vtkLineSource::New());
	m_LineMapperVector.push_back(vtkPolyDataMapper2D::New());
	m_LineActorVector.push_back(vtkActor2D::New());

	if(dashed)
		m_LineActorVector[m_LineActorVector.size() - 1]->GetProperty()->SetLineStipplePattern(0xf0f0);

	m_Renderer->AddActor2D(m_LineActorVector[m_LineActorVector.size() - 1]);

	UpdateLineActor(m_LineSourceVector.size() - 1, point1, point2);

	m_TotalLines++;
}
//----------------------------------------------------------------------------
void appInteractor2DTemplate::AddCircle(double point[3], double radius)
{
	m_CircleSource = vtkDiskSource::New();
	m_CircleSource->SetCircumferentialResolution(50);
	m_CircleSource->SetInnerRadius(radius - m_CircleBorder);
	m_CircleSource->SetOuterRadius(radius);
	
	m_CircleTransform = vtkTransform::New();
	m_CircleTransform->Identity();
	m_CircleTransform->Translate(point);

	m_CircleTransformFilter = vtkTransformPolyDataFilter::New();
	m_CircleTransformFilter->SetTransform(m_CircleTransform);
	m_CircleTransformFilter->SetInput(m_CircleSource->GetOutput());
	m_CircleTransformFilter->Update();

	m_CircleMapper = vtkDataSetMapper::New();
	m_CircleMapper->SetInput(m_CircleTransformFilter->GetOutput());

	m_CircleActor = vtkActor::New();
	m_CircleActor->SetMapper(m_CircleMapper);

	m_Renderer->AddActor(m_CircleActor);
}

/// RENDERING
//----------------------------------------------------------------------------
void appInteractor2DTemplate::UpdateLineActor(int index, double * point1, double * point2)
{
	if (index > -1 && index < m_LineSourceVector.size())
	{
		point1[Z] = point2[Z] = 0;

		m_LineSourceVector[index]->SetPoint1(point1);
		m_LineSourceVector[index]->SetPoint2(point2);
		m_LineSourceVector[index]->Update();

		m_LineMapperVector[index]->SetTransformCoordinate(m_Coordinate);
		m_LineMapperVector[index]->SetInput(m_LineSourceVector[index]->GetOutput());

		m_LineActorVector[index]->SetMapper(m_LineMapperVector[index]);
		m_LineActorVector[index]->GetProperty()->SetColor(m_ColorDefault[0], m_ColorDefault[1], m_ColorDefault[2]);
	}
}
//----------------------------------------------------------------------------
void appInteractor2DTemplate::UpdatePointActor(int index, double * point)
{
	point[Z] = 0;

	if (index > -1)
	{
		m_PointSourceVector[index]->SetCenter(point);
		m_PointSourceVector[index]->Update();

		m_PointMapperVector[index]->SetTransformCoordinate(m_Coordinate);
		m_PointMapperVector[index]->SetInput(m_PointSourceVector[index]->GetOutput());

		m_PointActorVector[index]->SetMapper(m_PointMapperVector[index]);
		m_PointActorVector[index]->GetProperty()->SetPointSize(8.0);

		m_PointActorVector[index]->GetProperty()->SetOpacity(m_ActorsOpacity);
	}
}
//----------------------------------------------------------------------------
void appInteractor2DTemplate::UpdateLines()
{
	double pointCen[3];	double pointRot[3];

	m_PointSourceVector[CENTER]->GetCenter(pointCen);
	m_PointSourceVector[ROTATION]->GetCenter(pointRot);

	//////////////////////////////////////////////////////////////////////////
	
	UpdateLineActor(0, pointCen, pointRot); // Extra Line (Rotation)
}
//----------------------------------------------------------------------------
void appInteractor2DTemplate::UpdateCircle()
{
	double pointCen[3];	double pointRot[3];

	m_PointSourceVector[CENTER]->GetCenter(pointCen);
	m_PointSourceVector[ROTATION]->GetCenter(pointRot);

	pointCen[0] += m_Delta[0];
	pointCen[1] += m_Delta[1];

	m_CircleTransform->Identity();
	m_CircleTransform->Translate(pointCen);
	m_CircleTransformFilter->Update();
}

/// TRANSFORM - INTERNAL
//----------------------------------------------------------------------------
void appInteractor2DTemplate::EditTemplate(int index, double point[3])
{
	if (index < 0)
		return;

	point[Z] = 0;

	if (index == CENTER) //Move is enabled on both modality
		MoveTemplate(index, point);
	else
	{
		UpdatePointActor(index, point);

		double pointCen[3];	double pointRot[3];

		m_PointSourceVector[CENTER]->GetCenter(pointCen);
		m_PointSourceVector[ROTATION]->GetCenter(pointRot);

		double radius = Distance(pointCen, pointRot);

		m_CircleSource->SetInnerRadius(radius - 0.5);
		m_CircleSource->SetOuterRadius(radius);
		m_CircleSource->Update();	
	}
	
	UpdateLines(); // Lines
	UpdateCircle();

	//////////////////////////////////////////////////////////////////////////
	if (m_Renderer)
		m_Renderer->GetRenderWindow()->Render();

	albaEventMacro(albaEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void appInteractor2DTemplate::MoveTemplate(int index, double point[3])
{
	double pointRot[3];
	m_PointSourceVector[ROTATION]->GetCenter(pointRot);

	//////////////////////////////////////////////////////////////////////////

	CalculateDelta(index, point);

	MovePoint(pointRot, m_Delta);

	//////////////////////////////////////////////////////////////////////////

	// Points
	UpdatePointActor(-1, point); // Edit Point Actor
	UpdatePointActor(CENTER, point);
	UpdatePointActor(ROTATION, pointRot);

	UpdateLines(); // Lines
	UpdateCircle();

	albaEventMacro(albaEvent(this, ID_MOVING));
}

//----------------------------------------------------------------------------
void appInteractor2DTemplate::ResetTemplate()
{
	m_Delta[X] = m_Delta[Y] = m_Delta[Z] = 0;
	m_CurrentPointIndex = 0;

	//////////////////////////////////////////////////////////////////////////

	// Points
	UpdatePointActor(CENTER, m_ResetPointCen);
	UpdatePointActor(ROTATION, m_ResetPointRot);

	UpdateLines(); // Lines
	UpdateCircle();

	//////////////////////////////////////////////////////////////////////////
	if (m_Renderer)
		m_Renderer->GetRenderWindow()->Render();

	albaEventMacro(albaEvent(this, CAMERA_UPDATE));
}

/// TRANSFORM - PUBLIC
//----------------------------------------------------------------------------
void appInteractor2DTemplate::Traslate(double delta[3])
{
	double pointCen[3];	double pointRot[3];
	
	m_PointSourceVector[CENTER]->GetCenter(pointCen);
	m_PointSourceVector[ROTATION]->GetCenter(pointRot);

	//////////////////////////////////////////////////////////////////////////

	MovePoint(pointCen, delta);
	MovePoint(pointRot, delta);

	//////////////////////////////////////////////////////////////////////////
	
	// Points
	UpdatePointActor(CENTER, pointCen);
	UpdatePointActor(ROTATION, pointRot);
	
	UpdateLines(); // Lines
	UpdateCircle();
}

/// SET
//----------------------------------------------------------------------------
void appInteractor2DTemplate::InitRenderer(albaEventInteraction *e)
{
	albaEventMacro(albaEvent(this, CAMERA_UPDATE));

	if (m_Renderer == NULL)
	{
		if (m_Mouse == NULL)
		{
			albaDevice *device = albaDevice::SafeDownCast((albaDevice*)e->GetSender());
			albaDeviceButtonsPadMouse  *mouse = albaDeviceButtonsPadMouse::SafeDownCast(device);
			m_Mouse = mouse;
		}

		m_Renderer = m_Mouse->GetRenderer();
	}

	if (m_Renderer)
		m_ParallelView = m_Renderer->GetActiveCamera()->GetParallelProjection() != 0;
}
//----------------------------------------------------------------------------
void appInteractor2DTemplate::SetRendererByView(albaView * view)
{
	m_View = view;

	vtkRenderer *newRenderer = NULL;

	vtkRendererCollection *rc;
	rc = view->GetRWI()->GetRenderWindow()->GetRenderers();

	// Searching for a frontal renderer on render collection
	if (rc)
	{
		rc->InitTraversal();
		vtkRenderer *ren;
		while (ren = rc->GetNextItem())
			if (ren->GetLayer() == 1) // Frontal Render
			{
				newRenderer = ren;
				break;
			}
	}

	m_Renderer = newRenderer;
	m_Renderer->GetRenderWindow()->Render();
}

//----------------------------------------------------------------------------
void appInteractor2DTemplate::Enable(bool enable)
{
	for (int p = 0; p < m_PointActorVector.size(); p++)
		m_PointActorVector[p]->SetVisibility(enable);

	for (int l = 0; l < m_LineActorVector.size(); l++)
		m_LineActorVector[l]->SetVisibility(enable);

	//////////////////////////////////////////////////////////////////////////
	if (m_Renderer)
		m_Renderer->GetRenderWindow()->Render();

	albaEventMacro(albaEvent(this, CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
void appInteractor2DTemplate::SetOpacity(double opacity)
{
	if (opacity > 1.0) opacity = 1.0;
	m_Opacity = opacity;
	
	//////////////////////////////////////////////////////////////////////////
	if (m_Renderer)
		m_Renderer->GetRenderWindow()->Render();

	albaEventMacro(albaEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void appInteractor2DTemplate::SetActorsOpacity(double opacity)
{
	if (opacity > 1.0) opacity = 1.0;
	m_ActorsOpacity = opacity;
	
	//////////////////////////////////////////////////////////////////////////
	if (m_Renderer)
		m_Renderer->GetRenderWindow()->Render();

	albaEventMacro(albaEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void appInteractor2DTemplate::SetColor(double r, double g, double b)
{
	m_ColorDefault[0] = r;
	m_ColorDefault[1] = g;
	m_ColorDefault[2] = b;
}
//----------------------------------------------------------------------------
void appInteractor2DTemplate::SetColorSelection(double r, double g, double b)
{
	m_ColorSelection[0] = r;
	m_ColorSelection[1] = g;
	m_ColorSelection[2] = b;
}

//----------------------------------------------------------------------------
double* appInteractor2DTemplate::GetPoint(int index)
{
	double point[3];
	m_PointSourceVector[index]->GetCenter(point);
	point[Z] = 0.0;

	return point;
}

//----------------------------------------------------------------------------
void appInteractor2DTemplate::FindAndHighlightCurrentPoint(double * pointCoord)
{
	bool found = false;

	for (int i = CENTER; i <= ROTATION; i++)
	{
		double point[3];
		m_PointSourceVector[i]->GetCenter(point);

		if (vtkMath::Distance2BetweenPoints(point, pointCoord) < POINT_UPDATE_DISTANCE_2)
		{
			if (m_CurrentPointIndex != i)
			{
				int cursor;
				m_CurrentPointIndex = i;
				m_PointActorVector[i]->GetProperty()->SetColor(m_ColorSelection);

				switch (m_CurrentPointIndex)
				{
				case CENTER:
					cursor = wxCURSOR_SIZING;
					break;
				case ROTATION:
					cursor = wxCURSOR_BULLSEYE;
				default:
					break;
				}

				// Set Mouse Cursor
				if (m_View->GetWindow())
					m_View->GetWindow()->SetCursor(wxCursor(cursor));

				m_Renderer->GetRenderWindow()->Render();
			}
			return;
		}
	}

	//point highlighted need to be normalized
	if (m_CurrentPointIndex >= 0)
	{
		m_PointActorVector[m_CurrentPointIndex]->GetProperty()->SetColor(m_ColorDefault);

		m_CurrentPointIndex = -1;

		// Set Mouse Cursor
		if (m_View->GetWindow())
			m_View->GetWindow()->SetCursor(wxCursor(wxCURSOR_ARROW));

		m_Renderer->GetRenderWindow()->Render();
	}
}

/// UTILITIES
//----------------------------------------------------------------------------
bool appInteractor2DTemplate::IsInBound(double *pos)
{
	double bounds[6];
	m_Renderer->ComputeVisiblePropBounds(bounds);
	m_Bounds = bounds;

	m_IsInBound = false;

	if (m_Bounds)
	{
		if (pos[X] > m_Bounds[0] && pos[X] < m_Bounds[1]) // MarginLeft & MarginRight
			if (pos[Y] > m_Bounds[2] && pos[Y] < m_Bounds[3]) // MarginUp & MarginDown
				m_IsInBound = true;
	}

	return m_IsInBound;
}
//----------------------------------------------------------------------------
void appInteractor2DTemplate::ScreenToWorld(double screen[2], double world[3])
{
	double wp[4];

	m_Renderer->SetDisplayPoint(screen[X], screen[Y], 0);
	m_Renderer->DisplayToWorld();
	m_Renderer->GetWorldPoint(wp);

	world[X] = wp[X];
	world[Y] = wp[Y];
	world[Z] = 0;
}


//----------------------------------------------------------------------------
double* appInteractor2DTemplate::CalculateDelta(int index, double point[3])
{
	double oldPoint[3];
	m_PointSourceVector[index]->GetCenter(oldPoint);

	m_Delta[X] = point[X] - oldPoint[X];
	m_Delta[Y] = point[Y] - oldPoint[Y];
	m_Delta[Z] = 0;

	return m_Delta;
}
//----------------------------------------------------------------------------
double appInteractor2DTemplate::CalculateAngle(double point1[3], double point2[3], double origin[3])
{
	double angleToP1 = atan2((point1[X] - origin[X]), (point1[Y] - origin[Y]));
	double angleToP2 = atan2((point2[X] - origin[X]), (point2[Y] - origin[Y]));

	double angle = angleToP2 - angleToP1;

	if (angle < 0) angle += (2 * vtkMath::Pi());
		
	return angle;
}

//----------------------------------------------------------------------------
void appInteractor2DTemplate::MovePoint(double(&point)[3], double *delta)
{
	point[X] += delta[X];
	point[Y] += delta[Y];
	point[Z] = 0;
}

//----------------------------------------------------------------------------
void appInteractor2DTemplate::ShowActors(bool show)
{
	m_ShowActors = show;

	for (int i = 0; i < m_PointActorVector.size(); i++)
		m_PointActorVector[i]->SetVisibility(show);

	for (int i = 0; i < m_LineActorVector.size(); i++)
		m_LineActorVector[i]->SetVisibility(show);

	//////////////////////////////////////////////////////////////////////////
	if (m_Renderer)
		m_Renderer->GetRenderWindow()->Render();

	albaEventMacro(albaEvent(this, CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
double appInteractor2DTemplate::GetClockwiseAngle(double point[3], double origin[3])
{
	double angle = 0.0;
	angle = atan2(point[Y] - origin[Y], point[X] - origin[X]) * 180 / M_PI;;

	return angle;
}
//----------------------------------------------------------------------------
double  appInteractor2DTemplate::Distance(double point1[3], double point2[3])
{
	return sqrt(pow(point1[X] - point2[X], 2) + pow(point1[Y] - point2[Y], 2));
}
