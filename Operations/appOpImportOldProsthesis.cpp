/*=========================================================================
Program:   AssemblerPro
Module:    appOpImportOldProsthesis.cpp
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

#include "appOpImportOldProsthesis.h"
#include "appDecl.h"
#include "appGUI.h"
#include "appLogic.h"

#include "albaGUI.h"
#include "albaVME.h"
#include "albaVMERoot.h"
#include "albaVMEStorage.h"
#include "albaMSFImporter.h"
#include "albaVMEIterator.h"
#include "albaTagArray.h"
#include "albaVMEGroup.h"
#include "albaMatrixVector.h"
#include "albaCrypt.h"
#include "vtkPolyDataReader.h"
#include "vtkALBASmartPointer.h"


//----------------------------------------------------------------------------
albaCxxTypeMacro(appOpImportOldProsthesis);

//----------------------------------------------------------------------------
appOpImportOldProsthesis::appOpImportOldProsthesis(wxString label) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = false;
}

//----------------------------------------------------------------------------
appOpImportOldProsthesis::~appOpImportOldProsthesis()
{
}

//----------------------------------------------------------------------------
bool appOpImportOldProsthesis::Accept(albaVME *node)
{
	return true;
}

//----------------------------------------------------------------------------
char** appOpImportOldProsthesis::GetIcon()
{
#include "pic/MENU_OP_IMPORT_PROSTHESIS.xpm"
	return MENU_OP_IMPORT_PROSTHESIS_xpm;
}

//----------------------------------------------------------------------------
albaOp* appOpImportOldProsthesis::Copy()
{
	appOpImportOldProsthesis *cp = new appOpImportOldProsthesis(m_Label);
	return cp;
}
//----------------------------------------------------------------------------
void appOpImportOldProsthesis::OpRun()
{
	albaString wildc = "MSF file (*.msf)|*.msf";
	wxString prosthesisFile = albaGetOpenFile(albaGetLastUserFolder().c_str(), wildc, "Select file").c_str();


	wxString path, name, ext;
	wxSplitPath(prosthesisFile.c_str(), &path, &name, &ext);

	albaVMEStorage *storage;
	storage = albaVMEStorage::New();
	albaVMERoot *root;
	root = storage->GetRoot();
	root->Initialize();
	root->SetName("RootB");

	albaMSFImporter *importer = new albaMSFImporter; //Import prosthesis MSF
	importer->SetURL(prosthesisFile.c_str());
	importer->SetRoot(root);
	importer->Restore();

	if (!root->GetFirstChild())
	{
		albaDEL(storage);
		wxMessageBox(_("Error Parsing!"));
		return;
	}

	bool modelMirror;
	double bendingAngle = 0;
	albaTagItem componentTypeTag, bendingAngleTag, modelMirrorTag;
	albaString componentTypeStr, bendingAngleStr, modelType, prosthesisFullName, prosthesisName, producer;
	albaProDBProsthesis::PRO_SIDES side;

	for (int i = 0; i < root->GetNumberOfChildren(); i++)
	{
		if (root->GetChild(i) && root->GetChild(i)->GetChild(0))
		{
			for (int j = 0; j < root->GetChild(i)->GetChild(0)->GetNumberOfChildren(); j++)
			{
				producer = root->GetChild(i)->GetChild(0)->GetName();

				if (root->GetChild(i)->GetChild(0)->GetChild(j))
				{
					//Getting model name
					prosthesisName = root->GetChild(i)->GetChild(0)->GetChild(j)->GetName();

					//Clean name and get model side
					if (strstr(prosthesisName, "Right") != NULL)
					{
						prosthesisName.NCopy(prosthesisName, prosthesisName.GetSize() - 7);
						side = albaProDBProsthesis::PRO_RIGHT;
					}
					else if (strstr(prosthesisName, "Left") != NULL)
					{
						prosthesisName.NCopy(prosthesisName, prosthesisName.GetSize() - 6);
						side = albaProDBProsthesis::PRO_LEFT;
					}

					//Ger Prosthesis Type
					root->GetChild(i)->GetChild(0)->GetChild(j)->GetTagArray()->GetTag("MODEL_COMPONENT_TYPE", componentTypeTag);
					componentTypeTag.GetValueAsSingleString(componentTypeStr);
					if (componentTypeTag.GetNumberOfComponents() == 0 || (!componentTypeStr.Equals("(\"Femoral\")") && !componentTypeStr.Equals("(\"Acetabular\")")))
					{
						cppDEL(importer);
						albaDEL(storage);
						OpStop(OP_RUN_CANCEL);
						return;
					}
					if (componentTypeStr.Equals("(\"Femoral\")"))
						modelType = "Femoral";
					else if (componentTypeStr.Equals("(\"Acetabular\")"))
						modelType = "Acetabular";

					//Get Prosthesis Mirror Flag
					root->GetChild(i)->GetChild(0)->GetChild(j)->GetTagArray()->GetTag("MODEL_MIRROR_FLAG", modelMirrorTag);
					if (modelMirrorTag.GetNumberOfComponents() == 0)
						modelMirror = false;
					else
						modelMirror = (int)	modelMirrorTag.GetValueAsDouble();

					//Get Prosthesis Bending Angle
					root->GetChild(i)->GetChild(0)->GetChild(j)->GetTagArray()->GetTag("PROSTESIS_BENDING_ANGLE", bendingAngleTag);
					if (bendingAngleTag.GetNumberOfComponents() == 0)
						bendingAngle = 0;
					else
						bendingAngle = bendingAngleTag.GetValueAsDouble();			
				}
				else
				{
					cppDEL(importer);
					albaDEL(storage);
					OpStop(OP_RUN_CANCEL);
					return;
				}
			}
		}
		else
		{
			cppDEL(importer);
			albaDEL(storage);
			OpStop(OP_RUN_CANCEL);
			return;
		}
	}


	appLogic *logic = (appLogic *)GetLogicManager();
	albaProsthesesDBManager *proDBMan = logic->GetProsthesesDBManager();

	if (proDBMan->HasProsthesis(prosthesisName, side))
	{
		albaString tmp;
		tmp.Printf("Prosthesis already %s exist in DB!\Do you want to override current prosthesis?", prosthesisName.GetCStr());
		int answare = wxMessageBox(tmp.GetCStr(), _("Ovverride Abort"), wxYES_NO | wxCENTER);
		if (answare == wxNO)
		{
			cppDEL(importer);
			albaDEL(storage);
			OpStop(OP_RUN_CANCEL);
			return;
		}
		else
		{
			proDBMan->DeleteProsthesis(prosthesisName, side);
		}
	}

	//Create producer if does not exists
	if (!proDBMan->HasProducer(producer))
	{
		albaProDBProducer *newProducer = new albaProDBProducer();
		//In the old format we have only producer name
		newProducer->SetName(producer);
		proDBMan->AddProducer(newProducer);
	}

	//Create type if does not exists
	if (!proDBMan->HasType(modelType))
	{
		albaProDBType *newType = new albaProDBType();
		//In the old format we have only producer name
		newType->SetName(modelType);
		proDBMan->AddType(newType);
	}

	albaProDBProsthesis *newProsthesis = new albaProDBProsthesis();
	newProsthesis->SetName(prosthesisName);
	newProsthesis->SetBendingAngle(bendingAngle);
	newProsthesis->SetProducer(producer);
	newProsthesis->SetSide(side);
	newProsthesis->SetType(modelType);
		
	albaVMEIterator * iter = root->NewIterator();
	for (albaVME *Inode = iter->GetFirstNode(); Inode; Inode = iter->GetNextNode())
	{
		if (Inode->GetTagArray()->GetTag("HIPOP_COMPONENT_VISIBILITY"))//Find if the node is a component of the Prosthesis DB
		{

			//Create Group and Set name
			albaProDBCompGroup *newGroup = new albaProDBCompGroup();
			newGroup->SetName(Inode->GetName());


			int numberOfComponents = Inode->GetNumberOfChildren();
			for (int i = 0; i < numberOfComponents; i++)
			{
				//Create Component
				albaVMEGroup * componentVME = albaVMEGroup::SafeDownCast(Inode->GetChild(i));
				albaProDBComponent *newComponent = new albaProDBComponent();

				//Get Name
				newComponent->SetName(componentVME->GetName());

				//Get Matrix
				albaVMEGroup * componentChild = albaVMEGroup::SafeDownCast(componentVME->GetChild(0));
				if(componentChild)
					newComponent->SetMatrix(*componentChild->GetMatrixVector()->GetMatrix(0));


				//Get VTK data
				albaString dataFile = path;
				dataFile << "\\";
				dataFile << componentVME->GetTagArray()->GetTag("HIPOP_DB_DATA_FILE")->GetValue();
				std::string fileMemory;
				albaDecryptFileInMemory(dataFile, fileMemory, "fattinonfostepervivercomebruti");
				if (fileMemory.empty())
				{
					albaLogMessage("Decryption Error! On file:%s", dataFile.GetCStr());
					wxMessageBox(_("Decryption Error!"));
					continue;
				}
				vtkALBASmartPointer<vtkPolyDataReader> reader;
				reader->SetFileName(dataFile.GetCStr());
				reader->ReadFromInputStringOn();
				reader->SetInputString(fileMemory.c_str(), fileMemory.size());
				reader->Update();
				vtkPolyData * vtkOutput = reader->GetOutput();
				newComponent->SetVTKData(vtkOutput);

				//Add component to Group
				newGroup->AddComponent(newComponent);
			}

			//Add Group to prosthesis
			newProsthesis->AddCompGroup(newGroup);
		}
	}

	//Add Prosthesis to DB
	proDBMan->AddProsthesis(newProsthesis);

	//Save DB 
	proDBMan->SaveDB();

	cppDEL(importer);
	albaDEL(storage);
	OpStop(OP_RUN_OK);
}
//----------------------------------------------------------------------------
void appOpImportOldProsthesis::OpStop(int result)
{
	albaEventMacro(albaEvent(this, result));
}
//----------------------------------------------------------------------------
void appOpImportOldProsthesis::OpDo()
{
}

