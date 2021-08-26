/*=========================================================================
Program:   AssemblerPro
Module:    appGUIDialogProducer.h
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

#ifndef __appGUIDialogProducer_H__
#define __appGUIDialogProducer_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaGUIDialog.h"
#include "appDecl.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class albaGUIButton;
class albaGUILab;
class albaGUIPicButton;
class appGUIHyperLink;

class ALBA_EXPORT appGUIDialogProducer : public albaGUIDialog
{
public:

	appGUIDialogProducer(const wxString& title, long style = albaCLOSEWINDOW);
	virtual ~appGUIDialogProducer();

	void OnEvent(albaEventBase *alba_event);

	void SelectImage();

	void SetProducer(AuxProducer &producer) { m_CurrentProducer = producer; };
	AuxProducer GetProducer() { return m_CurrentProducer; };

	void Show();

protected:

	void CreateProducerDialog();
	void UpdateProducerDialog();

	AuxProducer m_CurrentProducer;

	albaGUI *m_Gui; ///< Gui variable used to plug custom widgets
	
	wxBoxSizer *m_MainBoxSizer;
	wxBoxSizer *m_ImageSizer;
	albaGUIPicButton *m_ProducerImageButton;

	wxTextCtrl *m_ProducerName_textCtrl;
	wxTextCtrl *m_ProducerSite_textCtrl;
};
#endif
