/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaProsthesisDBManager
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaDecl.h"
#include "albaProsthesisDBManager.h"
#include <wx/tokenzr.h>
#include "albaLogicWithManagers.h"
#include "wx/dir.h"
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

//albaProsthesisDBManager defines
#define PRODB_NAME "ProstesesDB"
#define DB_VERSION "1.0"
#define NODE_PRODUCERS "Producers"
#define NODE_TYPES "Types"
#define NODE_PROSTHESES "Prostheses"
#define NODE_PRODUCER "Producer"
#define NODE_TYPE "Type"
#define NODE_PROSTHESIS "Prosthesis"
#define NODE_COMPONENTS "Components"
#define NODE_COMPONENT "Component"
#define NODE_MATRIX "Matrix"
#define ATTR_VERSION "Version"
#define ATTR_NAME "Name"
#define ATTR_IMG "IMG"
#define ATTR_SITE "Site"
#define ATTR_SIDE "Side"
#define ATTR_TYPE "Type"
#define ATTR_PRODUCER "Producer"
#define ATTR_FILE "File"


//----------------------------------------------------------------------------
albaProsthesisDBManager::albaProsthesisDBManager()
{
	m_DBFilename = (albaGetAppDataDirectory()).c_str();
	m_DBFilename += "\\ProsthesesDB\\";

	if(!wxDir::Exists(m_DBFilename.GetCStr()))
		wxMkdir(m_DBFilename.GetCStr());

	m_DBFilename += "ProsthesesDB.xml";

	LoadDB();
	
	//TMP TEST
/*
	m_DBFilename = (albaGetAppDataDirectory()).c_str();
	m_DBFilename += "\\ProsthesesDB\\";

	if (!wxDir::Exists(m_DBFilename.GetCStr()))
		wxMkdir(m_DBFilename.GetCStr());

	m_DBFilename += "ProsthesesDB2.xml";

	SaveDB();
*/
}

//----------------------------------------------------------------------------
albaProsthesisDBManager::~albaProsthesisDBManager()
{

}


//----------------------------------------------------------------------------
int albaProsthesisDBManager::LoadDB()
{
	if (!wxFileExists(m_DBFilename.GetCStr()))
		return ALBA_OK;

	//Open the file xml with manufacture and model information
	try {
		XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Initialize();
	}
	catch (const XERCES_CPP_NAMESPACE_QUALIFIER XMLException& toCatch) {
		// Do your failure processing here
		return ALBA_ERROR;
	}

	XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser *XMLParser = new  XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser;

	XMLParser->setValidationScheme(XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser::Val_Auto);
	XMLParser->setDoNamespaces(false);
	XMLParser->setDoSchema(false);
	XMLParser->setCreateEntityReferenceNodes(false);

	mmuDOMTreeErrorReporter *errReporter = new mmuDOMTreeErrorReporter();
	XMLParser->setErrorHandler(errReporter);

	try {
		XMLParser->parse(m_DBFilename.GetCStr());
		int errorCount = XMLParser->getErrorCount();

		if (errorCount != 0)
		{
			// errors while parsing...
			albaErrorMessage("Errors while parsing XML file");
			return ALBA_ERROR;
		}

		// extract the root element and wrap inside a albaXMLElement
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc = XMLParser->getDocument();
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *root = doc->getDocumentElement();
		assert(root);

		if (CheckNodeElement(root, PRODB_NAME))
		{
			albaString version = GetElementAttribute(root, ATTR_VERSION);

			//Check the DB version
			if (version != DB_VERSION)
			{
				albaLogMessage("Wrong DB Version:\n DB version:%s, Software Version:%s", version.GetCStr(), DB_VERSION);
				return ALBA_ERROR;
			}
		}
		else
		{
			albaLogMessage("Wrong DB check root node");
			return ALBA_ERROR;
		}

		Clear();

		Load(root);
	}
	catch (...) {
		return ALBA_ERROR;
	}

	cppDEL(errReporter);
	delete XMLParser;

	// terminate the XML library
	XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Terminate();

	albaLogMessage(_("DB Loaded"));

	return ALBA_OK;
}

