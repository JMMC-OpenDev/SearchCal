#ifndef msgMCS_ENV_H
#define msgMCS_ENV_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgMCS_ENV.h,v 1.1 2004-12-03 17:05:50 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  03-Dec-2004  Created
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


/*
 * Class declaration
 */

/**
 * Give back the host name and port number of the msgManager process associated
 * with the current MCS environment name available the environment variable
 * $MCSENV.
 * 
 */
class msgMCS_ENV
{

public:
    // Brief description of the constructor
    msgMCS_ENV();

    // Brief description of the destructor
    virtual ~msgMCS_ENV();

    virtual const char*      GetHostName(void);
    virtual const mcsUINT16  GetPortNumber(void);


protected:

    
private:
    mcsLOGICAL   _initialized; /* Flag that tell waither the host name and the
                                * port number have already been retrived or not
                                */
    mcsSTRING256 _hostName;    // the msgManager host name
    mcsUINT16    _portNumber;  // the msgManager port number

    mcsCOMPL_STAT RetrieveHostNameAndPortNumber(void);

    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    msgMCS_ENV(const msgMCS_ENV&);
    msgMCS_ENV& operator=(const msgMCS_ENV&);
};


#endif /*!msgMCS_ENV_H*/

/*___oOo___*/
