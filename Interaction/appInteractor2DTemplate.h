/*=========================================================================
Program:   Assembler
Module:    appInteractor2DTemplate.h
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

#ifndef __appInteractor2DTemplate_h
#define __appInteractor2DTemplate_h

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appInteractionDefines.h"

#include "albaEvent.h"
#include "albaInteractorPER.h"
#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkCamera.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class albaDeviceButtonsPadMouse;
class albaRWI;

class vtkLineSource;
class vtkCoordinate;
class vtkPolyDataMapper2D;
class vtkActor2D;
class vtkRenderWindow;
class vtkRenderer;
class vtkXYPlotActor;
class vtkRendererCollection;
class vtkPointSource;
class vtkDiskSource;
class vtkRegularPolygonSource;
class vtkDataSetMapper;
class vtkTransform;
class vtkTransformPolyDataFilter;
class vtkDataSetMapper;
class vtkActor;

// Class Name: appInteractor2DTemplate
class APP_INTERACTION_EXPORT appInteractor2DTemplate : public albaInteractorPER
{
public:

	albaTypeMacro(appInteractor2DTemplate, albaInteractorPER);

	virtual void OnEvent(albaEventBase *event);

	enum EVENT_INTERACTIONS_ID
	{
		ID_MOVING = MINID,
		ID_SCALING,
		ID_DEFORM,
		ID_ROTATING,
	};
	
	enum TransformPoint
	{
		CENTER,
		ROTATION,
	};

	/// Template Methods

	/** Add Template*/
	void AddTemplate(double center[3], double radius);

	/** Reset Template*/
	void ResetTemplate();

	/* Traslate Template*/
	void Traslate(double delta[3]);
	
	/*Set Renderer by View needed*/
	void SetRendererByView(albaView * view);
	
	/** Set Template Opacity*/
	void SetOpacity(double opacity);
	/** Set Actors Opacity*/
	void SetActorsOpacity(double opacity);
	/** Set Color Default*/
	void SetColor(double r, double g, double b);
	/** Set Color Selection*/
	void SetColorSelection(double r, double g, double b);

	/** Set Transform Modality (MODE_TRANSFORM = 0, MODE_DEFORM = 1)*/
	void SetTransformMode(int mode);
	
	/** Enable/Disable Interactor*/
	void Enable(bool enable);
	
	double* GetPoint(int index);

	/* Return current point index */
	int GetCurrentPointIndex() { return m_CurrentPointIndex; };

	double GetOpacity() { return m_Opacity; };
	double GetActorsOpacity() { return m_ActorsOpacity; };
	
	// For Debugging
	double* GetDelta() { return m_Delta; };
	bool GetScaleMode() { return m_AltPressed; };
	
	void ShowActors(bool show);
	int IsVisible() { return m_IsVisible; };

	vtkRenderer *GetRenderer() { return m_Renderer; };

protected:

	appInteractor2DTemplate();
	virtual ~appInteractor2DTemplate();

	// Mouse Events
	virtual void OnLeftButtonDown(albaEventInteraction *e);
	virtual void OnLeftButtonUp(albaEventInteraction *e);
	virtual void OnMove(albaEventInteraction *e);

	/** Remove Template*/
	void RemoveTemplate();

	/** Edit Template*/
	void EditTemplate(int index, double point[3]);

	/** Move Template*/
	void MoveTemplate(int index, double point[3]);
	
	/* Calculate Delta*/
	double* CalculateDelta(int index, double point[3]);

	/* Calculate angle */
	double CalculateAngle(double point1[3], double point2[3], double origin[3]);
	void MovePoint(double(&point)[3], double *delta);

	void InitRenderer(albaEventInteraction *e);

	void ScreenToWorld(double screen[2], double world[3]);

	enum LINES_RANGES
	{
		TRANSFORM_LINE_START = 0,
		TRANSFORM_LINE_END = 4,
		DEFORM_LINE_START = 5,
		DEFORM_LINE_END = 12,
	};

	enum AXIS	{	X,Y,Z, };

	/** Add Point*/
	void AddPoint(double point[3]);
	/** Add Line*/
	void AddLine(double point1[3], double point2[3], bool dashed=false);
	/** Add Circle*/
	void AddCircle(double point[3], double radius);

	void UpdateLineActor(int index, double * point1, double * point2);
	void UpdatePointActor(int index, double * point);
	
	/** Update all Actor Lines*/
	void UpdateLines();

	void UpdateCircle();

	void FindAndHighlightCurrentPoint(double * pointCoord);
	
	bool IsInBound(double *pos);

	double GetClockwiseAngle(double point[3], double origin[3]);

	double Distance(double point1[3], double point2[3]);

	albaDeviceButtonsPadMouse	*m_Mouse;
	vtkRenderer								*m_Renderer;
	albaView									*m_View;
	vtkCoordinate							*m_Coordinate;

	// Points
	std::vector<vtkPointSource *>				m_PointSourceVector;
	std::vector<vtkPolyDataMapper2D *>	m_PointMapperVector;
	std::vector<vtkActor2D *>						m_PointActorVector;

	// Lines
	std::vector<vtkLineSource *>				m_LineSourceVector;
	std::vector<vtkPolyDataMapper2D *>	m_LineMapperVector;
	std::vector<vtkActor2D *>						m_LineActorVector;
	
	vtkDiskSource *m_CircleSource;
	vtkTransform *m_CircleTransform;
	vtkTransformPolyDataFilter *m_CircleTransformFilter;
	vtkDataSetMapper *m_CircleMapper;
	vtkActor *m_CircleActor;

	bool m_IsCreated;

	double m_Opacity;
	double m_ActorsOpacity;
	int m_IsVisible;

	double m_ColorDefault[3];
	double m_ColorSelection[3];

	int m_CurrentPointIndex;
	int m_TotalPoints;
	int m_TotalLines;

	double *m_Bounds;
	bool m_IsInBound;
	bool m_ButtonDownInside;

	bool m_ParallelView;

	double m_ResetPointCen[3];
	double m_ResetPointRot[3];

	double oldPointRot[3];


	int m_AltPressed; // Used for alternative Transform Mode
	int m_ShowActors;

	double m_Delta[3];
	double m_CircleBorder;

private:
	appInteractor2DTemplate(const appInteractor2DTemplate&);   // Not implemented.
	void operator=(const appInteractor2DTemplate&);  // Not implemented.
	friend class appInteractor2DTemplateTest;
};

#endif

