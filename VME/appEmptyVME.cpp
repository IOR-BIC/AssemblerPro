/*=========================================================================
Program:   AssemblerPro
Module:    appEmptyVME.cpp
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2021 (http://www.ltmsoftware.org/alba.htm)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "appDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "appDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "appEmptyVME.h"

#include "albaGUI.h"
#include "albaStorageElement.h"
#include "albaVME.h"
#include "albaVMEOutputNULL.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(appEmptyVME);

//------------------------------------------------------------------------------
appEmptyVME::appEmptyVME()
{
	m_Gui = NULL;

	m_String = "String";
	m_Integer = 12;
	m_Double = 3.14159265359;
	m_Bool = true;
}
//-------------------------------------------------------------------------
appEmptyVME::~appEmptyVME()
{
	//SetOutput(NULL);
}

// GET-SET //////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------
albaVMEOutput *appEmptyVME::GetOutput()
{
	// Allocate the right type of output on demand
	if (m_Output == NULL)
	{
		SetOutput(albaVMEOutputNULL::New()); // Create the output
	}

	return m_Output;
}

//-------------------------------------------------------------------------
char** appEmptyVME::GetIcon()
{
#include "pic/VME_PROCEDURAL_ICON.xpm"
	return VME_PROCEDURAL_ICON_xpm;
}

//----------------------------------------------------------------------------
void appEmptyVME::SetMatrix(const albaMatrix &mat)
{
}

//----------------------------------------------------------------------------
void appEmptyVME::GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes)
{
}

//-------------------------------------------------------------------------
albaGUI* appEmptyVME::CreateGui()
{
	m_Gui = albaVME::CreateGui();

	if(m_Gui)
	{
		m_Gui->SetListener(this);

		m_Gui->Divider();
		m_Gui->Label("Empty VME Gui:", true);
		m_Gui->Divider();

		// Show VME Properties
		m_Gui->String(NULL, "String", &m_String);
		m_Gui->Integer(NULL, "Integer", &m_Integer);
		m_Gui->Double(NULL, "Double", &m_Double);
		m_Gui->Bool(NULL, "Bool", ((int*)&m_Bool));

		m_Gui->Divider(1);
		m_Gui->Button(ID_RESET_BUTTON, "Reset");
		m_Gui->Divider();

		m_Gui->Show();
	}

	return m_Gui;
}

//----------------------------------------------------------------------------
void appEmptyVME::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch (e->GetId())
		{
		case ID_RESET_BUTTON:
		{
			// Reset Values
			m_String = "String";
			m_Integer = 12;
			m_Double = 3.14159265359;
			m_Bool = true;

			if (m_Gui) 
				m_Gui->Update();
		}
			break;

		default:
			albaVME::OnEvent(alba_event);
		}
	}
	else
		albaVME::OnEvent(alba_event);
}

//-----------------------------------------------------------------------
void appEmptyVME::InternalPreUpdate()
{
	InternalInitialize();
}

//-----------------------------------------------------------------------
void appEmptyVME::InternalUpdate()
{
}

//-----------------------------------------------------------------------
int appEmptyVME::InternalStore(albaStorageElement *node)
{
	if (Superclass::InternalStore(node) == ALBA_OK)
	{
		if (node->StoreText("String", m_String) == ALBA_OK &&
				node->StoreInteger("Integer", m_Integer) == ALBA_OK &&
				node->StoreDouble("Double", m_Double) == ALBA_OK
			)
			return ALBA_OK;
	}
	return ALBA_ERROR;
}

//-----------------------------------------------------------------------
int appEmptyVME::InternalRestore(albaStorageElement *node)
{
	if (Superclass::InternalRestore(node) == ALBA_OK)
	{
		node->RestoreText("String", m_String);
		node->RestoreInteger("Integer", m_Integer);
		node->RestoreDouble("Double", m_Double);

		return ALBA_OK;
	}
	return ALBA_ERROR;
}