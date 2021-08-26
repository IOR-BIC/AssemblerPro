/*=========================================================================
Program:   AssemblerPro
Module:    appGUIDialogProsthesis.h
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

#ifndef __appGUIDialogProsthesis_H__
#define __appGUIDialogProsthesis_H__

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

class ALBA_EXPORT appGUIDialogProsthesis : public albaGUIDialog
{
public:
	
	appGUIDialogProsthesis(const wxString& title, long style = albaCLOSEWINDOW);
	virtual ~appGUIDialogProsthesis();
	
	void OnEvent(albaEventBase *alba_event);

	void SetProsthesis(AuxProsthesis &model);;
	AuxProsthesis GetProsthesis() { return m_CurrentProsthesis; };

	void Show();

protected:

	void Init();

	void CreateDialog();
	void UpdateDialog();

	void EditProducer();
	void AddProducer();
	void RemoveProducer();

	void UpdateProducer(AuxProducer producer);

	void EditType();
	void AddType();
	void RemoveType();

	void SelectImage();

	AuxProducer   m_CurrentProducer;
	AuxProsthesis m_CurrentProsthesis;

	albaProsthesesDBManager *m_DBManager;
	std::vector<AuxProducer> m_ProducersVect;

	albaGUI *m_Gui; ///< Gui variable used to plug custom widgets

	wxBoxSizer *m_MainBoxSizer;
	wxBoxSizer *m_ImageSizer;

	wxComboBox *m_ImageComboBox;
	wxImage	*m_Image;
	albaGUIPicButton *m_ImageButton;
	wxString m_ImagePath;

	wxTextCtrl *m_NameTextCtrl;
	wxComboBox *m_ProducerComboBox;
	wxComboBox *m_TypeComboBox;

	std::vector<wxString> m_ProducerNameList;
	std::vector<wxString> m_TypeNameList;

	int m_SelectedProducer;
	int m_SelectedType;
	int m_SelectedSide;

};
#endif
