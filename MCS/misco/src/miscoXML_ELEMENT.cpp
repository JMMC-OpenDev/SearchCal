/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: miscoXML_ELEMENT.cpp,v 1.3 2006-10-16 11:41:07 swmgr Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2006/10/16 11:34:57  mella
 * First functionnal revision
 *
 * Revision 1.1  2006/10/16 07:34:22  mella
 * Class miscoXML_ELEMENT created
 *
 ******************************************************************************/

/**
 * @file
 *  Definition of miscoXML_ELEMENT class.
 */

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: miscoXML_ELEMENT.cpp,v 1.3 2006-10-16 11:41:07 swmgr Exp $"; 

/* 
 * System Headers 
 */
#include <iostream>
using namespace std;

/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"

/*
 * Local Headers 
 */
#include "miscoXML_ELEMENT.h"
#include "miscoPrivate.h"

/**
 * Class constructor
 */
miscoXML_ELEMENT::miscoXML_ELEMENT(string name)
{
    _name=name;
}


/**
 * Class destructor
 */
miscoXML_ELEMENT::~miscoXML_ELEMENT()
{
}

/*
 * Public methods
 */

/**
 * Add the given element as child 
 * @param element new child to add
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT miscoXML_ELEMENT::AddElement(miscoXML_ELEMENT * element)
{
    logTrace("miscoXML_ELEMENT::AddElement()");
    _elements.push_back(element);
    return mcsSUCCESS;
}


/**
 * Create one new attribute. If one attribute already exist, its content will be
 * replaced.
 * @param attributeName the attribute name
 * @param attributeValue the attribute value
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT miscoXML_ELEMENT::AddAttribute(string attributeName,
                                             string attributeValue)
{
    logTrace("miscoXML_ELEMENT::AddAttributeElement()");
    _attributes.erase(attributeName);
    _attributes.insert(make_pair(attributeName, attributeValue));
    return mcsSUCCESS;
}


/**
 * Append the given string to the element's content. 
 * @param content new contetn to append.
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT miscoXML_ELEMENT::AddContent(string content)
{
    logTrace("miscoXML_ELEMENT::AddContent()");
    _content.append(content);
    return mcsSUCCESS;
}

/**
 * Return the xml stringified  representation of the element.
 * @return the xml representation.
 */
string miscoXML_ELEMENT::ToString()
{
    logTrace("miscoXML_ELEMENT::ToString()");

    string xmlStr ;
   
    // Append starting markup
    xmlStr.append("<");
    xmlStr.append(_name);  

    // Append attributes
    std::map<string, string>::iterator i = _attributes.begin();
    while( i != _attributes.end() )
    {
        // append only if value is not empty
        if(! i->second.empty() )
        {
            xmlStr.append(" ");
            xmlStr.append(i->first);
            xmlStr.append("=\"");
            xmlStr.append(i->second);
            xmlStr.append("\"");
        }
        i++;
    }
    xmlStr.append(">");
  
    // Append children elements content
    std::list<miscoXML_ELEMENT*>::iterator j = _elements.begin();
    while(j != _elements.end())
    {
        xmlStr.append((*j)->ToString());
        j++;
    }
    
    // Append content
    xmlStr.append(_content);
    
    // Append closing markup
    xmlStr.append("</");
    xmlStr.append(_name);    
    xmlStr.append(">");
    
    return xmlStr;
}

/*
 * Protected methods
 */


/*
 * Private methods
 */


/*___oOo___*/
