#ifndef vobsPARSER_H
#define vobsPARSER_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsPARSER.h,v 1.8 2005-01-26 08:10:32 scetre Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* scetre    06-Jul-2004  Created
*
*
*******************************************************************************/

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * system header files
 */
#include <vector>
#include <gdome.h>

/*
 * header files
 */
#include "vobsSTAR_LIST.h"
#include "vobsCDATA.h"

/*
 * Class declaration
 */
/**
 * vobsPARSER allow to get a xml file from an URL in the CDS and to parse it
 * in order to extract the data present in it.
 */
class vobsPARSER
{
public:
    vobsPARSER();
    virtual ~vobsPARSER();
    
    // Parse of the XML document from a URI
    mcsCOMPL_STAT Parse(char *uri, 
                        vobsSTAR_LIST &starList,
                        char* logFileName);
    
protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsPARSER& operator=(const vobsPARSER&);
    vobsPARSER (const vobsPARSER&);
    
    // Recurvise parsing of XML document 
    mcsCOMPL_STAT ParseXmlSubTree(GdomeNode *node,
                                  vobsCDATA *cData);

    // Parsing of the CDATA section
    mcsCOMPL_STAT ParseCData(vobsCDATA *cData,
                             vobsSTAR_LIST &starList);
};

#endif /*!vobsPARSER_H*/


/*___oOo___*/
