/*******************************************************************************
* JMMC project
*
* "@(#) $Id: evhEVENT_MAP.cpp,v 1.1 2004-12-05 19:00:25 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     27-Sep-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * evhKEY_MAP class definition.
 */

static char *rcsId="@(#) $Id: evhEVENT_MAP.cpp,v 1.1 2004-12-05 19:00:25 gzins Exp $"; 
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
#include "evhEVENT_MAP.h"
#include "evhPrivate.h"

/*
 * Class constructor
 */
evhEVENT_MAP::evhEVENT_MAP()
{
}

/*
 * Class destructor
 */
evhEVENT_MAP::~evhEVENT_MAP()
{
}

/*
 * Public methods
 */
mcsCOMPL_STAT evhEVENT_MAP::AddCallback(const evhCMD_KEY &key,
                                        evhCMD_CALLBACK &callback)
{
    logExtDbg("evhEVENT_MAP::AddCallback()");

    // For each registered event
    std::list<std::pair<evhKEY *, evhCALLBACK_LIST *>>::iterator iter;
    for (iter=_eventList.begin(); iter != _eventList.end(); ++iter)
    {
        // If event is the same than the specified one
        if (((*iter).first)->IsSame(key))
        {
            // Add the new callback to the list
            if (((*iter).second)->AddAtTail(&callback) == FAILURE)
            {
                return FAILURE;
            } 
            return SUCCESS;
        }
        // End if
    }
    // End for

    // If event is not yet registered, add it
    evhCMD_KEY   *newKey= new evhCMD_KEY(key);
    evhCALLBACK_LIST *newCbList = new evhCALLBACK_LIST();
    newCbList->AddAtTail(&callback);
    _eventList.push_back(pair<evhKEY *, evhCALLBACK_LIST *>(newKey, newCbList));

    return SUCCESS;
}

mcsCOMPL_STAT evhEVENT_MAP::AddCallback(const evhIOSTREAM_KEY &key,
                                        evhIOSTREAM_CALLBACK &callback)
{
    logExtDbg("evhEVENT_MAP::AddCallback()");

    // For each registered event
    std::list<pair<evhKEY *, evhCALLBACK_LIST *> > ::iterator iter;
    for (iter=_eventList.begin(); iter != _eventList.end(); ++iter)
    {
        // If event is the same than the specified one
        if (((*iter).first)->IsSame(key))
        {
            // Add the new callback to the list
            if (((*iter).second)->AddAtTail(&callback) == FAILURE)
            {
                return FAILURE;
            } 
            return SUCCESS;
        }
        // End if
    }
    // End for

    // If event is not yet registered, add it
    evhIOSTREAM_KEY   *newKey= new evhIOSTREAM_KEY(key);
    evhCALLBACK_LIST *newCbList = new evhCALLBACK_LIST();
    newCbList->AddAtTail(&callback);
    _eventList.push_back(pair<evhKEY *, evhCALLBACK_LIST *>(newKey, newCbList));

    return SUCCESS;
}

mcsCOMPL_STAT evhEVENT_MAP::Run(const evhCMD_KEY &key, msgMESSAGE &msg)
{
    logExtDbg("evhEVENT_MAP::Run()");

    // For each registered event
    std::list<pair<evhKEY *, evhCALLBACK_LIST *> > ::iterator iter;
    for (iter=_eventList.begin(); iter != _eventList.end(); ++iter)
    {
        // If event is the same than the specified one
        if (((*iter).first)->Match(key))
        {
            // Add the new callback to the list
            if (((*iter).second)->Run(msg) == FAILURE)
            {
                return FAILURE;
            } 
        }
        // End if
    }
    // End for
    return SUCCESS;
}

mcsCOMPL_STAT evhEVENT_MAP::Run(const evhIOSTREAM_KEY &key, int fd)
{
    logExtDbg("evhEVENT_MAP::Run()");

    // For each registered event
    std::list<pair<evhKEY *, evhCALLBACK_LIST *> > ::iterator iter;
    for (iter=_eventList.begin(); iter != _eventList.end(); ++iter)
    {
        // If event is the same than the specified one
        if (((*iter).first)->Match(key))
        {
            // Add the new callback to the list
            if (((*iter).second)->Run(fd) == FAILURE)
            {
                return FAILURE;
            } 
        }
        // End if
    }
    // End for
    return SUCCESS;
}

/*
 * Protected methods
 */

/*
 * Private methods
 */



/*___oOo___*/