//----------------------------------------------------------------------------
int albaProsthesisDBManager::SaveDB()
{
	//Open the file xml with manufacture and model information
	try {
		XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Initialize();
	}
	catch (const XERCES_CPP_NAMESPACE_QUALIFIER XMLException& toCatch) {
		// Do your failure processing here
		return ALBA_ERROR;
	}

	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc;
	XMLCh tempStr[100];
	XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("LS", tempStr, 99);
	XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementation *impl = XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementationRegistry::getDOMImplementation(tempStr);
	XERCES_CPP_NAMESPACE_QUALIFIER DOMWriter* theSerializer = ((XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementationLS*)impl)->createDOMWriter();
	theSerializer->setNewLine(albaXMLString("\r"));
	doc = impl->createDocument(NULL, albaXMLString(PRODB_NAME), NULL);

	doc->setEncoding(albaXMLString("UTF-8"));
	doc->setStandalone(true);
	doc->setVersion(albaXMLString("1.0"));

	// optionally you can set some features on this serializer
	if (theSerializer->canSetFeature(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTDiscardDefaultContent, true))
		theSerializer->setFeature(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTDiscardDefaultContent, true);

	if (theSerializer->canSetFeature(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTFormatPrettyPrint, true))
		theSerializer->setFeature(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTFormatPrettyPrint, true);

	XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatTarget *XMLTarget;
	XMLTarget = new XERCES_CPP_NAMESPACE_QUALIFIER LocalFileFormatTarget(m_DBFilename.GetCStr());
	
	// extract root element and wrap it with an albaXMLElement object
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *root = doc->getDocumentElement();
	assert(root);

	// attach version attribute to the root node
	root->setAttribute(albaXMLString(ATTR_VERSION), albaXMLString(DB_VERSION));

	//STORE Elements to DB
	Store(doc, root);

	try {
		// do the serialization through DOMWriter::writeNode();
		theSerializer->writeNode(XMLTarget, *doc);
	}
	catch (...) {
			return ALBA_ERROR;
	}

	theSerializer->release();
	cppDEL(XMLTarget);
	doc->release();

	XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Terminate();

	albaLogMessage(wxString::Format("New DB has been written %s", m_DBFilename.GetCStr()));

	return ALBA_OK;
}

//----------------------------------------------------------------------------
int albaProsthesisDBManager::Load(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node)
{
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeList *dbChildren = node->getChildNodes();

	for (unsigned int i = 0; i < dbChildren->getLength(); i++)
	{
		//Reading Type nodes 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *currentNode = dbChildren->item(i);


		//PRODUCERS
		if (CheckNodeElement(currentNode, NODE_PRODUCERS))
		{
			XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeList *producersChildren = currentNode->getChildNodes();

			for (unsigned int i = 0; i < producersChildren->getLength(); i++)
			{
				//Reading manufacturer nodes
				XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *producerNode = producersChildren->item(i);

				if (!CheckNodeElement(producerNode, NODE_PRODUCER))
					continue;

				albaProDBProducer *newProducer = new albaProDBProducer();
				m_Producers.push_back(newProducer);
				if (newProducer->Load(producerNode) == ALBA_ERROR)
					return ALBA_ERROR;
			}
		}
		//TYPES
		else if (CheckNodeElement(currentNode, NODE_TYPES))
		{
			XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeList *typesChildren = currentNode->getChildNodes();

			for (unsigned int i = 0; i < typesChildren->getLength(); i++)
			{
				//Reading manufacturer nodes
				XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *typeNode = typesChildren->item(i);
	
				if (!CheckNodeElement(typeNode, NODE_TYPE))
					continue;
	
				albaProDBType *newType = new albaProDBType();
				m_Types.push_back(newType);
				if (newType->Load(typeNode) == ALBA_ERROR)
					return ALBA_ERROR;
			}
		}
		//PROSTHESES
		else if (CheckNodeElement(currentNode, NODE_PROSTHESES))
		{
			XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeList *prosthesesChildren = currentNode->getChildNodes();

			for (unsigned int i = 0; i < prosthesesChildren->getLength(); i++)
			{
				//Reading manufacturer nodes
				XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *prosthesesNode = prosthesesChildren->item(i);
				
				if (!CheckNodeElement(prosthesesNode, NODE_PROSTHESIS))
					continue;

				albaProDBProshesis *newProsthesis = new albaProDBProshesis();
				m_Prostheses.push_back(newProsthesis);
				if (newProsthesis->Load(prosthesesNode) == ALBA_ERROR)
					return ALBA_ERROR;
			}
		}
		else 
		{
			continue;
		}
	}
}

