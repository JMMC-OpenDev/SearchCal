#ifndef msgMCS_ENV_H
#define msgMCS_ENV_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgMCS_ENV.h,v 1.2 2004-12-05 19:11:59 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  03-Dec-2004  Created
* gzins     05-Dec-2004  Changed method prototypes and class members
*
*
*******************************************************************************/

/**
 * \file
 * msgMCS_ENV class declaration.
 */


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include "misc.h"

/*
 * Class declaration
 */

/**
 * Class handling MCS environment.
 *
 * The msgMCS_ENV class contains methods to get informations (host name
 * and port number) of MCS environments. 
 * 
 * \usedfiles
 * \filename $MCSROOT/etc/mcsEnvList : file containing definition of MCS
 * environments.
 */
class msgMCS_ENV
{

public:
    // Brief description of the constructor
    msgMCS_ENV();

    // Brief description of the destructor
    virtual ~msgMCS_ENV();

    virtual const char*    GetHostName(char *envName=NULL);
    virtual const mcsINT32 GetPortNumber(char *envName=NULL);

protected:
    
private:
    mcsCOMPL_STAT LoadEnvListFile(void);
    miscDYN_BUF   _envList;           /* Internal buffer containing
                                         environment list */
    mcsLOGICAL    _envListFileLoaded; /* Flag that tell weither the file
                                         containing the environment list
                                         definition has already been loaded or
                                         not */

    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    msgMCS_ENV(const msgMCS_ENV&);
    msgMCS_ENV& operator=(const msgMCS_ENV&);
};


#endif /*!msgMCS_ENV_H*/

/*___oOo___*/
