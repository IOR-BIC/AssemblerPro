/*=========================================================================
Program:   AssemblerPro
Module:    appGUI.cpp
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

#include "appDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "appDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "appGUI.h"
#include "appGUIHyperLink.h"
#include "appGUISeparator.h"

#include "albaGUIButton.h"
#include "albaGUIValidator.h"
#include "albaPics.h"
#include "albaVME.h"

enum APP_GUI_ID
{
	ID_LIST_DOUBLE_CLICK = albaVME::ID_LAST,
};

BEGIN_EVENT_TABLE(appGUI, albaGUI)
EVT_LISTBOX_DCLICK(wxID_ANY, appGUI::OnDClickList)
END_EVENT_TABLE()


const int  M = 1;											// margin all around a row  
const int LM = 5;											// label margin             
const int HM = 2 * M;										// horizontal margin        (2)

const int LH = 18;											// label/entry height       
const int BH = 20;											// button height            

																				//const int LW	= 60;											// label width
																				//const int EW	= 60;											// entry width  - (was 48)  


const int LW = 55;	// label width Windows
const int EW = 45;											// entry width  - (was 48)  
const int FW = LW + LM + EW + HM + EW + HM + EW;		// full width               (304)
const int DW = EW + HM + EW + HM + EW;					// Data Width - Full Width without the Label (184)

static wxPoint dp = wxDefaultPosition;

//-------------------------------------------------------------------------
appGUI::appGUI(albaObserver *listener)
	:albaGUI(listener)
{
}

//-------------------------------------------------------------------------
appGUI::~appGUI()
{
}

//-------------------------------------------------------------------------
void appGUI::OnDClickList(wxCommandEvent& event)
{
	albaEventMacro(albaEvent(this, ID_LIST_DOUBLE_CLICK));
}

//----------------------------------------------------------------------------
void appGUI::HyperLink(int id, wxString label, wxString url)
{
	int w_id = GetWidgetId(id);

	appGUIHyperLink  *link = NULL;
	link = new appGUIHyperLink(this, w_id, label);
	link->SetUrl(url);

	Add(link);
}

//----------------------------------------------------------------------------
void appGUI::Separator(long style, const wxSize& size /*= wxDefaultSize*/)
{
	Add(new appGUISeparator(this, style, size));
}

//-------------------------------------------------------------------------
void appGUI::TwoIntegers(int id, albaString label, int *var1, int *var2, int min /*= MININT*/, int max /*= MAXINT*/, albaString tooltip /*= ""*/, bool labelAlwaysEnable /*= false*/, double customSizer /*= 1.0*/)
{
	int w_id;
	int fw = FW;
	int lw = LW;
	int dw = DW;
	long labStyle = wxALIGN_RIGHT | wxST_NO_AUTORESIZE;

	if (customSizer < 1.0 && customSizer > 0.0)
	{
		lw = (fw * customSizer) - (2 * LM);
		dw = (fw - lw) - LM;
		fw *= customSizer;

		labStyle = wxALIGN_LEFT | wxST_NO_AUTORESIZE;
	}

	if (label.IsEmpty())
	{
		w_id = GetWidgetId(id);
		wxTextCtrl  *text = new wxTextCtrl(this, w_id, "", dp, wxSize(fw/2, LH), m_EntryStyle);
		text->SetValidator(albaGUIValidator(this, w_id, text, var1, min, max));
		text->SetFont(m_Font);
		if (!tooltip.IsEmpty())
			text->SetToolTip(tooltip.GetCStr());
		Add(text, 0, wxALL, M);

		w_id = GetWidgetId(id);
		wxTextCtrl  *text2 = new wxTextCtrl(this, w_id, "", dp, wxSize(fw/2, LH), m_EntryStyle);
		text2->SetValidator(albaGUIValidator(this, w_id, text2, var2, min, max));
		text2->SetFont(m_Font);
		if (!tooltip.IsEmpty())
			text2->SetToolTip(tooltip.GetCStr());

		wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add(text, 0, wxRIGHT, HM);
		sizer->Add(text2, 0, wxRIGHT, HM);
		Add(sizer, 0, wxALL, M);
	}
	else
	{
		wxStaticText *lab = new wxStaticText(this, labelAlwaysEnable ? -1 : GetWidgetId(id), label.GetCStr(), dp, wxSize(lw, LH), labStyle);
		if (m_UseBackgroundColor)
			lab->SetBackgroundColour(m_BackgroundColor);
		lab->SetFont(m_Font);

		w_id = GetWidgetId(id);
		wxTextCtrl  *text = new wxTextCtrl(this, w_id, "", dp, wxSize(dw/2, LH), m_EntryStyle);
		text->SetValidator(albaGUIValidator(this, w_id, text, var1, min, max));
		text->SetFont(m_Font);
		if (!tooltip.IsEmpty())
			text->SetToolTip(tooltip.GetCStr());

		w_id = GetWidgetId(id);
		wxTextCtrl  *text2 = new wxTextCtrl(this, w_id, "", dp, wxSize(dw/2, LH), m_EntryStyle);
		text2->SetValidator(albaGUIValidator(this, w_id, text2, var2, min, max));
		text2->SetFont(m_Font);
		if (!tooltip.IsEmpty())
			text2->SetToolTip(tooltip.GetCStr());

		wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add(lab, 0, wxRIGHT, LM);
		sizer->Add(text, 0, wxRIGHT, HM);
		sizer->Add(text2, 0, wxRIGHT, HM);
		Add(sizer, 0, wxALL, M);
	}
}

//-------------------------------------------------------------------------
void appGUI::ComponentButton(int id1, int id2, int id3, int* boolVar, wxString* textVar)
{
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	
	// CheckBox
	int w_id1 = GetWidgetId(id1);
	wxCheckBox *check = new wxCheckBox(this, w_id1, "", dp, wxSize(-1, -1), m_EntryStyle);
	check->SetValidator(albaGUIValidator(this, w_id1, check, boolVar));
	check->SetFont(m_Font);
	check->SetToolTip("Show/Hide");

	sizer->Add(check, 0, wxALL, M);

	// TextCtrl
	int w_id2 = GetWidgetId(id2);
	wxTextCtrl *text = new wxTextCtrl(this, w_id2, "", dp, wxSize(LW+97, LH));
	text->SetValidator(albaGUIValidator(this, w_id2, text, textVar, true));
	text->SetFont(m_Font);
	text->SetToolTip("Group Name");
	
	sizer->Add(text, 0, wxALL, M);

	// Button
	int w_id3 = GetWidgetId(id3);
	albaGUIButton *butt = new albaGUIButton(this, w_id3, "Del", dp, wxSize(30, BH));
	butt->SetValidator(albaGUIValidator(this, w_id3, butt));
	butt->SetFont(m_Font);
	butt->SetToolTip("Delete Group");

	sizer->Add(butt, 0, wxALL, M);

	//
	Add(sizer, 0, wxALL, M);
}
