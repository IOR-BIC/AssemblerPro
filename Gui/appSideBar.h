/*=========================================================================
Program:   Albedo
Module:    appSideBar.h
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

#ifndef __appSideBar_H__
#define __appSideBar_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/notebook.h>
#include <wx/splitter.h>
#include <wx/utils.h>

#include "albaEvent.h"
#include "albaObserver.h"
#include "albaGUICheckTree.h"
#include "albaGUIVMEChooserTree.h"
#include "albaAbsSideBar.h"
//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class albaGUIHolder;
class albaGUI;
class albaGUIPanelStack;
class albaGUIPanel;
class albaVME;
class albaView;
class albaGUINamedPanel;
class albaGUISplittedPanel;
class albaGUIMDIFrame;
//----------------------------------------------------------------------------
// appSideBar :
//----------------------------------------------------------------------------
class ALBA_EXPORT appSideBar : public albaAbsSideBar
{
public:
	appSideBar(albaGUIMDIFrame* parent, int id, albaObserver *Listener, long style = DOUBLE_NOTEBOOK);

	~appSideBar();

	enum SIDEBAR_STYLE
	{
		SINGLE_NOTEBOOK,
		DOUBLE_NOTEBOOK
	};

	// Description:
	// Add a new vme into the tree.
	void VmeAdd(albaVME *vme);

	// Description:
	// Remove a vme from the tree.
	void VmeRemove(albaVME *vme);

	// Description:
	// Notify to the tree that a vme is modified.
	void VmeModified(albaVME *vme);

	// Description:
	// Notify to the tree the visibility of the vme.
	void VmeShow(albaVME *vme, bool visibility);

	// Description:
	// Notify to the tree that the vme has been selected.
	void VmeSelected(albaVME *vme);

	// Description:
	// Show the operation's parameters gui on the tabbed panel.
	void OpShowGui(bool push_gui, albaGUIPanel *panel);

	// Description:
	// Hide the view/operation's gui from the tabbed panel.
	void OpHideGui(bool view_closed);

	// Description:
	// Plug the view settings on the tabbed panel.
	void ViewSelect(albaView *view);

	// Description:
	// Notify to the tree that a view has been deleted.
	void ViewDeleted(albaView *view);

	// Description:
	// Enable/Disable the vme selection when an operation ends/start.
	void EnableSelect(bool enable);

	// Description:
	// Switch the visibility of the panel.
	void Show();

	bool IsShown() { return m_Notebook->IsShown(); };
	void Show(bool show) { m_Notebook->Show(show); };

	/** Respond to a VME_CHOOSE evt. Build a dialog containing the vme tree and return the vme choosed from the user. */
	std::vector<albaVME*> VmeChoose(void *vme_accept_function = 0, long style = REPRESENTATION_AS_TREE, albaString title = "Choose Node", bool multiSelect = false, albaVME *vme = NULL);

	/** Open a Find VME dialog.*/
	void FindVME();

protected:

	void InitMainPanel();
	void InitLeftPanel();

	void UpdateVmePanel();
	void UpdateProsthesisPanel();

	albaGUIMDIFrame* m_Parent;
	int m_Id;
	int m_Style;

	wxNotebook        *m_Notebook;
	wxNotebook        *m_VmeNotebook;
	wxSplitterWindow *m_SideSplittedPanel;

	albaGUIPanelStack	*m_OpPanel;
	albaGUIHolder	*m_ViewPropertyPanel;
	albaGUIHolder	*m_ProsthesisPropertyPanel;
	albaGUICheckTree  *m_Tree;

	albaGUIHolder  *m_VmePanel;
	albaGUIHolder  *m_VmePipePanel;

	albaVME     *m_SelectedVme;
	albaView     *m_SelectedView;
	albaObserver *m_Listener;

	albaGUI *m_AppendingGUI;

	albaGUI *m_CurrentVmeGui;
	albaGUI *m_CurrentVmeOutputGui;
	albaGUI *m_CurrentPipeGui;

	wxNotebook	*m_LeftNotebook;
	albaGUIHolder  *m_InfoPanel;
};
#endif