void albaProsthesisDBManager::Store(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *node)
{
	//Producers
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *producersNode = doc->createElement(albaXMLString(NODE_PRODUCERS));
	node->appendChild(producersNode);
	for (int i = 0; i < m_Producers.size(); i++)
		m_Producers[i]->Store(doc, producersNode);

	//Types
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *typesNode = doc->createElement(albaXMLString(NODE_TYPES));
	node->appendChild(typesNode);
	for (int i = 0; i < m_Producers.size(); i++)
		m_Types[i]->Store(doc, typesNode);

	//Prostheses
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *prosthesesNode = doc->createElement(albaXMLString(NODE_PROSTHESES));
	node->appendChild(prosthesesNode);
	for (int i = 0; i < m_Prostheses.size(); i++)
		m_Prostheses[i]->Store(doc, prosthesesNode);
}

//----------------------------------------------------------------------------
void albaProsthesisDBManager::Clear()
{
	for (int i = 0; i < m_Producers.size(); i++)
		m_Producers[i]->Clear();
	m_Producers.clear();

	for (int i = 0; i < m_Types.size(); i++)
		m_Types[i]->Clear();
	m_Types.clear();

	for (int i = 0; i < m_Prostheses.size(); i++)
		m_Prostheses[i]->Clear();
	m_Prostheses.clear();
}

//----------------------------------------------------------------------------
int albaProDBCompGruop::Load(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node)
{
	//<Compoents Name="Stem">
	m_Name = GetElementAttribute(node, ATTR_NAME);

	if (m_Name == "")
	{
		albaLogMessage("ERROR: Prosthesis name is empty");
		return ALBA_ERROR;
	}
	
	XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeList *components = node->getChildNodes();

	for (unsigned int i = 0; i < components->getLength(); i++)
	{
		//Reading Type nodes 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *componentNode = components->item(i);


		if (!CheckNodeElement(componentNode, NODE_COMPONENT))
			continue;

		albaProDBComponent *component = new albaProDBComponent();

		m_Components.push_back(component);
		if (component->Load(componentNode) == ALBA_ERROR)
			return ALBA_ERROR;
	}
	return ALBA_OK;
}

//----------------------------------------------------------------------------
void albaProDBCompGruop::Store(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *node)
{
	//Types
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * groupNode = doc->createElement(albaXMLString(NODE_COMPONENTS));
	groupNode->setAttribute(albaXMLString(ATTR_NAME), albaXMLString(m_Name));
	node->appendChild(groupNode);
	for (int i = 0; i < m_Components.size(); i++)
		m_Components[i]->Store(doc, groupNode);
}

