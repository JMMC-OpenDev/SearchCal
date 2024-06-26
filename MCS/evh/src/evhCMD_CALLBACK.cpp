/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of the evhCMD_CALLBACK class
 */


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
#include "evhCMD_CALLBACK.h"
#include "evhPrivate.h"
#include "evhErrors.h"

/**
 * Class constructor.
 */
evhCMD_CALLBACK::evhCMD_CALLBACK(fndOBJECT *object,
                                 const evhCMD_CB_METHOD method,
                                 void *userData) : evhCALLBACK(object, userData)
{
    _method   = method;
}

/**
 * Assignment operator.
 */
evhCMD_CALLBACK &evhCMD_CALLBACK::operator =(const evhCMD_CALLBACK &source)
{
    _method = source._method;
    return *this;
}

/**
 * Copy constructor.
 */
evhCMD_CALLBACK::evhCMD_CALLBACK(const evhCMD_CALLBACK &source) : evhCALLBACK(source)
{
    *this = source;
}

/**
 * Class destructor
 */
evhCMD_CALLBACK::~evhCMD_CALLBACK()
{
}

/*
 * Public methods
 */

/**
 * Set the method to be executed.
 *
 * \param method method to be executed.
 * \param userData user data pointer passed to the method
 *
 * \return reference to the object itself
 *
 */
evhCMD_CALLBACK &evhCMD_CALLBACK::SetMethod(const evhCMD_CB_METHOD method,
                                            void *userData)
{
    _method = method;

    SetUserData(userData);

    return *this;
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
mcsLOGICAL evhCMD_CALLBACK::IsSame(evhCALLBACK &callback)
{
    if (evhCALLBACK::IsSame(callback) == mcsTRUE)
    {
        if (_method == ((evhCMD_CALLBACK *) & callback)->_method)
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
 * arguments the message passed in the Run() method call and the void
 * pointer stored in the evhCMD_CALLBACK.
 *
 * If a callback returns with the evhCB_FAILURE bit set, the method send reply
 * to sender process (if it is not an internal message).
 *
 * \param msg message to be passed to the callback (see evhCMD_CALLBACK).
 */
evhCB_COMPL_STAT evhCMD_CALLBACK::Run(const msgMESSAGE &msg)
{
    evhCB_COMPL_STAT stat = evhCB_SUCCESS;

    /* If callback is detached */
    if (_detached == mcsTRUE)
    {
        return stat;
    }

    /* If object is a null pointer */
    if (_object == reinterpret_cast<void *> (NULL))
    {
        /* Return mcsFAILURE */
        errAdd(evhERR_NULL_OBJECT);
        return evhCB_FAILURE;
    }
        /* Else if method has not been set */
    else if (_method == (evhCMD_CB_METHOD) NULL)
    {
        /* Return mcsFAILURE */
        errAdd(evhERR_NULL_METHOD);
        return evhCB_FAILURE;
    }
        /* Else */
    else
    {
        /* Run callback */
        stat = (_object->*(_method))(msg, _userData);
    }
    /* End if*/

    /* If callback failed */
    if ((stat & evhCB_FAILURE) != 0)
    {
        /* Return mcsFAILURE */
        errAdd(evhERR_RUN_CB);

        // If it is not an internal message
        if (msg.IsInternal() == mcsFALSE)
        {
            msgMESSAGE newMsg;
            newMsg = msg;
            msgMANAGER_IF    msgManager;
            msgManager.SendReply(newMsg, mcsTRUE);
        }
        return stat;
    }
    /* End if */

    return (stat);
}

/*___oOo___*/
