/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Definition of fndMVC_MODEL class.
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: fndMVC_MODEL.cpp,v 1.3 2006-05-11 13:04:33 mella Exp $";
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
#include "fndMVC_MODEL.h"
#include "fndPrivate.h"

/**
 * Class constructor
 */
fndMVC_MODEL::fndMVC_MODEL()
{
}

/**
 * Class destructor
 */
fndMVC_MODEL::~fndMVC_MODEL()
{
}

/*
 * Public methods
 */
/**
 * Add a view in the list of views associated to this model
 *
 * @param view the view to add in the list
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT fndMVC_MODEL::AddView(fndMVC_VIEW *view)
{
    logTrace("fndMVC_MODEL::AddView()");

    _viewList.push_back(view);
    
    return mcsSUCCESS;
}

/**
 * Delete a view from the list of views associated to this model
 *
 * @param view the view to delete from the list
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT fndMVC_MODEL::DeleteView(fndMVC_VIEW *view)
{
    logTrace("fndMVC_MODEL::DeleteView()");

    // Create an iterator of a fndViewList in order to be able to move in the
    // list
    fndViewList::iterator viewListIterator;
    // Put this iterator at the beginning of the list
    viewListIterator = _viewList.begin();
    mcsLOGICAL isFound = mcsFALSE;
    
    // Check if the view to remove is in the list of view
    while ((viewListIterator != _viewList.end()) || (isFound != mcsTRUE))
    {
        // If the view is equal to one of the list
        if ((*viewListIterator) == view)
        {
            // remove the view of the list
            _viewList.erase(viewListIterator);
            // Changed isFound as true
            isFound = mcsTRUE;
        }
        else
        {
            viewListIterator ++;
        }
    }
    
    return mcsSUCCESS;
}

/**
 * Delete all views from the list of views associated to this model
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT fndMVC_MODEL::DeleteViews()
{
    logTrace("fndMVC_MODEL::DeleteViews()");

    // erase all element between the beginning and the end of the list
    _viewList.erase(_viewList.begin(), _viewList.end());
        
    return mcsSUCCESS;
}

/**
 * Notify all views associated to this model
 * 
 * If this object has changed, then notify all of its associated views; i.e each
 * view has its Update method called.
 * 
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT fndMVC_MODEL::NotifyViews()
{
    logTrace("fndMVC_MODEL::NotifyViews()");

    // Create an iterator of a fndViewList in order to be able to move in the
    // list
    fndViewList::iterator viewListIterator;
    // Put this iterator at the beginning of the list
    viewListIterator = _viewList.begin();

    // For each view of the list of views, Update them
    while (viewListIterator != _viewList.end())
    {
        if ((*viewListIterator)->Update() == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        viewListIterator ++;
    }

    return mcsSUCCESS;
}

/**
 * Get the number of views associated to this model
 *
 * @return number of views associated to this model
 */
mcsINT32 fndMVC_MODEL::GetNbViews()
{
    logTrace("fndMVC_MODEL::GetNbViews()");
    
    return _viewList.size();
}

/*___oOo___*/
