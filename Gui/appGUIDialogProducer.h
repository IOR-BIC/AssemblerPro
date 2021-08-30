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
#include "albaProsthesesDBManager.h"

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

	void SetProducer(albaProDBProducer &producer);
	albaProDBProducer* GetProducer() { return m_CurrentProducer; };

	void Show();

	bool OkClosed() { return m_IsChanged; };

protected:

	void SelectImage();

	void CreateProducerDialog();
	void UpdateProducerDialog();

	albaProDBProducer *m_CurrentProducer;

	wxString m_ProducerName;
	wxString m_ProducerWebSite;
	wxString m_ProducerImageName;
	wxString m_ProducerImageFullName;
	bool m_IsChanged;

	albaGUI *m_Gui; ///< Gui variable used to plug custom widgets
	
	wxBoxSizer *m_MainBoxSizer;
	wxBoxSizer *m_ImageSizer;
	albaGUIPicButton *m_ProducerImageButton;

	wxTextCtrl *m_ProducerName_textCtrl;
	wxTextCtrl *m_ProducerSite_textCtrl;
};
#endif
