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
 * Return the string representation of the given action
 * @param action action to convert as string
 * @return string representation
 */
const char* vobsGetAction(vobsACTION action)
{
    switch (action)
    {
        case vobsCLEAR_MERGE:
            return "CLEAR_MERGE";
        case vobsMERGE:
            return "MERGE";
        case vobsUPDATE_ONLY:
            return "UPDATE_ONLY";
        default:
            return "";
    }
}

/**
 * Class constructor
 */
vobsSCENARIO_ENTRY::vobsSCENARIO_ENTRY(vobsORIGIN_INDEX catalogId,
                                       vobsREQUEST *request,
                                       vobsSTAR_LIST *listInput,
                                       vobsSTAR_LIST *listOutput,
                                       vobsACTION action,
                                       vobsSTAR_COMP_CRITERIA_LIST *criteriaList,
                                       vobsFILTER *filter)
{
    _catalogId = catalogId;
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
