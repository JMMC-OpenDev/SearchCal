/*******************************************************************************
* JMMC project
*
* "@(#) $Id: evhCALLBACK_LIST.cpp,v 1.2 2004-12-08 13:32:15 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     23-Sep-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * Definition of the evhCALLBACK_LIST class.
 */

static char *rcsId="@(#) $Id: evhCALLBACK_LIST.cpp,v 1.2 2004-12-08 13:32:15 gzins Exp $"; 
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
#include "evhCALLBACK_LIST.h"
#include "evhCMD_CALLBACK.h"
#include "evhIOSTREAM_CALLBACK.h"
#include "evhPrivate.h"

/*
 * Class constructor
 */
evhCALLBACK_LIST::evhCALLBACK_LIST()
{
}

/*
 * Class destructor
 */
evhCALLBACK_LIST::~evhCALLBACK_LIST()
{
    _callbackList.clear();
}

/*
 * Public methods
 */

/**
 * Check whether the list is empty or not.  
 *
 * \return
 * True value (i.e. mcsTRUE) if the number of elements is zero, false (i.e.
 * mcsFLASE) otherwise.
 */
mcsLOGICAL evhCALLBACK_LIST::IsEmpty(void)
{
    logExtDbg("evhCALLBACK_LIST::IsEmpty()");
    if (_callbackList.empty() == true)
    {
        return mcsTRUE;
    }
    else
    {
        return mcsFALSE;
    }
}

/**
 * Erase all elements from the list.
 *
 * \return
 * Always SUCCESS.
 */
mcsCOMPL_STAT evhCALLBACK_LIST::Clear(void)
{
    logExtDbg("evhCALLBACK_LIST::Clear()"); 
    _callbackList.clear();
    return SUCCESS;
}

/**
 * Adds the element at the end of the list
 *
 * \param callback element to be added to the list.
 * \return
 * Always SUCCESS.
 */
mcsCOMPL_STAT evhCALLBACK_LIST::AddAtTail(evhCALLBACK *callback)
{
    logExtDbg("evhCALLBACK_LIST::AddAtTail()");

    // Put element in the list
    _callbackList.push_back(callback);

    return SUCCESS;
}

/**
 * Remove the element from the list
 *
 * This method looks for the specified \em callback in the list. If found, it
 * remove it, and do nothing otherwise.
 *
 * The method evhCALLBACK::IsSame() is used to compare element of the list with
 * the specified one.
 *
 * \warning if list contains more than one instance, only first occurence is
 * removed. 
 *
 * \param callback element to be removed from the list.
 *
 * \return Always SUCCESS.
 */
mcsCOMPL_STAT evhCALLBACK_LIST::Remove(evhCALLBACK *callback)
{
    logExtDbg("evhCALLBACK_LIST::Remove()");

    // Search callback in the list
    std::list<evhCALLBACK *>::iterator iter;
    for (iter=_callbackList.begin(); iter != _callbackList.end(); iter++)
    {
        // If found
        if ((*iter)->IsSame(*callback) == mcsTRUE)
        {
            // Remove it
            _callbackList.erase(iter);
            return SUCCESS;
        }
    }

    return SUCCESS;
}

/**
 * Returns the number of elements (callbacks) currently stored in the list.
 * \return
 * The numbers of callbacks in the list.
 */
mcsUINT32 evhCALLBACK_LIST::Size(void)
{
    return _callbackList.size();
}


/**
 * Executes all the callbacks of the list.
 *
 * The received messsage is passed as argument to the callback, with the user
 * data pointer which has been given when callback has been added.
 *
 * If a callback returns with the evhCB_FAILURE bit set, the method returns
 * immediately, i.e. the remaining callbacks in the list are not executed.
 *
 * If a callback returns with the evhCB_DELETE bit set, the method just
 * detaches the callback (i.e. it is not removed from the list). The method
 * Clean() to remove the detached event from the list.
 *           
 * \return SUCCESS or FAILURE (see above).
 */
mcsCOMPL_STAT evhCALLBACK_LIST::Run(const msgMESSAGE &msg)
{
    logExtDbg("evhCALLBACK_LIST::Run()");

    // For each callback in the list
    std::list<evhCALLBACK *>::iterator iter;
    for (iter=_callbackList.begin(); iter != _callbackList.end(); iter++)
    {
        // Run callback
        evhCB_COMPL_STAT status;
        status = ((evhCMD_CALLBACK *)(*iter))->Run(msg);
        if ((status & evhCB_FAILURE) != 0)
        {
            return FAILURE;
        }
        if ((status & evhCB_DELETE) != 0)
        {
            return ((evhCMD_CALLBACK *)(*iter))->Detach();
        }
    }
    // End for

    return SUCCESS;
}

/**
 * Executes all the callbacks of the list.
 *
 * The received I/O descriptor is passed as argument to the callback, with the
 * user data pointer which has been given when callback has been added.
 *
 * If a callback returns with the evhCB_FAILURE bit set, the method returns
 * immediately, i.e. the remaining callbacks in the list are not executed.
 *
 * If a callback returns with the evhCB_DELETE bit set, the method just
 * detaches the callback (i.e. it is not removed from the list). The method
 * Clean() to remove the detached event from the list.
 *           
 * \return SUCCESS or FAILURE (see above).
 */
mcsCOMPL_STAT evhCALLBACK_LIST::Run(const int fd)
{
    logExtDbg("evhCALLBACK_LIST::Run()");

    // For each callback in the list
    std::list<evhCALLBACK *>::iterator iter;
    for (iter=_callbackList.begin(); iter != _callbackList.end(); iter++)
    {
        // Run callback
        evhCB_COMPL_STAT status;
        status = ((evhIOSTREAM_CALLBACK *)(*iter))->Run(fd);
        if ((status & evhCB_FAILURE) != 0)
        {
            return FAILURE;
        }
        if ((status & evhCB_DELETE) != 0)
        {
            return ((evhIOSTREAM_CALLBACK *)(*iter))->Detach();
        }
    }
    // End for

    return SUCCESS;
}


/*___oOo___*/
