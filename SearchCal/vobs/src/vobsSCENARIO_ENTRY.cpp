/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsSCENARIO_ENTRY.cpp,v 1.12 2006-03-03 15:03:28 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.11  2006/02/21 16:32:00  scetre
 * Updated documentation
 *
 * Revision 1.10  2006/01/18 08:46:49  scetre
 * Managed the new option which could be gave to a catalog
 *
 * Revision 1.9  2005/11/29 10:36:09  gzins
 * Changed vobsBASE_FILTER to vobsFILTER
 *
 * Revision 1.8  2005/11/24 08:13:50  scetre
 * Changed mother class of filter from vobsFILTER to vobsFILTER
 *
 * Revision 1.7  2005/11/16 10:47:55  scetre
 * Updated documentation
 *
 * Revision 1.6  2005/11/16 10:47:54  scetre
 * Updated documentation
 *
 * Revision 1.5  2005/11/15 14:57:56  scetre
 * Added new scenario structure
 * Added possibility to query merand and borde as primary catalog
 *
 * Revision 1.4  2005/02/09 06:10:44  gzins
 * Changed vobsSTAR_COMP_CRITERIA_LIST& to vobsSTAR_COMP_CRITERIA_LIST* in vobsSCENARIO
 *
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
 * @file
 *  Definition of vobsSCENARIO_ENTRY class.
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: vobsSCENARIO_ENTRY.cpp,v 1.12 2006-03-03 15:03:28 scetre Exp $"; 

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
vobsSCENARIO_ENTRY::vobsSCENARIO_ENTRY(mcsSTRING32                 catalogName,
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