//----------------------------------------------------------------------------
int albaProDBComponent::Load(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node)
{
	//<Component Name="S1" File="S1.cry">

	m_Name = GetElementAttribute(node, ATTR_NAME);
	m_Filename = GetElementAttribute(node, ATTR_FILE);


	if (m_Name == "")
	{
		albaLogMessage("ERROR: Component name is empty");
		return ALBA_ERROR;
	}
	if (m_Filename == "")
	{
		albaLogMessage("ERROR: Component filename is empty");
		return ALBA_ERROR;
	}

	XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeList *components = node->getChildNodes();

	for (unsigned int i = 0; i < components->getLength(); i++)
	{
		//Reading Type nodes 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *matrixNode = components->item(i);


		if (!CheckNodeElement(matrixNode, NODE_MATRIX))
			continue;

		double *el = *m_Matrix.GetElements();


		albaString mtrStr(matrixNode->getTextContent());
		
		int nRead=sscanf(mtrStr.GetCStr(), "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", el+0, el+1, el+2, el+3, el+4, el+5, el+6, el+7, el+8, el+9, el+10, el+11, el+12, el+13, el+14, el+15);

		if (nRead != 16)
		{
			albaLogMessage("ERROR wrong number of element inside matrix");
			return ALBA_ERROR;
		}


	}
	return ALBA_OK;
}

