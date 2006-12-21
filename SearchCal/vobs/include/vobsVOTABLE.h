#ifndef vobsVOTABLE_H
#define vobsVOTABLE_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsVOTABLE.h,v 1.5 2006-12-21 15:08:15 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2006/10/09 15:07:08  lafrasse
 * Added request XML serialization in VOTables.
 *
 * Revision 1.3  2006/02/24 15:01:22  lafrasse
 * Updates Doxygen tags
 *
 * Revision 1.2  2005/12/07 15:28:20  lafrasse
 * Updated VOTable generation to include information about software version, request and date
 *
 * Revision 1.1  2005/11/30 15:24:37  lafrasse
 * Exported VOTable generation code from vobsSTAR_LIST to vobsVOTABLE
 *
 ******************************************************************************/

/**
 * @file
 * Declaration of vobsVOTABLE class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * MCS header
 */
#include "mcs.h"

/*
 * Local Headers 
 */
#include "vobsSTAR_LIST.h"

/*
 * Class declaration
 */

/**
 * Serialize a vobsSTAR_LIST object in a VOTable 1.1 XML file.
 *
 * @sa vobsSTAR_LIST
 * @sa http://www.ivoa.net/Documents/latest/VOT.html
 */
class vobsVOTABLE
{

public:
    // Class constructor
    vobsVOTABLE();

    // Class destructor
    virtual ~vobsVOTABLE();

    // Serialize a star list in a VOTable v1.1 XML file
    mcsCOMPL_STAT GetVotable(vobsSTAR_LIST& starList,
                             const char*    fileName,
                             const char*    header,
                             const char*    softwareVersion,
                             const char*    request,
                             const char*    xmlRequest,
                             miscoDYN_BUF*  buffer);

    // Save the star list serialization (in VOTable v1.1 format) in file
    mcsCOMPL_STAT Save(vobsSTAR_LIST& starList,
                       const char *fileName,
                       const char *header,
                       const char *softwareVersion,
                       const char *request,
                       const char *xmlRequest);

protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsVOTABLE(const vobsVOTABLE&);
    vobsVOTABLE& operator=(const vobsVOTABLE&);
};

#endif /*!vobsVOTABLE_H*/

/*___oOo___*/
