#ifndef vobsPARSER_H
#define vobsPARSER_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsPARSER.h,v 1.16 2008-03-10 07:53:42 lafrasse Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.15  2006/02/14 13:00:02  lafrasse
* Changed receive time out from 4 minutes to 10
*
* Revision 1.14  2005/09/13 11:52:04  scetre
* Load xml file from memory after using msgSOCKET instead of loading xml file directly with gdome methods
*
* Revision 1.13  2005/02/13 15:22:11  gzins
* Removed ParseCDATA and GetPropertyId. Moved to vobsCDATA class
*
* Revision 1.12  2005/02/11 10:29:26  gzins
* Added GetPropertyId() method
*
* Revision 1.11  2005/02/10 10:46:33  gzins
* Changed column name to parameter name
*
* Revision 1.10  2005/02/08 20:35:24  gzins
* Changed some parameters from char* to const char*
*
* Revision 1.9  2005/02/04 09:56:32  gzins
* Declared ParseCData as public (for temporary MIDI catalog needs)
*
* Revision 1.8  2005/01/26 08:10:32  scetre
* change history
*
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

/**
 * Time out to get the CDS XML file
 * Vizier IP adress
 * Vizier address
 * Port
 */
#define vobsTIME_OUT 600000
#define vobsVIZIER_IP_ADDRESS "130.79.128.13" // IP address of the vizier.u-strasbg.fr
#define vobsQUERY_PORT 80

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
    mcsCOMPL_STAT Parse(const char *uri, 
                        const char *catalogName,
                        vobsSTAR_LIST &starList,
                        const char *logFileName=NULL);

protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsPARSER& operator=(const vobsPARSER&);
    vobsPARSER (const vobsPARSER&);
    
    // Recurvise parsing of XML document 
    mcsCOMPL_STAT ParseXmlSubTree(GdomeNode *node,
                                  vobsCDATA *cData);
};

#endif /*!vobsPARSER_H*/


/*___oOo___*/
