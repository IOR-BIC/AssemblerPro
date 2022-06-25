/*=========================================================================
Program:   AssemblerPro
Module:    appOpExportOldProsthesis.cpp
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

#include "appOpExportOldProsthesis.h"
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
#include "albaXMLString.h"
#include "wx/dir.h"
#include "wx/filename.h"
#include "albaVMEProsthesis.h"
#include "albaString.h"
#include "albaMSFImporter.h"
#include "albaVMEStorage.h"
#include "albaVMERoot.h"
#include "albaVMEGroup.h"
#include "mmuDOMTreeErrorReporter.h"
#include "albaXMLElement.h"
#include "mmuXMLDOMElement.h"
#include "albaXMLString.h"
#include "albaTagArray.h"


#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/sax/ErrorHandler.hpp>

//----------------------------------------------------------------------------
albaCxxTypeMacro(appOpExportOldProsthesis);

//----------------------------------------------------------------------------
appOpExportOldProsthesis::appOpExportOldProsthesis(wxString label) :albaOp(label)
{
	m_OpType = OPTYPE_EXPORTER;
	m_Canundo = false;
}

//----------------------------------------------------------------------------
appOpExportOldProsthesis::~appOpExportOldProsthesis()
{
}

//----------------------------------------------------------------------------
bool appOpExportOldProsthesis::InternalAccept(albaVME *node)
{
	return true;
}

//----------------------------------------------------------------------------
char** appOpExportOldProsthesis::GetIcon()
{
#include "pic/MENU_OP_IMPORT_PROSTHESIS.xpm"
	return MENU_OP_IMPORT_PROSTHESIS_xpm;
}

//----------------------------------------------------------------------------
albaOp* appOpExportOldProsthesis::Copy()
{
	appOpExportOldProsthesis *cp = new appOpExportOldProsthesis(m_Label);
	return cp;
}
//----------------------------------------------------------------------------
void appOpExportOldProsthesis::OpRun()
{
	albaString wildc = "Zip file (*.zip)|*.zip";
	wxString prosthesisFile = albaGetSaveFile(albaGetLastUserFolder().c_str(), wildc, "Select file").c_str();


	wxString path, name, ext;
	wxSplitPath(prosthesisFile.c_str(), &path, &name, &ext);

	albaString dbDir = GetLogicManager()->GetProsthesesDBManager()->GetDBDir();

	wxString tmpFolder = albaGetApplicationDirectory().c_str();
	tmpFolder += "\\tmpExport\\";


	//Create or clean tmp export dir
	if (!wxFileName::DirExists(tmpFolder))
	{
		wxFileName::Mkdir(tmpFolder);
	}
	else
	{
		bool dir_empty = false;
		wxDir dir(tmpFolder);
		if (dir.IsOpened())
		{
			wxString file;

			bool cont = dir.GetFirst(&file);
			file = tmpFolder + wxFileName::GetPathSeparator() + file;
			while (cont)
			{
				if (wxFileName::FileExists(file)) 
					wxRemoveFile(file);
				
				cont = dir.GetNext(&file);
				file = tmpFolder + wxFileName::GetPathSeparator() + file;
			}
		}
	}


	appLogic *logic = (appLogic *)GetLogicManager();
	
	albaVMEProsthesis *prosthesis =  (albaVMEProsthesis *)logic->GetVMEProthesis();

	albaProDBProsthesis *dbPro = prosthesis->GetDBProsthesis();
	albaString proName = dbPro->GetName();
	albaProDBProsthesis::PRO_SIDES proSide = dbPro->GetSide();


	if (proSide == albaProDBProsthesis::PRO_SIDES::PRO_LEFT)
	{
		ExportProsthesis(tmpFolder, dbPro, 0);
	}
	else if (proSide == albaProDBProsthesis::PRO_SIDES::PRO_RIGHT)
	{
		ExportProsthesis(tmpFolder, dbPro, 1);
	}
	else //BOTH
	{
		ExportProsthesis(tmpFolder, dbPro, 0);
		ExportProsthesis(tmpFolder, dbPro, 1);
	}

	OpStop(OP_RUN_OK);
}




void AddTagItem(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc, XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * tarray, char *tagName, int mult, char *type, const char *tagContent)
{
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *tItem = doc->createElement(albaXMLString("TItem"));
	tItem->setAttribute(albaXMLString("Tag"), albaXMLString(tagName));
	tItem->setAttribute(albaXMLString("Mult"), albaXMLString(albaString(mult).GetCStr()));
	tItem->setAttribute(albaXMLString("Type"), albaXMLString(type));

	if (mult > 0)
	{
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *tContent = doc->createElement(albaXMLString("TC"));
		tItem->appendChild(tContent);

		XERCES_CPP_NAMESPACE_QUALIFIER DOMText *text_node = doc->createTextNode(albaXMLString(tagContent));
		tContent->appendChild(text_node);
	}
	tarray->appendChild(tItem);
}

void appOpExportOldProsthesis::AddMatrix(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument * doc, XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * node, albaMatrix *matrix,const char *nextGroup)
{

	albaString elements;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			elements << albaString(matrix->GetElements()[i][j]) << " ";
		}
	}
	
	//<VME Type = "mflVMEGroup" Name = "insert">
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *vme = doc->createElement(albaXMLString("VME"));


	vme->setAttribute(albaXMLString("Type"), albaXMLString("mflVMEGroup"));
	vme->setAttribute(albaXMLString("Name"), albaXMLString(nextGroup));
	node->appendChild(vme);
	
	//<VMatrix>
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *vmatrix = doc->createElement(albaXMLString("VMatrix"));
	vme->appendChild(vmatrix);

	// <Matrix TimeStamp="0"> 1 0 0 0  0 1 0 0  0 0 1 0  0 0 0 1  </Matrix>
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *matrixNode = doc->createElement(albaXMLString("Matrix"));
	matrixNode->setAttribute(albaXMLString("TimeStamp"), albaXMLString("0"));
	vmatrix->appendChild(matrixNode);


	XERCES_CPP_NAMESPACE_QUALIFIER DOMText *text_node = doc->createTextNode(albaXMLString(elements));
	matrixNode->appendChild(text_node);
	
}

//----------------------------------------------------------------------------
void appOpExportOldProsthesis::ExportProsthesis(wxString tmpFolder, albaProDBProsthesis * dbPro, int side)
{
	albaString filename = tmpFolder;
	filename += dbPro->GetName();
	filename += side ? "_Right.msf" : "_Left.msf";

	albaString dbDir = GetLogicManager()->GetProsthesesDBManager()->GetDBDir();


	//Open the file xml with manufacture and model information
	try {
		XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Initialize();
	}
	catch (const XERCES_CPP_NAMESPACE_QUALIFIER XMLException& toCatch) {
		// Do your failure processing here
		return;
	}

	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc;
	XMLCh tempStr[100];
	XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("LS", tempStr, 99);
	XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementation *impl = XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementationRegistry::getDOMImplementation(tempStr);
	XERCES_CPP_NAMESPACE_QUALIFIER DOMWriter* theSerializer = ((XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementationLS*)impl)->createDOMWriter();
	theSerializer->setNewLine(albaXMLString("\r"));
	doc = impl->createDocument(NULL, albaXMLString("MSF"), NULL);

	doc->setEncoding(albaXMLString("UTF-8"));
	doc->setStandalone(true);
	doc->setVersion(albaXMLString("1.0"));

	// optionally you can set some features on this serializer
	if (theSerializer->canSetFeature(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTDiscardDefaultContent, true))
		theSerializer->setFeature(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTDiscardDefaultContent, true);

	if (theSerializer->canSetFeature(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTFormatPrettyPrint, true))
		theSerializer->setFeature(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTFormatPrettyPrint, true);




	XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatTarget *XMLTarget;
	XMLTarget = new XERCES_CPP_NAMESPACE_QUALIFIER LocalFileFormatTarget(filename.GetCStr());

	// extract root element and wrap it with an albaXMLElement object
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *root = doc->getDocumentElement();
	assert(root);	

	// attach version attribute to the root node
	root->setAttribute(albaXMLString("Version"), albaXMLString("1.2"));
	root->setAttribute(albaXMLString("MaxItemId"), albaXMLString("-1"));


	//Producer
	//<VME Type = "mflVMEGroup" Name = "Adler Ortho">
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *firstProducer = doc->createElement(albaXMLString("VME"));
	firstProducer->setAttribute(albaXMLString("Type"), albaXMLString("mflVMEGroup"));
	firstProducer->setAttribute(albaXMLString("Name"), albaXMLString(dbPro->GetProducer().GetCStr()));
	root->appendChild(firstProducer);

	//again
	//<VME Type = "mflVMEGroup" Name = "Adler Ortho">  
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *secondProducer = doc->createElement(albaXMLString("VME"));
	secondProducer->setAttribute(albaXMLString("Type"), albaXMLString("mflVMEGroup"));
	secondProducer->setAttribute(albaXMLString("Name"), albaXMLString(dbPro->GetProducer().GetCStr()));
	firstProducer->appendChild(secondProducer);


	//Prosthesis
	albaString name = dbPro->GetName();
	name += side ? "Right" : "Left";

	//<VME Type="mflVMEGroup" Name="Agilis-PE Right"> 
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *prosthesis = doc->createElement(albaXMLString("VME"));
	prosthesis->setAttribute(albaXMLString("Type"), albaXMLString("mflVMEGroup"));
	prosthesis->setAttribute(albaXMLString("Name"), albaXMLString(dbPro->GetProducer().GetCStr()));
	secondProducer->appendChild(prosthesis);


	//Tag Array
	//<TArray NumberOfTags = "2">
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *tarray = doc->createElement(albaXMLString("TArray"));
	tarray->setAttribute(albaXMLString("NumberOfTags"), albaXMLString("2"));
	prosthesis->appendChild(tarray);

	//	<TItem Tag = "MODEL_MIRROR_FLAG" Mult="0" Type="NUM">  <TC>1</TC> </TItem>
	AddTagItem(doc, tarray, "MODEL_MIRROR_FLAG", 1, "MUM", "1");

	//<TItem Tag="MODEL_COMPONENT_TYPE" Mult="1" Type="STR">
	//   <TC>Acetabular</TC>
	//</TItem>
	AddTagItem(doc, tarray, "MODEL_COMPONENT_TYPE", 1, "STR", "Acetabular");
	
	std::vector<albaProDBCompGroup *> *compGroups = dbPro->GetCompGroups();

	for (int i = 0; i < compGroups->size(); i++)
	{
		albaProDBCompGroup *group = compGroups->at(i);
		//<VME Type = "mflVMEGroup" Name = "Adler Ortho">  
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *groupNode= doc->createElement(albaXMLString("VME"));
		groupNode->setAttribute(albaXMLString("Type"), albaXMLString("mflVMEGroup"));
		groupNode->setAttribute(albaXMLString("Name"), albaXMLString(group->GetName().GetCStr()));
		prosthesis->appendChild(groupNode);


		//<TArray NumberOfTags="1">
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *groupTarray = doc->createElement(albaXMLString("TArray"));
		groupTarray->setAttribute(albaXMLString("NumberOfTags"), albaXMLString("1"));
		groupNode->appendChild(groupTarray);

		//<TItem Tag="HIPOP_MOUNTED_SIZE" Mult="1" Type="STR">
		//	<TC/>
		//</TItem>
		AddTagItem(doc, groupTarray, "HIPOP_MOUNTED_SIZE", 1, "STR", "");
		
		std::vector<albaProDBComponent *> * components = group->GetComponents();
		for (int j = 0; j < components->size(); j++)
		{
			albaProDBComponent *component = components->at(j);

			//<VME Type="mflVMEGroup" Name="fixa42 a">
			XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *componentNode = doc->createElement(albaXMLString("VME"));
			componentNode->setAttribute(albaXMLString("Type"), albaXMLString("mflVMEGroup"));
			componentNode->setAttribute(albaXMLString("Name"), albaXMLString(component->GetName().GetCStr()));
			groupNode->appendChild(componentNode);

			//<TArray NumberOfTags="1">
			XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *compTarray = doc->createElement(albaXMLString("TArray"));
			compTarray->setAttribute(albaXMLString("NumberOfTags"), albaXMLString("1"));
			componentNode->appendChild(compTarray);

			//<TItem Tag="HIPOP_DB_DATA_FILE" Mult="1" Type="STR">
			//	<TC>fixa42 a.cry</TC>
			//</TItem>
			albaString dataFileName = component->GetDataFileName();
			AddTagItem(doc, compTarray, "HIPOP_DB_DATA_FILE", 1, "STR",dataFileName.GetCStr());

			ConvertFile(dbDir + dataFileName.GetCStr(), tmpFolder + dataFileName.GetCStr());

			if (i < compGroups->size() - 1)
			{
				albaProDBCompGroup *nextGroup = compGroups->at(i+1);
				AddMatrix(doc, componentNode, &component->GetMatrix(), nextGroup->GetName().GetCStr());
			}
		}
	}

	try {
		// do the serialization through DOMWriter::writeNode();
		theSerializer->writeNode(XMLTarget, *doc);
	}
	catch (const XERCES_CPP_NAMESPACE_QUALIFIER  XMLException& toCatch) {
		char* message = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(toCatch.getMessage());
		XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&message);
		return;
	}
	catch (const XERCES_CPP_NAMESPACE_QUALIFIER DOMException& toCatch) {
		char* message = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(toCatch.msg);
		XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&message);
		return;
	}
	catch (...) {
		return;
	}

	theSerializer->release();
	cppDEL(XMLTarget);
	doc->release();

	XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Terminate();

}

//----------------------------------------------------------------------------
void appOpExportOldProsthesis::ConvertFile(albaString source, albaString dest)
{
	std::string fileMemory;
	albaDecryptFileInMemory(source, fileMemory, GetLogicManager()->GetProsthesesDBManager()->GetPassPhrase());
	albaEncryptFileFromMemory(fileMemory.c_str(),fileMemory.length(), dest, "fattinonfostepervivercomebruti");
}


//----------------------------------------------------------------------------
void appOpExportOldProsthesis::OpStop(int result)
{
	albaEventMacro(albaEvent(this, result));
}


//----------------------------------------------------------------------------
void appOpExportOldProsthesis::OpDo()
{
}