//----------------------------------------------------------------------------
void albaProDBComponent::Store(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *node)
{
	//Types
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * componentNode = doc->createElement(albaXMLString(NODE_COMPONENTS));

	componentNode->setAttribute(albaXMLString(ATTR_NAME), albaXMLString(m_Name));

	char tmpStr[1024];
	double *el = *m_Matrix.GetElements();
	sprintf(tmpStr, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", el[0], el[1], el[2], el[3], el[4], el[5], el[6], el[7], el[8], el[9], el[10], el[11], el[12], el[13], el[14], el[15]);
	componentNode->setTextContent(albaXMLString(tmpStr));

	node->appendChild(componentNode);
}

//----------------------------------------------------------------------------
int albaProDBType::Load(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node)
{
	//<Type Name="Acetabular"/>
	
	m_Name = GetElementAttribute(node, ATTR_NAME);

	if (m_Name == "")
	{
		albaLogMessage("ERROR: Type name is empty");
		return ALBA_ERROR;
	}
	return ALBA_OK;
}

//----------------------------------------------------------------------------
void albaProDBType::Store(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *node)
{
	//Types
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement * typeNode = doc->createElement(albaXMLString(NODE_TYPE));
	typeNode->setAttribute(albaXMLString(ATTR_NAME), albaXMLString(m_Name));
	node->appendChild(typeNode);
}

//----------------------------------------------------------------------------
int albaProDBProducer::Load(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node)
{
	//<Producer Name="Producer" Img="Producer.png" Site="http://www.prosthesisProducer.com/"/>

	m_Name = GetElementAttribute(node, ATTR_NAME);
	m_ImgFileName = GetElementAttribute(node, ATTR_IMG);
	m_WebSite = GetElementAttribute(node, ATTR_SITE);

	if (m_Name == "")
	{
		albaLogMessage("ERROR: Producer name is empty");
		return ALBA_ERROR;
	}
	return ALBA_OK;
}

//----------------------------------------------------------------------------
void albaProDBProducer::Store(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *node)
{
	//Producers
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *producerNode = doc->createElement(albaXMLString(NODE_PRODUCER));
	producerNode->setAttribute(albaXMLString(ATTR_NAME), albaXMLString(m_Name));
	producerNode->setAttribute(albaXMLString(ATTR_IMG), albaXMLString(m_ImgFileName));
	producerNode->setAttribute(albaXMLString(ATTR_SITE), albaXMLString(m_WebSite));
	node->appendChild(producerNode);
}

int albaProDBProshesis::Load(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node)
{
	//<Prosthesis Name="Example" Producer="Producer" Type="Acetabular" Side="Both" Img="Example.png">
	m_Name = GetElementAttribute(node, ATTR_NAME);
	m_ImgFileName = GetElementAttribute(node, ATTR_IMG);
	m_Side = GetSideByString(GetElementAttribute(node, ATTR_SIDE));
	m_Type = GetElementAttribute(node, ATTR_TYPE);
	m_Producer = GetElementAttribute(node, ATTR_PRODUCER);

	if (m_Name == "")
	{
		albaLogMessage("ERROR: Prosthesis name is empty");
		return ALBA_ERROR;
	}
	if (m_Side == PRO_UKNOWN)
	{
		albaLogMessage("ERROR: Prosthesis Side is unknown");
		return ALBA_ERROR;
	}

	XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeList *componentGroups = node->getChildNodes();

	for (unsigned int i = 0; i < componentGroups->getLength(); i++)
	{
		//Reading Type nodes 
		XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *componentGroupNode = componentGroups->item(i);


		if (!CheckNodeElement(componentGroupNode, NODE_COMPONENTS))
			continue;

		albaProDBCompGruop *componentGroup = new albaProDBCompGruop();

		m_CompGroups.push_back(componentGroup);
		if (componentGroup->Load(componentGroupNode) == ALBA_ERROR)
			return ALBA_ERROR;

	}
	return ALBA_OK;
}

//----------------------------------------------------------------------------
void albaProDBProshesis::Store(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc, XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *node)
{
	//Producers
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *prosthesisNode = doc->createElement(albaXMLString(NODE_PROSTHESES));
	prosthesisNode->setAttribute(albaXMLString(ATTR_NAME), albaXMLString(m_Name));
	prosthesisNode->setAttribute(albaXMLString(ATTR_IMG), albaXMLString(m_ImgFileName));
	prosthesisNode->setAttribute(albaXMLString(ATTR_TYPE), albaXMLString(m_Type));
	prosthesisNode->setAttribute(albaXMLString(ATTR_PRODUCER), albaXMLString(m_Producer));
	prosthesisNode->setAttribute(albaXMLString(ATTR_SIDE), albaXMLString(GetSideAsStr()));
	node->appendChild(prosthesisNode);
	for (int i = 0; i < m_CompGroups.size(); i++)
		m_CompGroups[i]->Store(doc, prosthesisNode);

}


//----------------------------------------------------------------------------
bool ProStorable::CheckNodeElement(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, const char *elementName)
{
	//Reading manufacturer nodes
	if (node->getNodeType() != XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
		return false;

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *nodeElement = (XERCES_CPP_NAMESPACE_QUALIFIER DOMElement*)node;
	albaString nameElement = "";
	nameElement = albaXMLString(nodeElement->getTagName());
	return (nameElement == elementName);
}

//----------------------------------------------------------------------------
albaString ProStorable::GetElementAttribute(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, const char *attributeName)
{
	if (node->getNodeType() != XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
		return "";
	return albaXMLString(((XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *)node)->getAttribute(albaXMLString(attributeName)));
}

//----------------------------------------------------------------------------
void albaProDBProducer::Clear()
{
	throw std::logic_error("The method or operation is not implemented.");
}

//----------------------------------------------------------------------------
void albaProDBType::Clear()
{
	throw std::logic_error("The method or operation is not implemented.");
}

//----------------------------------------------------------------------------
void albaProDBComponent::Clear()
{
	throw std::logic_error("The method or operation is not implemented.");
}

//----------------------------------------------------------------------------
void albaProDBCompGruop::Clear()
{
	throw std::logic_error("The method or operation is not implemented.");
}

//----------------------------------------------------------------------------
void albaProDBProshesis::Clear()
{
	throw std::logic_error("The method or operation is not implemented.");
}

//----------------------------------------------------------------------------
albaProDBProshesis::PRO_SIDES albaProDBProshesis::GetSideByString(albaString sideName)
{
	if (sideName == "Left")
		return PRO_LEFT;
	else if (sideName == "Right")
		return PRO_RIGHT;
	else if (sideName == "Both")
		return PRO_BOTH;
	else
		return PRO_UKNOWN;
}

//----------------------------------------------------------------------------
char * albaProDBProshesis::GetSideAsStr()
{
	switch (m_Side)
	{
		case albaProDBProshesis::PRO_LEFT:
			return "Left";
			break;
		case albaProDBProshesis::PRO_RIGHT:
			return "Right";
			break;
		case albaProDBProshesis::PRO_BOTH:
			return "Both";
			break;
		default:
			return "Error";
			break;
	}
}
