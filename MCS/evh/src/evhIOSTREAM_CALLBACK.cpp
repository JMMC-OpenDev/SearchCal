/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: evhIOSTREAM_CALLBACK.cpp,v 1.3 2005-01-29 15:17:02 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * gzins     22-Sep-2004  Created
 * gzins     17-Nov-2004  Fixed bug in assignment operator method
 * gzins     07-Jan-2005  Changed SUCESS/FAILURE to mcsSUCCESS/mcsFAILURE
 *
 ******************************************************************************/

/**
 * \file
 * Definition of the evhIOSTREAM_CALLBACK class
 */

static char *rcsId="@(#) $Id: evhIOSTREAM_CALLBACK.cpp,v 1.3 2005-01-29 15:17:02 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


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
#include "evhIOSTREAM_CALLBACK.h"
#include "evhPrivate.h"
#include "evhErrors.h"

/**
 * Class constructor.
 */
evhIOSTREAM_CALLBACK::evhIOSTREAM_CALLBACK(fndOBJECT *object,
                         const evhIOSTREAM_CB_METHOD method,
                         void *userData): evhCALLBACK(object, userData) 
{
    _method   = method;
}

/**
 * Assignment operator.
 */
evhIOSTREAM_CALLBACK &evhIOSTREAM_CALLBACK::operator =(const evhIOSTREAM_CALLBACK &source)
{
    _method   = source._method;
    return *this;
}

/**
 * Copy constructor.
 */
evhIOSTREAM_CALLBACK::evhIOSTREAM_CALLBACK(const evhIOSTREAM_CALLBACK &source) : evhCALLBACK(source)
{
    *this = source;
}

/**
 * Class destructor
 */
evhIOSTREAM_CALLBACK::~evhIOSTREAM_CALLBACK()
{
}

/*
 * Public methods
 */
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
mcsLOGICAL evhIOSTREAM_CALLBACK::IsSame(evhCALLBACK &callback)
{
    logExtDbg("evhIOSTREAM_CALLBACK::IsSame()");
    
    if (evhCALLBACK::IsSame(callback) == mcsTRUE)
    {
        if (_method == ((evhIOSTREAM_CALLBACK *)&callback)->_method)
        {
            return mcsTRUE;
        }
    }
    return mcsFALSE;
}

/**
 * Run the callback.
 *
 * Whenever this method is called (like in the main loop of an event handler,
 * when the corresponding event occurs), the 'method' is executed, having as
 * arguments the file descriptor in the Run() method call and the void
 * pointer stored in the evhIOSTREAM_CALLBACK.
 * 
 * \param fd file descriptor to be passed to the callback (see
 * evhIOSTREAM_CALLBACK).
 */
evhCB_COMPL_STAT evhIOSTREAM_CALLBACK::Run(const int fd)
{
    evhCB_COMPL_STAT stat = evhCB_SUCCESS;

    /* If callback is detcahed */
    if (_detached == mcsTRUE)
    {
        return stat;
    }

    /* If object is a null pointer */
    if(_object == reinterpret_cast<void *>(NULL))
    {
        /* Return mcsFAILURE */
        errAdd(evhERR_NULL_OBJECT);
        return evhCB_FAILURE;
    }
    /* Else if method has not been set */
    else if (_method == (evhIOSTREAM_CB_METHOD)NULL) 
    {
        /* Return mcsFAILURE */
        errAdd(evhERR_NULL_METHOD);
        return evhCB_FAILURE;
    }
    /* Else */
    else
    {
        /* Run callback */
        stat = (_object->*(_method))(fd, _userData);
    }
    /* End if*/

    /* If callback failed */
    if((stat & evhCB_FAILURE) != 0)
    {
        /* Return mcsFAILURE */
        errAdd(evhERR_RUN_CB);
        return stat;
    }
    /* End if */

    return(stat);
}

/*___oOo___*/
