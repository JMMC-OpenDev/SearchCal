/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Definition of vobsSCENARIO_ENTRY class.
 */

/* 
 * System Headers 
 */
#include <iostream>
#include <string.h>
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
vobsSCENARIO_ENTRY::vobsSCENARIO_ENTRY(const char* catalogName,
                                       vobsREQUEST *request,
                                       vobsSTAR_LIST *listInput,
                                       vobsSTAR_LIST *listOutput,
                                       vobsACTION action,
                                       vobsSTAR_COMP_CRITERIA_LIST *criteriaList,
                                       vobsFILTER *filter)
{
    _catalogName = catalogName;
    _queryOption = NULL;
    _request = request;
    _listInput = listInput;
    _listOutput = listOutput;
    _action = action;

    _criteriaList = criteriaList;
    _filter = filter;
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
