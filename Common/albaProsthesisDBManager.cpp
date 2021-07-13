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



//----------------------------------------------------------------------------
albaProsthesisDBManager::albaProsthesisDBManager()
//----------------------------------------------------------------------------
{
	m_DBFilename = (albaGetAppDataDirectory()).c_str();
	m_DBFilename += "\\ProsthesisDB\\";

	if(!wxDir::Exists(m_DBFilename.GetCStr()))
		wxMkdir(m_DBFilename.GetCStr());


	Load(NULL);
}

//----------------------------------------------------------------------------
albaProsthesisDBManager::~albaProsthesisDBManager()
{

}

void albaProsthesisDBManager::Load(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node)
{
}

void albaProsthesisDBManager::Store()
{
}

void ablaProDBCompGruop::Load(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node)
{
}

void ablaProDBCompGruop::Store()
{
}

void albaProDBComponent::Load(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node)
{
}

void albaProDBComponent::Store()
{
}

void albaProDBType::Load(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node)
{
}

void albaProDBType::Store()
{
}

void albaProDBProducer::Load(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node)
{
}

void albaProDBProducer::Store()
{
}

void albaProDBProshesis::Load(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node)
{
}

void albaProDBProshesis::Store()
{
}
