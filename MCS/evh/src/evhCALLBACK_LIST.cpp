/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: evhCALLBACK_LIST.cpp,v 1.8 2006-05-11 13:04:18 mella Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.7  2005/01/29 15:17:02  gzins
 * Added CVS log as modification history
 *
 * gzins     23-Sep-2004  Created
 * gzins     08-Dec-2004  Handled evhCB_DELETE callback return value
 *                        Added some method documentation
 * lafrasse  08-Dec-2004  Added Purge().
 * gzins     08-Dec-2004  Updated Purge().
 * gzins     07-Jan-2005  Changed SUCESS/FAILURE to mcsSUCCESS/mcsFAILURE
 *
 ******************************************************************************/

/**
 * \file
 * Definition of the evhCALLBACK_LIST class.
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: evhCALLBACK_LIST.cpp,v 1.8 2006-05-11 13:04:18 mella Exp $";

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
 * Always mcsSUCCESS.
 */
mcsCOMPL_STAT evhCALLBACK_LIST::Clear(void)
{
    logExtDbg("evhCALLBACK_LIST::Clear()"); 
    _callbackList.clear();
    return mcsSUCCESS;
}

/**
 * Adds the element at the end of the list
 *
 * \param callback element to be added to the list.
 * \return
 * Always mcsSUCCESS.
 */
mcsCOMPL_STAT evhCALLBACK_LIST::AddAtTail(evhCALLBACK *callback)
{
    logExtDbg("evhCALLBACK_LIST::AddAtTail()");

    // Put element in the list
    _callbackList.push_back(callback);

    return mcsSUCCESS;
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
 * \return Always mcsSUCCESS.
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
            return mcsSUCCESS;
        }
    }

    return mcsSUCCESS;
}

/**
 * Delete all detached callbacks.
 *
 * \return
 * Always mcsSUCCESS.
 */
mcsCOMPL_STAT evhCALLBACK_LIST::Purge(void)
{
    logExtDbg("evhCALLBACK_LIST::Purge()"); 

    // For each callback
    std::list<evhCALLBACK *>::iterator iter = _callbackList.begin();
    while (iter != _callbackList.end())
    {
        // If the current callback is detached
        if ((*iter)->IsDetached() == mcsTRUE)
        {
            // Remove it, and restart at the beginning of the list
            _callbackList.erase(iter);
            iter = _callbackList.begin();
        }
        else
        {
            iter++;
        }
    }

    return mcsSUCCESS;
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
 * delete it.
 *           
 * \return mcsSUCCESS or mcsFAILURE (see above).
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
            return mcsFAILURE;
        }
        if ((status & evhCB_DELETE) != 0)
        {
            if (((evhCMD_CALLBACK *)(*iter))->Detach() == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
    }
    // End for

    // Delete detached callbacks
    if (Purge() == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
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
 * delete it.
 *           
 * \return mcsSUCCESS or mcsFAILURE (see above).
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
            return mcsFAILURE;
        }
        if ((status & evhCB_DELETE) != 0)
        {
            if (((evhIOSTREAM_CALLBACK *)(*iter))->Detach() == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
    }
    // End for
    
    // Delete detached callbacks
    if (Purge() == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}


/*___oOo___*/
