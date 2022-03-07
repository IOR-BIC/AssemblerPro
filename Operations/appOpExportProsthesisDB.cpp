/*=========================================================================
Program:   AssemblerPro
Module:    appOpExportProsthesisDB.cpp
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

#include "appOpExportProsthesisDB.h"
#include "appDecl.h"
#include "appGUI.h"
#include "appUtils.h"

#include "albaGUI.h"
#include "albaGUICheckListBox.h"
#include "albaVME.h"
#include "albaProsthesesDBManager.h"

#include <wx/zipstrm.h>
#include <wx/zstream.h>
#include <wx/ffile.h>
#include <wx/wfstream.h>

//----------------------------------------------------------------------------
albaCxxTypeMacro(appOpExportProsthesisDB);

//----------------------------------------------------------------------------
appOpExportProsthesisDB::appOpExportProsthesisDB(wxString label) :albaOp(label)
{
	m_OpType = OPTYPE_EXPORTER;
	m_Canundo = true;

	m_DBName = "";
	m_DBVersion = "1.0";

	m_ProducerCheckBox = NULL;
	m_TypeCheckBox = NULL;
	m_ProsthesisCheckBox = NULL;

	m_SelectAllProducers = 1;
	m_SelectAllTypes = 1;
	m_SelectAllModels = 1;
}

//----------------------------------------------------------------------------
appOpExportProsthesisDB::~appOpExportProsthesisDB()
{
}

//----------------------------------------------------------------------------
bool appOpExportProsthesisDB::InternalAccept(albaVME *node)
{
	//return node->IsA("...");
	return true;
}

//----------------------------------------------------------------------------
char** appOpExportProsthesisDB::GetIcon()
{
#include "pic/MENU_OP_EXPORT_PROSTHESIS.xpm"
	return MENU_OP_EXPORT_PROSTHESIS_xpm;
}

//----------------------------------------------------------------------------
albaOp* appOpExportProsthesisDB::Copy()
{
	appOpExportProsthesisDB *cp = new appOpExportProsthesisDB(m_Label);
	return cp;
}
//----------------------------------------------------------------------------
void appOpExportProsthesisDB::OpRun()
{
	m_ProsthesesDBManager = GetLogicManager()->GetProsthesesDBManager();

	if (!m_TestMode)
	{
		CreateGui();
		UpdateGui();
	}
}
//----------------------------------------------------------------------------
void appOpExportProsthesisDB::OpStop(int result)
{
	if (result == OP_RUN_OK)
	{
		ExportDB();
	}

	if (!m_TestMode)
	{
		HideGui();
	}

	m_ProducerVect.clear();
	m_TypeVect.clear();
	m_ProsthesisVect.clear();

	albaEventMacro(albaEvent(this, result));
}
//----------------------------------------------------------------------------
void appOpExportProsthesisDB::OpDo()
{
}

//----------------------------------------------------------------------------
void appOpExportProsthesisDB::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch (e->GetId())
		{
		case wxOK:
			OpStop(OP_RUN_OK);
			break;

		case wxCANCEL:
			OpStop(OP_RUN_CANCEL);
			break;

		case ID_PRODUCER_SELECTION:
		case ID_TYPE_SELECTION:
			UpdateGui();
			break;

		default:
			Superclass::OnEvent(alba_event);
			break;
		}
	}
}

//----------------------------------------------------------------------------
void appOpExportProsthesisDB::CreateGui()
{
	// Interface:
	m_Gui = new appGUI(this);

	//m_Gui->String(NULL, "Name", &m_DBName);
	//m_Gui->String(NULL, "Version", &m_DBVersion);
	m_Gui->Label("Filters:");
	m_Gui->Divider(1);

	m_ProducerCheckBox = m_Gui->CheckList(ID_PRODUCER_SELECTION, "Producer");
	//m_Gui->Bool(NULL, "Select All", &m_SelectAllProducers);
	m_Gui->Divider(1);

	m_TypeCheckBox = m_Gui->CheckList(ID_TYPE_SELECTION, "Type", 30);
	//m_Gui->Bool(NULL, "Select All", &m_SelectAllTypes);
	m_Gui->Divider(1);

	m_Gui->Label("Prosthesis", true);
	m_ProsthesisCheckBox = m_Gui->CheckList(ID_MODEL_SELECTION, "", 180);
	//m_Gui->Bool(NULL, "Select All", &m_SelectAllModels);
	m_Gui->Divider(1);

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();
	m_Gui->Label("");

	//////////////////////////////////////////////////////////////////////////
	// Update Producer CheckBox
	std::vector<albaProDBProducer *> DBproducer = m_ProsthesesDBManager->GetProducers();

	m_ProducerCheckBox->Clear();
	m_ProducerVect.clear();

	for (int p = 0; p < DBproducer.size(); p++)
	{
		wxString name = wxString::Format("%s", DBproducer[p]->GetName().GetCStr());
		m_ProducerCheckBox->AddItem(NULL, name, true);
		m_ProducerVect.push_back(DBproducer[p]);
	}

	// Update Type CheckBox
	std::vector<albaProDBType *> DBtypes = m_ProsthesesDBManager->GetTypes();

	m_TypeCheckBox->Clear();
	m_TypeVect.clear();

	for (int t = 0; t < DBtypes.size(); t++)
	{
		wxString name = wxString::Format("%s", DBtypes[t]->GetName().GetCStr());
		m_TypeCheckBox->AddItem(NULL, name, true);
		m_TypeVect.push_back(DBtypes[t]);
	}

	ShowGui();
}

//----------------------------------------------------------------------------
void appOpExportProsthesisDB::UpdateGui()
{
	// Update Prosthesis CheckBox
	std::vector<albaProDBProsthesis *> DBprosthesis = m_ProsthesesDBManager->GetProstheses();

	m_ProsthesisCheckBox->Clear();
	m_ProsthesisVect.clear();
	for (int m = 0; m < DBprosthesis.size(); m++)
	{
		int p = m_ProducerCheckBox->FindItem(DBprosthesis[m]->GetProducer().GetCStr());
		int t = m_TypeCheckBox->FindItem(DBprosthesis[m]->GetType().GetCStr());

		if (m_ProducerCheckBox->IsItemChecked(p) && m_TypeCheckBox->IsItemChecked(t))
		{
			wxString name = wxString::Format("%s - %s", DBprosthesis[m]->GetProducer().GetCStr(), DBprosthesis[m]->GetName().GetCStr());
			m_ProsthesisCheckBox->AddItem(NULL, name, true);
			m_ProsthesisVect.push_back(DBprosthesis[m]);
		}
	}

	m_Gui->Update();
}

//----------------------------------------------------------------------------
void appOpExportProsthesisDB::ExportDB()
{
	albaString fileNameFullPath = albaGetDocumentsDirectory().c_str();
	fileNameFullPath.Append("\\newDB.zip");

	albaString wildc = "DB file (*.zip)|*.zip";
	albaString newFileName = albaGetSaveFile(fileNameFullPath.GetCStr(), wildc, "Save DB", 0, false).c_str();

	if (!newFileName.IsEmpty())
	{
		wxString path, name, ext;
		wxSplitPath(newFileName, &path, &name, &ext);

		albaString xmlFileName = path + "\\" + name + ".xml";
		albaString zipFileName = path + "\\" + name + ".zip";
		wxArrayString files;

		wxString DBPath = m_ProsthesesDBManager->GetDBDir();
		wxString configPath = albaGetConfigDirectory().c_str();

		//files.Add(configPath + "/Wizard/Producer.bmp"); // Add image file to ZIP (default)
		//files.Add(configPath + "/Wizard/Model.bmp"); // Add image file to ZIP (default)

		// Create AuxDB
		albaProsthesesDBManager *auxDBManager = new albaProsthesesDBManager();

		// Producer
		for (int p = 0; p < m_ProducerCheckBox->GetNumberOfItems(); p++)
		{
			if (m_ProducerCheckBox->IsItemChecked(p))
			{
				auxDBManager->AddProducer(m_ProducerVect[p]);

				albaString imageName = m_ProducerVect[p]->GetImgFileName();
				if (!imageName.IsEmpty())
				{
					wxString imagePath = DBPath + "/" + imageName;

					if (wxFileExists(imagePath))
						files.Add(imagePath); // Add image files to ZIP
				}
			}
		}

		// Type
		for (int t = 0; t < m_TypeCheckBox->GetNumberOfItems(); t++)
		{
			if (m_TypeCheckBox->IsItemChecked(t))
				auxDBManager->AddType(m_TypeVect[t]);
		}

		// Prosthesis
		for (int m = 0; m < m_ProsthesisCheckBox->GetNumberOfItems(); m++)
		{
			if (m_ProsthesisCheckBox->IsItemChecked(m))
			{
				auxDBManager->AddProsthesis(m_ProsthesisVect[m]);

				albaString imageName = m_ProsthesisVect[m]->GetImgFileName();
				if (!imageName.IsEmpty())
				{
					wxString imagePath = DBPath + "/" + imageName;

					if (wxFileExists(imagePath))
						files.Add(imagePath); // Add image files to ZIP
				}

				for (int g = 0; g < m_ProsthesisVect[m]->GetCompGroups()->size(); g++)
				{
					for (int c = 0; c < m_ProsthesisVect[m]->GetCompGroups()->at(g)->GetComponents()->size(); c++)
					{
						wxString dataPath = DBPath;
						dataPath.Append("/"+ m_ProsthesisVect[m]->GetCompGroups()->at(g)->GetComponents()->at(c)->GetDataFileName());

						if (wxFileExists(dataPath) && !IsIn(files, dataPath))
							files.Add(dataPath); // Add cry files to ZIP
					}
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////

		auxDBManager->SaveDBToFile(xmlFileName);	
		delete auxDBManager;

		files.Add(xmlFileName.GetCStr()); // Add xml to ZIP

		// Make Zip
		if (!MakeZip(zipFileName.GetCStr(), &files))
		{
			albaMessage(_("Failed to create compressed archive!"), _("Error"));
		}

		wxRemoveFile(xmlFileName.GetCStr());
	}
}

//----------------------------------------------------------------------------
bool appOpExportProsthesisDB::IsIn(wxArrayString as, wxString string)
{
	for (int s = 0; s < as.size(); s++)
		if(as[s] == string) return true;

	return false;
}

//----------------------------------------------------------------------------
bool appOpExportProsthesisDB::MakeZip(const albaString &zipname, wxArrayString *files)
{
	wxString name, path, short_name, ext;
	wxFileOutputStream out(zipname.GetCStr());
	wxZipOutputStream zip(out);

	if (!out || !zip)
		return false;

	for (size_t i = 0; i < files->GetCount(); i++)
	{
		name = files->Item(i);
		wxSplitPath(name, &path, &short_name, &ext);
		short_name += ".";
		short_name += ext;

		if (wxDirExists(name))
		{
			if (!zip.PutNextDirEntry(name)) // put the file inside the archive
				return false;
		}
		else
		{
			wxFFileInputStream in(name);

			if (in.Ok())
			{
				wxDateTime dt(wxFileModificationTime(name)); // get the file modification time

				if (!zip.PutNextEntry(short_name, dt, in.GetLength()) || !zip.Write(in) || !in.Eof()) // put the file inside the archive
					return false;
			}
		}
	}

	return zip.Close() && out.Close();
}