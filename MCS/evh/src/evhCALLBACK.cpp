/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of the evhCALLBACK class
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: evhCALLBACK.cpp,v 1.6 2006-05-11 13:04:18 mella Exp $";

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
#include "evhCALLBACK.h"
#include "evhPrivate.h"
#include "evhErrors.h"

/**
 * Class constructor.
 */
evhCALLBACK::evhCALLBACK(fndOBJECT *object, void *userData) 
{
    _object   = object;
    _userData = userData;
    _detached = mcsFALSE; 
}

/**
 * Assignment operator.
 */
evhCALLBACK &evhCALLBACK::operator =(const evhCALLBACK &source)
{
    _object   = source._object;
    _userData = source._userData;
    _detached = source._detached;
    return *this;
}

/**
 * Copy constructor.
 */
evhCALLBACK::evhCALLBACK(const evhCALLBACK &source) : fndOBJECT(source)
{
    *this = source;
}

/**
 * Class destructor
 */
evhCALLBACK::~evhCALLBACK()
{
}

/*
 * Public methods
 */
/**
 * Set the user data pointer to be passed to method
 * 
 * \param userData user data pointer passed to the method
 *
 * \return reference to the object itself
 *
 */
evhCALLBACK &evhCALLBACK::SetUserData(void *userData)
{
    logExtDbg("evhCALLBACK::SetUserData()");

    _userData = userData;

    return *this;
}

/**
 * Detach the callback.
 *
 * When a callback is detached, the Run() method has no longer effect.
 * 
 * \return always mcsSUCCESS.
 */
mcsCOMPL_STAT evhCALLBACK::Detach()
{
    logExtDbg("evhCALLBACK::Detach()");

    _detached = mcsTRUE;

    return mcsSUCCESS;
}

/**
 * Check whether the callback is detached or not.  
 *
 * \return mcsTRUE is callback is detached, and mcsFALSE otherwise.
 */
mcsLOGICAL evhCALLBACK::IsDetached()
{
    logExtDbg("evhCALLBACK::IsDetached()");

    return _detached;
}

/**
 * Test if this callback is the same than another one.
 * 
 * The test is only performed on the object and method members; i.e. userData
 * pointer value is not tested.
 *
 * \param callback the other callback to be compared with.
 *
 * \return TRUE if it is the same callback. Otherwise FALSE is returned.
 */
mcsLOGICAL evhCALLBACK::IsSame(evhCALLBACK &callback)
{
    logExtDbg("evhCALLBACK::IsSame()");
    
    if (_object == callback._object)
    {
        return mcsFALSE;
    }
    return mcsTRUE;
}

/*___oOo___*/
