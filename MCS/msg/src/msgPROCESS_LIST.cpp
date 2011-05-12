/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Storage class used to hold all the msgPROCESS objects of the \<msgManager\>.
 *
 * \sa msgPROCESS_LIST
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: msgPROCESS_LIST.cpp,v 1.9 2006-05-11 13:04:56 mella Exp $";
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
 * Return whether the list is empty or not.  
 *
 * \return mcsTRUE if the list is empty, mcsFALSE otherwise
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
 * Return the number of processes currently stored in the list.
 *
 * \return the number of processes in the list
 */
mcsUINT32 msgPROCESS_LIST::Size(void) 
{
    return _processList.size();
}

/**
 * Add a process at the end of the list.
 *
 * \param process the msgPROCESS object to be added to the list
 *
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT msgPROCESS_LIST::AddAtTail(msgPROCESS *process)
{
    logExtDbg("msgPROCESS_LIST::AddAtTail()");

    // Put the process at the end of the list
    _processList.push_back(process);

    return mcsSUCCESS;
}

/**
 * Returns the next process in the list.
 *
 * This method returns a pointer on the next element of the list. If \em init
 * equals mcsTRUE, returns a pointer on the first element of the list.
 * 
 * This method can be used to move forward in the list, as shown below:
 * \code
 *     for (unsigned int el = 0; el < processList.Size(); el++)
 *     {
 *         processList.GetNextProcess((mcsLOGICAL)(el==0))->Close();
 *     }
 * \endcode
 *
 * \param init if equals to mcsTRUE, return the first element of the list,
 * otherwise return the one next to the current internal position
 *
 * \return a pointer on the next element of the list, or NULL if the end of the
 * list is reached
 */
msgPROCESS *msgPROCESS_LIST::GetNextProcess(const mcsLOGICAL init) 
{
    logExtDbg("msgPROCESS_LIST::GetNextProcess()");
    
    // If init is TRUE
    if (init == mcsTRUE)
    {
        // Get First element of the list
        _processIterator = _processList.begin();
    }
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
    }

    // Return current element
    return (*_processIterator);
}

/**
 * Return the process of the list which has been registered with the given name.
 *
 * This method looks in the list for the process which has been registered with
 * the given name, and which is connected to the \em sd socket. If \em sd is not
 * specified, the first process with the given name is returned.
 *  
 * This method can be used to figure out whether a process is in the list or
 * not, as shown below:
 * \code
 *     if (processList.GetProcess(process.GetName()) == NULL)
 *     {
 *         printf ("Process not found in list !!");
 *     }
 * \endcode
 * 
 * \param name the searched process name
 * \param sd optionnal socket descriptor to which the searched process should be
 * connected
 * 
 * \return a pointer on the searched process, or NULL if the process was not
 * found in list.
 */
msgPROCESS *msgPROCESS_LIST::GetProcess(const char *name, const mcsINT32 sd)
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

/**
 * Remove a process from the list
 *
 * This method looks for the process corresponding to the specified socket
 * descriptor \em sd in the list. If found, removes it, otherwise does nothing.
 *
 * \warning if the list contains more than one instance corresponding to \em sd,
 * only the first occurence is removed. 
 *
 * \param sd socket descriptor of the process to be removed from the list
 *
 * \return always mcsSUCCESS
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
 * Erase all elements of the list.
 *
 * \return always mcsSUCCESS
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

/*___oOo___*/
