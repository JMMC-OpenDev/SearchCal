/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsSCENARIO_ENTRY.cpp,v 1.4 2005-02-09 06:10:44 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2005/02/07 17:36:53  scetre
 * added printf to resolve bug problem in multiple query
 *
 * Revision 1.2  2005/01/27 15:56:04  scetre
 * scenario became a friend class of scenario entry to be able to access private members
 *
 * Revision 1.1  2005/01/27 13:42:54  scetre
 * vobsSCENARIO_ENTRY created. It move from vobsSCENARIO
 *
 ******************************************************************************/

/**
 * \file
 *  Definition of vobsSCENARIO_ENTRY class.
 */

static char *rcsId="@(#) $Id: vobsSCENARIO_ENTRY.cpp,v 1.4 2005-02-09 06:10:44 gzins Exp $"; 
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
#include "vobsSCENARIO_ENTRY.h"
#include "vobsPrivate.h"

/**
 * Class constructor
 */
vobsSCENARIO_ENTRY::vobsSCENARIO_ENTRY(vobsCATALOG                 *catalog,
                                       vobsSTAR_LIST               *listInput,
                                       vobsSTAR_LIST               *listOutput,
                                       vobsACTION                  action,
                                       vobsSTAR_COMP_CRITERIA_LIST *criteriaList)
{
    _catalog      = catalog;
    _listInput    = listInput;
    _listOutput   = listOutput;
    _action       = action;
    if (criteriaList != NULL)
    {
        _criteriaList = new vobsSTAR_COMP_CRITERIA_LIST(*criteriaList);
    }
    else
    {
        _criteriaList = NULL;
    }
}

/**
 * Copy Constructor
 */
vobsSCENARIO_ENTRY::vobsSCENARIO_ENTRY(const vobsSCENARIO_ENTRY &scenarioEntry)
{
    _catalog      = scenarioEntry._catalog;
    _listInput    = scenarioEntry._listInput;
    _listOutput   = scenarioEntry._listOutput;
    _action       = scenarioEntry._action;
    if (scenarioEntry._criteriaList != NULL)
    {
        _criteriaList = 
            new vobsSTAR_COMP_CRITERIA_LIST(*scenarioEntry._criteriaList);
    }
    else
    {
        _criteriaList = NULL;
    }
}


/**
 * Class destructor
 */
vobsSCENARIO_ENTRY::~vobsSCENARIO_ENTRY()
{
    _listInput=NULL;
    _listOutput=NULL;
    if (_criteriaList != NULL)
    {
        delete (_criteriaList);
        _criteriaList = NULL;
    }
}

/*
 * Protected methods
 */


/*
 * Private methods
 */


/*___oOo___*/
