/*=========================================================================
  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkAPPAbstractPointsSourcePointCloudSource.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

	This software is distributed WITHOUT ANY WARRANTY; without even
	the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
	PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkAPPAbstractPointsSourcePointCloudSource - create a random cloud of points
// .SECTION Description
// vtkAPPAbstractPointsSourcePointCloudSource is a source object that creates a user-specified number 
// of points within a specified radius about a specified center point. 
// By default location of the points is random within the sphere. It is
// also possible to generate random points only on the surface of the
// sphere.

#ifndef __vtkAPPAbstractPointsSourcePointCloudSource_h
#define __vtkAPPAbstractPointsSourcePointCloudSource_h

#include "vtkALBAAbstractPointsSource.h"

class vtkPoints;

class VTK_GRAPHICS_EXPORT vtkAPPPointCloudSource : public vtkALBAAbstractPointsSource
{
public:
	static vtkAPPPointCloudSource *New();
	vtkTypeRevisionMacro(vtkAPPPointCloudSource, vtkALBAAbstractPointsSource);


protected:
	vtkAPPPointCloudSource();
	~vtkAPPPointCloudSource();

private:
	void SetCellArrayToOutput(vtkPolyData *output, vtkCellArray *cellArray);

	vtkAPPPointCloudSource(const vtkAPPPointCloudSource&);  // Not implemented.
	void operator=(const vtkAPPPointCloudSource&);  // Not implemented.
};
#endif
