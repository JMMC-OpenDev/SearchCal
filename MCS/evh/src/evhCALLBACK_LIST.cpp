/*******************************************************************************
* JMMC project
*
* "@(#) $Id: evhCALLBACK_LIST.cpp,v 1.1 2004-12-05 19:00:25 gzins Exp $"
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

static char *rcsId="@(#) $Id: evhCALLBACK_LIST.cpp,v 1.1 2004-12-05 19:00:25 gzins Exp $"; 
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


mcsCOMPL_STAT evhCALLBACK_LIST::Run(const msgMESSAGE &msg)
{
    logExtDbg("evhCALLBACK_LIST::Run()");

    // Search callback in the list
    std::list<evhCALLBACK *>::iterator iter;
    for (iter=_callbackList.begin(); iter != _callbackList.end(); iter++)
    {
        logExtDbg("((*iter).Run(msg)()"); 
        if ((((evhCMD_CALLBACK *)(*iter))->Run(msg) & evhCB_FAILURE) != 0)
        {
            return FAILURE;
        }
    }

    return SUCCESS;
}

mcsCOMPL_STAT evhCALLBACK_LIST::Run(const int fd)
{
    logExtDbg("evhCALLBACK_LIST::Run()");

    // Search callback in the list
    std::list<evhCALLBACK *>::iterator iter;
    for (iter=_callbackList.begin(); iter != _callbackList.end(); iter++)
    {
        logExtDbg("((*iter).Run(msg)()"); 
        if ((((evhIOSTREAM_CALLBACK *)(*iter))->Run(fd) & evhCB_FAILURE) != 0)
        {
            return FAILURE;
        }
    }

    return SUCCESS;
}


/*___oOo___*/
