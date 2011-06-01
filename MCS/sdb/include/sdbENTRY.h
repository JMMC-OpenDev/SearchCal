#ifndef sdbENTRY_H
#define sdbENTRY_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Declaration of sdbENTRY class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * MCS header
 */
#include "mcs.h"
#include "thrd.h"


/*
 * Class declaration
 */

/**
 * Status database class.
 *
 * This class provides protected method to read and write a database entry; i.e.
 * includes mutual-exclusion mechanism for readind and writing operations.
 */
class sdbENTRY
{
public:
    // Class constructor
    sdbENTRY();

    // Class destructor
    virtual ~sdbENTRY();

    mcsCOMPL_STAT  Write   (const char*       message);
    mcsCOMPL_STAT  Read    (      char*       message,
                                  mcsLOGICAL  waitNewMessage = mcsFALSE,
                                  mcsINT32    timeoutInMs = -1);
protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    sdbENTRY(const sdbENTRY&);
    sdbENTRY& operator=(const sdbENTRY&);

    thrdMUTEX      _mutex;
    mcsSTRING256   _buffer;
    mcsLOGICAL     _isNewMessage;
};

#endif /*!sdbENTRY_H*/

/*___oOo___*/
