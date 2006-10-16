/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: miscoXML_ELEMENT.cpp,v 1.1 2006-10-16 07:34:22 mella Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * @file
 *  Definition of miscoXML_ELEMENT class.
 */

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: miscoXML_ELEMENT.cpp,v 1.1 2006-10-16 07:34:22 mella Exp $"; 

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
    cout << name << endl;
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
mcsCOMPL_STAT miscoXML_ELEMENT::AddElement(miscoXML_ELEMENT * e)
{
    logTrace("miscoXML_ELEMENT::AddElement()");
    return mcsSUCCESS;
}


mcsCOMPL_STAT miscoXML_ELEMENT::AddAttribute(string attributeName,
                                             string attributeValue)
{
    logTrace("miscoXML_ELEMENT::AddAttributeElement()");
    return mcsSUCCESS;
}

string miscoXML_ELEMENT::ToString()
{
     logTrace("miscoXML_ELEMENT::ToString()");
    return string("<c>oucou</c>");
}

/*
 * Protected methods
 */


/*
 * Private methods
 */


/*___oOo___*/
