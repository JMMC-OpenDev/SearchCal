/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsSCENARIO_ENTRY.cpp,v 1.2 2005-01-27 15:56:04 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/01/27 13:42:54  scetre
 * vobsSCENARIO_ENTRY created. It move from vobsSCENARIO
 *
 ******************************************************************************/

/**
 * \file
 *  Definition of vobsSCENARIO_ENTRY class.
 */

static char *rcsId="@(#) $Id: vobsSCENARIO_ENTRY.cpp,v 1.2 2005-01-27 15:56:04 scetre Exp $"; 
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
vobsSCENARIO_ENTRY::vobsSCENARIO_ENTRY()
{
}

/**
 * Copy Constructor
 */
vobsSCENARIO_ENTRY::vobsSCENARIO_ENTRY(const vobsSCENARIO_ENTRY &scenarioEntry)
{
    _catalog=scenarioEntry._catalog;
    _listInput=scenarioEntry._listInput;
    _listOutput=scenarioEntry._listOutput;
    _action=scenarioEntry._action;
    _criteriaList=scenarioEntry._criteriaList;
}
/**
 * Copy Constructor
 */
vobsSCENARIO_ENTRY::vobsSCENARIO_ENTRY(vobsCATALOG                 *catalog,
                                       vobsSTAR_LIST               *listInput,
                                       vobsSTAR_LIST               *listOutput,
                                       vobsACTION                  action,
                                       vobsSTAR_COMP_CRITERIA_LIST criteriaList)
{
    _catalog=catalog;
    _listInput=listInput;
    _listOutput=listOutput;
    _action=action;
    _criteriaList=criteriaList;
}
/**
 * Class destructor
 */
vobsSCENARIO_ENTRY::~vobsSCENARIO_ENTRY()
{
}

/*
 * Protected methods
 */


/*
 * Private methods
 */


/*___oOo___*/
