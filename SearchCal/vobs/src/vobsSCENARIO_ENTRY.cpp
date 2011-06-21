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
vobsSCENARIO_ENTRY::vobsSCENARIO_ENTRY(const mcsSTRING32         catalogName,
                                       vobsREQUEST                 *request,
                                       vobsSTAR_LIST               *listInput,
                                       vobsSTAR_LIST               *listOutput,
                                       vobsACTION                  action,
                                       vobsSTAR_COMP_CRITERIA_LIST *criteriaList,
                                       vobsFILTER                  *filter)
{
    _queryOption = "";

    strcpy(_catalogName, catalogName);
    _request      = request;
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
    _filter = filter;
}

/**
 * Copy Constructor
 */
vobsSCENARIO_ENTRY::vobsSCENARIO_ENTRY(const vobsSCENARIO_ENTRY &scenarioEntry)
{
    _queryOption = scenarioEntry._queryOption;

    strcpy(_catalogName, scenarioEntry._catalogName);
    _request      = scenarioEntry._request;
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
    _filter       = scenarioEntry._filter;
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

/**
 * Set query option
 *
 * @param queryOption query option
 *
 * return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsSCENARIO_ENTRY::SetQueryOption(string queryOption)
{
    logTrace("vobsSCENARIO_ENTRY::SetQueryOption()");

    _queryOption = queryOption;
    
    return mcsSUCCESS;
}

/**
 * Get query option
 *
 * @return the query option
 */
string vobsSCENARIO_ENTRY::GetQueryOption(void)
{
    logTrace("vobsSCENARIO_ENTRY::GetQueryOption()");

    return _queryOption;
}

/*
 * Protected methods
 */


/*
 * Private methods
 */


/*___oOo___*/
