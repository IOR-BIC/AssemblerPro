/*=========================================================================
Program:   Albedo
Module:    appFakeLogicForTest.h
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

#ifndef _appFakeLogicForTest_H
#define _appFakeLogicForTest_H

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaAbsLogicManager.h"
#include <vector>

//----------------------------------------------------------------------------
// Class Name: appFakeLogicForTest
//----------------------------------------------------------------------------
class appFakeLogicForTest : public albaAbsLogicManager
{
public:

	enum testFunctions
	{
		FKL_VME_SELECT,
		FKL_VME_ADD,
		FKL_VME_REMOVE,
		FKL_VME_MODIFIED,
		FKL_VME_SHOW,
		FKL_VME_VISUAL_MODE_CHANGED,
		FKL_VME_REMOVED,
		FKL_PRINT_IMAGE
	};

	struct calls
	{
		testFunctions testFunction;
		albaVME *vme;
	};

	appFakeLogicForTest();
	~appFakeLogicForTest();

	// VME
	virtual void VmeSelect(albaVME *vme);

	virtual void VmeSelected(albaVME *vme);

	virtual void VmeShow(albaVME *vme, bool visibility);

	virtual void VmeAdd(albaVME *vme);

	virtual void VmeRemove(albaVME *vme);

	virtual void VmeModified(albaVME* vme);

	virtual void VmeVisualModeChanged(albaVME * vme);

	// Camera
	virtual void CameraUpdate();

	virtual void CameraReset();

	void ClearCalls();

	calls GetCall(int callNum);

	void AddCall(testFunctions func, albaVME* vme);

	virtual bool IsOperationRunning();
	
	virtual const char* GetMsfFileExtension() override;
	
	virtual void VmeRemoved() override;
	
	virtual void PrintImage(albaVMEImage *img) override;

	virtual albaOpManager * GetOpManager() const override;

protected:

	std::vector<calls> m_Calls;
};
#endif