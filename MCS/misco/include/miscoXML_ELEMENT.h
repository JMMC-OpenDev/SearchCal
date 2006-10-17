#ifndef miscoXML_ELEMENT_H
#define miscoXML_ELEMENT_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: miscoXML_ELEMENT.h,v 1.4 2006-10-17 11:27:19 mella Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2006/10/17 08:20:48  mella
 * Use standard ToXml() instead of previous ToString()
 *
 * Revision 1.2  2006/10/16 11:34:54  mella
 * First functionnal revision
 *
 * Revision 1.1  2006/10/16 07:34:19  mella
 * Class miscoXML_ELEMENT created
 *
 ******************************************************************************/

/**
 * @file
 * Declaration of miscoXML_ELEMENT class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * System Headers
 */
#include<list>
#include<map>


/*
 * MCS header
 */
#include "mcs.h"

/**
 * typedef of child elements
 */
//typedef std::list<XML_ELEMENTS*> miscoXML_ELEMENT_LIST;


/*
 * Class declaration
 */

/**
 * miscoXML_ELEMENT permits user to build one simple xml tree and get its string
 * representation.
 */
class miscoXML_ELEMENT
{

public:
    // Class constructor
    miscoXML_ELEMENT(string name);

    // Class destructor
    virtual ~miscoXML_ELEMENT();

    virtual mcsCOMPL_STAT AddElement(miscoXML_ELEMENT * element);
    virtual mcsCOMPL_STAT AddAttribute(string attributeName,
                                       string attributeValue);
    virtual mcsCOMPL_STAT AddAttribute(string attributeName,
                                       mcsDOUBLE attributeValue);
    virtual mcsCOMPL_STAT AddAttribute(string attributeName,
                                       mcsLOGICAL attributeValue);
    virtual mcsCOMPL_STAT AddContent(string content);
    virtual mcsCOMPL_STAT AddContent(mcsDOUBLE content);
    virtual mcsCOMPL_STAT AddContent(mcsLOGICAL content);
    virtual string ToXml();    

protected:
    // List of children elements
    std::list<miscoXML_ELEMENT*> _elements;
    // List of attributes
    std::map<string, string> _attributes;
    // Store element name
    string _name;
    // Store element content
    string _content;
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    miscoXML_ELEMENT(const miscoXML_ELEMENT&);
    miscoXML_ELEMENT& operator=(const miscoXML_ELEMENT&);
};

#endif /*!miscoXML_ELEMENT_H*/

/*___oOo___*/
