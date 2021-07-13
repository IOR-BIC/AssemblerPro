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

#ifndef __albaProsthesisDBManager_H__
#define __albaProsthesisDBManager_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/dom/DOM.hpp>
#include <set>
#include "albaMatrix.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class albaWizardWaitOp;
class albaGUISettingsDialog;

class ProStorable
{
public:
	virtual void Load(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node) = 0;
	virtual void Store() = 0;
};

/**
Class Name: ProProducer.
*/
class albaProDBProducer : public ProStorable
{

public:
	albaString GetName() const { return m_Name; }
	void SetName(albaString val) { m_Name = val; }
	albaString GetImgFileName() const { return m_ImgFileName; }
	void SetImgFileName(albaString val) { m_ImgFileName = val; }
	albaString GetWebSite() const { return m_WebSite; }
	void SetWebSite(albaString val) { m_WebSite = val; }
	
	// Inherited via ProStorable
	virtual void Load(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node) override;
	virtual void Store() override;

protected:
	albaString m_Name;
	albaString m_ImgFileName;
	albaString m_WebSite;
};

/**
Class Name: ProType.
*/
class albaProDBType : public ProStorable
{
public:
	albaString GetName() const { return m_Name; }
	void SetName(albaString val) { m_Name = val; }

	// Inherited via ProStorable
	virtual void Load(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node) override;
	virtual void Store() override;
protected:
	albaString m_Name;
};

class albaProDBComponent : public ProStorable
{
public:
	albaMatrix GetMatrix() const { return m_Matrix; }
	void SetMatrix(albaMatrix val) { m_Matrix = val; }
	albaString GetName() const { return m_Name; }
	void SetName(albaString val) { m_Name = val; }
	albaString GetFilename() const { return m_Filename; }
	void SetFilename(albaString val) { m_Filename = val; }

	// Inherited via ProStorable
	virtual void Load(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node) override;
	virtual void Store() override; 

protected:
	albaMatrix m_Matrix;
	albaString m_Name;
	albaString m_Filename;
};

/**
Class Name: albaProsthesisDBManager.
*/
class ablaProDBCompGruop : public ProStorable
{
public:
	albaString GetName() const { return m_Name; }
	void SetName(albaString val) { m_Name = val; }

	// Inherited via ProStorable
	virtual void Load(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node) override;
	virtual void Store() override;

protected:
	albaString m_Name;
	std::vector<albaProDBComponent> m_Components;
};


class albaProDBProshesis: public ProStorable
{
public: 

	enum PRO_SIDES
	{
		PRO_LEFT,
		PRO_RIGHT,
		PRO_BOTH
	};

	albaString GetName() const { return m_Name; }
	void SetName(albaString val) { m_Name = val; }
	albaString GetImgFileName() const { return m_ImgFileName; }
	void SetImgFileName(albaString val) { m_ImgFileName = val; }
	albaString GetType() const { return m_Type; }
	void SetType(albaString val) { m_Type = val; }
	albaString GetProducer() const { return m_Producer; }
	void SetProducer(albaString val) { m_Producer = val; }
	albaProDBProshesis::PRO_SIDES GetSide() const { return m_Side; }
	void SetSide(albaProDBProshesis::PRO_SIDES val) { m_Side = val; }

	// Inherited via ProStorable
	virtual void Load(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node) override;
	virtual void Store() override;

protected:
	albaString m_Name;
	albaString m_ImgFileName;
	albaString m_Producer;
	albaString m_Type;
	PRO_SIDES m_Side;

	std::vector<ablaProDBCompGruop> m_Groups;
};

/**
  Class Name: albaProsthesisDBManager.
*/
class ALBA_EXPORT albaProsthesisDBManager : public ProStorable
{
public:

  /** Default constructor */
  albaProsthesisDBManager();

  /** Default destructor */
  ~albaProsthesisDBManager();

	// Inherited via ProStorable
	virtual void Load(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node) override;
	virtual void Store() override;
			
private:
	std::vector<albaProDBProducer> m_Producers;
	albaString m_DBFilename;
};
#endif
