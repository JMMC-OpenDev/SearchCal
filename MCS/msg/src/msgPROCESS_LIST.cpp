/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgPROCESS_LIST.cpp,v 1.4 2005-01-07 18:36:38 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     06-Dec-2004  Created
* gzins     08-Dec-2004  Added descriptor argument to GetProcess()
* gzins     14-Dec-2004  Minor documentation changes 
* gzins     07-Jan-2005  Changed SUCCESS/FAILURE to mcsSUCCESS/mcsFAILURE 
*
*******************************************************************************/

/**
 * \file
 * msgPROCESS_LIST class definition.
 */

static char *rcsId="@(#) $Id: msgPROCESS_LIST.cpp,v 1.4 2005-01-07 18:36:38 gzins Exp $"; 
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
#include "msgPROCESS_LIST.h"
#include "msgPrivate.h"

/**
 * Class constructor
 */
msgPROCESS_LIST::msgPROCESS_LIST()
{
}

/**
 * Class destructor
 */
msgPROCESS_LIST::~msgPROCESS_LIST()
{
}

/*
 * Public methods
 */
/**
 * Check whether the list is empty or not.  
 *
 * \return true value (i.e. mcsTRUE) if the number of elements is zero, false
 * (i.e.  mcsFALSE) otherwise.
 */
mcsLOGICAL msgPROCESS_LIST::IsEmpty(void)
{
    logExtDbg("msgPROCESS_LIST::IsEmpty()");
    if (_processList.empty() == true)
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
 * \return always mcsSUCCESS.
 */
mcsCOMPL_STAT msgPROCESS_LIST::Clear(void)
{
    // Delete all objects in list 
    std::list<msgPROCESS *>::iterator iter;
    for (iter=_processList.begin(); iter != _processList.end(); iter++)
    {
        delete (*iter);
    }

    // Clear list
    _processList.clear();
    return mcsSUCCESS;
}

/**
 * Adds the element at the end of the list
 *
 * \param process element to be added to the list.
 *
 * \return always mcsSUCCESS.
 */
mcsCOMPL_STAT msgPROCESS_LIST::AddAtTail(msgPROCESS *process)
{
    logExtDbg("msgPROCESS_LIST::AddAtTail()");

    // Put element in the list
    _processList.push_back(process);

    return mcsSUCCESS;
}

/**
 * Remove the element from the list
 *
 * This method looks for the process corresponding to the specified socket
 * descriptor \em sd in the list. If found, it removes it, and does nothing
 * otherwise.
 *
 * \warning if list contains more than one instance corresponding to \em sd,
 * only first occurence is removed. 
 *
 * \param sd socket descriptor of the process to be removed from the list.
 *
 * \return always mcsSUCCESS.
 */
mcsCOMPL_STAT msgPROCESS_LIST::Remove(mcsINT32 sd)
{
    logExtDbg("msgPROCESS_LIST::Remove()");

    // Search process in the list
    std::list<msgPROCESS *>::iterator iter;
    for (iter=_processList.begin(); iter != _processList.end(); iter++)
    {
        // If found
        if ((*iter)->GetDescriptor() == sd)
        {
            // Delete element
            delete (*iter);
            // Clear element from list
            _processList.erase(iter);
            return mcsSUCCESS;
        }
    }

    return mcsSUCCESS;
}

/**
 * Returns the number of elements (processes) currently stored in the list.
 *
 * \return numbers of processes in the list.
 */
mcsUINT32 msgPROCESS_LIST::Size(void) 
{
    return _processList.size();
}

/**
 * Returns the next element (process) in the list.
 *
 * This method returns the pointer to the next element of the list. If \em
 * init is mcsTRUE, it returns the first element of the list.
 * 
 * This method can be used to move forward in the list, as shown below:
 * \code
 *     for (unsigned int el = 0; el < processList.Size(); el++)
 *     {
 *         processList.GetNextProcess((el==0))->Close();
 *     }
 * \endcode
 *
 * \param init if true return the element of the list, otherwise return the
 * next one.
 *
 * \return pointer to the next element of the list or NULL if the end of the
 * list is reached.
 */
msgPROCESS *msgPROCESS_LIST::GetNextProcess(mcsLOGICAL init) 
{
    logExtDbg("msgPROCESS_LIST::GetNextProcess()");
    
    // If init is TRUE
    if (init == mcsTRUE)
    {
        // Get First element of the list
        _processIterator = _processList.begin();
    }
    // Else
    else
    {
        // Get next element
        _processIterator++;

        // If end of list is reached
        if (_processIterator == _processList.end())
        {
            // Return NULL
            return NULL;
        }
        // End if
    }
    // End if

    // Return current element
    return (*_processIterator);
}

/**
 * Returns the process of the list, which is registered with the given name.
 *
 * This method looks, in the list, for the process which is registered with
 * the specified name and which is connected to the socket \em sd. If \em sd
 * is not specified, the the process with the given name found is returned.
 * If found, it returns the pointer to this element, and NULL otherwise.
 *  
 * This method can be used to know whether a process is in list or not, as shown
 * below:
 * \code
 *     if (processList.GetProcess(process.GetName()) == NULL)
 *     {
 *         printf ("Process not found in list !!");
 *     }
 * \endcode
 * 
 * \param name process name.
 * \param sd socket descriptor to which process is connected.
 * 
 * \return pointer to the found element of the list or NULL if element is not
 * found in list.
 */
msgPROCESS *msgPROCESS_LIST::GetProcess(char *name, mcsINT32 sd)
{
    logExtDbg("msgPROCESS_LIST::GetProcess()");

    // Search process in the list
    std::list<msgPROCESS *>::iterator iter;
    for (iter=_processList.begin(); iter != _processList.end(); iter++)
    {
        // If found
        if ((strcmp((*iter)->GetName(), name) == 0) &&
            ((sd == -1) || ((*iter)->GetDescriptor() == sd)))
        {
            // Return pointer to the element 
            return (*iter);
        }
    }

    // If not found return NULL pointer
    return NULL;
}

/*___oOo___*/
