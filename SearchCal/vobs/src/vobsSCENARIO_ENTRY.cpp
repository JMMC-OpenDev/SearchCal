/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsSCENARIO_ENTRY.cpp,v 1.1 2005-01-27 13:42:54 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * \file
 *  Definition of vobsSCENARIO_ENTRY class.
 */

static char *rcsId="@(#) $Id: vobsSCENARIO_ENTRY.cpp,v 1.1 2005-01-27 13:42:54 scetre Exp $"; 
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
    catalog=scenarioEntry.catalog;
    listInput=scenarioEntry.listInput;
    listOutput=scenarioEntry.listOutput;
    action=scenarioEntry.action;
    criteriaList=scenarioEntry.criteriaList;
}

/**
 * Class destructor
 */
vobsSCENARIO_ENTRY::~vobsSCENARIO_ENTRY()
{
}

/*
 * Public methods
 */


/*
 * Protected methods
 */


/*
 * Private methods
 */


/*___oOo___*/
