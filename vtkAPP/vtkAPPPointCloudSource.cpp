/*=========================================================================
  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkAPPAbstractPointsSourcePointCloudSource.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

	This software is distributed WITHOUT ANY WARRANTY; without even
	the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
	PURPOSE.  See the above copyright notice for more information.
=========================================================================*/

#include "vtkAPPPointCloudSource.h"

#include "vtkCellArray.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"

#include <float.h>
#include <math.h>

vtkCxxRevisionMacro(vtkAPPPointCloudSource, "$Revision: 1.43 $");
vtkStandardNewMacro(vtkAPPPointCloudSource);

//----------------------------------------------------------------------------
vtkAPPPointCloudSource::vtkAPPPointCloudSource():vtkALBAAbstractPointsSource()
{
}

//----------------------------------------------------------------------------
vtkAPPPointCloudSource::~vtkAPPPointCloudSource()
{
}

void vtkAPPPointCloudSource::SetCellArrayToOutput(vtkPolyData *output, vtkCellArray *cellArray)
{
	output->SetVerts(cellArray);
}