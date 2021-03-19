/*=========================================================================
Program:   AssemblerPro
Module:    AssemblerPro.h
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

#ifndef __AssemblerPro_H__
#define __AssemblerPro_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "appLogic.h" 

//----------------------------------------------------------------------------
// Class Name: AssemblerPro
//----------------------------------------------------------------------------
class AssemblerPro : public wxApp
{
public:

	/**/
	bool OnInit();
	
	/**/
	int  OnExit();

	/** This function filters wxEvents and is used to control global hotKeys */
	int FilterEvent(wxEvent& event);

protected:

	/** Include icon files */
	void InitializeIcons();

	/** Initialize register */
	//void InitializeReg();

	/**Manage Fatal Exception - Try to save project */
	void OnFatalException();

	appLogic *m_Logic;
};
DECLARE_APP(AssemblerPro)
#endif 
