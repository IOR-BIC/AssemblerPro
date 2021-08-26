/*=========================================================================
Program:   AssemblerPro
Module:    albaVMEProsthesis.cpp
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Gianluigi Crimi
==========================================================================
Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/
#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "appDecl.h"
#include "albaVMEProsthesis.h"

#include "mmaMaterial.h"
#include "albaTransform.h"
#include "albaGUI.h"
#include "albaVMEOutput.h"
#include "albaVMESurface.h"

#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"
#include "vtkLookupTable.h"


//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEProsthesis)

//-------------------------------------------------------------------------
albaVMEProsthesis::albaVMEProsthesis()
{
}
//-------------------------------------------------------------------------
albaVMEProsthesis::~albaVMEProsthesis()
{
}
//-------------------------------------------------------------------------
int albaVMEProsthesis::InternalInitialize()
{
	if (Superclass::InternalInitialize() == ALBA_OK)
	{
		// force material allocation
		GetMaterial();

		return ALBA_OK;
	}

	return ALBA_ERROR;
}
//-------------------------------------------------------------------------
mmaMaterial *albaVMEProsthesis::GetMaterial()
{
	mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
	if (material == NULL)
	{
		material = mmaMaterial::New();
		SetAttribute("MaterialAttributes", material);
	}
	return material;
}
//-------------------------------------------------------------------------
bool albaVMEProsthesis::IsAnimated()
{
	return false;
}
//-------------------------------------------------------------------------
void albaVMEProsthesis::GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes)
{
	kframes.clear(); // no timestamps
}
//-------------------------------------------------------------------------
albaGUI* albaVMEProsthesis::CreateGui()
{
	m_Gui = new albaGUI(this);


	m_Gui->Divider();
	m_Gui->FitGui();

	return m_Gui;
}
//-------------------------------------------------------------------------
void albaVMEProsthesis::OnEvent(albaEventBase *alba_event)
{
	// events to be sent up or down in the tree are simply forwarded
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch (e->GetId())
		{

			case ID_START:
			{

			}
			break;
			default:
				albaVME::OnEvent(alba_event);
		}
	}
	else
	{
		Superclass::OnEvent(alba_event);
	}
}
//-------------------------------------------------------------------------
albaVMEOutput *albaVMEProsthesis::GetOutput()
{
	// allocate the right type of output on demand
	if (m_Output == NULL)
	{
		SetOutput(albaVMEOutputSurface::New()); // create the output
	}
	return m_Output;
}
