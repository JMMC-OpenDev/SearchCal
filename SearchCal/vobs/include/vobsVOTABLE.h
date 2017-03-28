#ifndef vobsVOTABLE_H
#define vobsVOTABLE_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
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

/** current votable output format (version) */
static const mcsDOUBLE vobsVOTABLE_FORMAT = 2013.7;

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
    mcsCOMPL_STAT GetVotable(const vobsSTAR_LIST& starList,
                             const char* command,
                             const char* fileName,
                             const char* header,
                             const char* softwareVersion,
                             const char* request,
                             const char* xmlRequest,
                             const char *log,
                             mcsLOGICAL trimColumns,
                             miscoDYN_BUF* votBuffer);

    // Save the star list serialization (in VOTable v1.1 format) in file
    mcsCOMPL_STAT Save(vobsSTAR_LIST& starList,
                       const char* command,
                       const char *fileName,
                       const char *header,
                       const char *softwareVersion,
                       const char *request,
                       const char *xmlRequest,
                       const char *log,
                       mcsLOGICAL trimColumns);

protected:

private:

    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsVOTABLE(const vobsVOTABLE&);
    vobsVOTABLE& operator=(const vobsVOTABLE&) ;
} ;

#endif /*!vobsVOTABLE_H*/

/*___oOo___*/
