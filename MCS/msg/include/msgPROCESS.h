#ifndef msgPROCESS_H
#define msgPROCESS_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgPROCESS.h,v 1.1 2004-12-07 07:39:08 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     06-Dec-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * msgPROCESS class declaration.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * Local header files
 */
#include "msgSOCKET_CLIENT.h"

/*
 * Class declaration
 */

/**
 * Class handling process connected to the MCS message service.
 * 
 * The msgPROCESS class provides methods to set/get informations of a
 * connected process to the MCS message service. 
 */
class msgPROCESS : public msgSOCKET_CLIENT
{

public:
    // Class constructor
    msgPROCESS();

    // Class destructor
    virtual ~msgPROCESS();

    virtual mcsCOMPL_STAT SetName(char *name);
    virtual const char   *GetName() const;
protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    msgPROCESS(const msgPROCESS &process);
    msgPROCESS& operator=(const msgPROCESS&);

    // Name of the process; i.e. MCS registering name
    mcsPROCNAME _name;
};

#endif /*!msgPROCESS_H*/

/*___oOo___*/
