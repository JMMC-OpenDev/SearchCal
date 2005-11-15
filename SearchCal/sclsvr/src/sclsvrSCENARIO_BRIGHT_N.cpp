/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrSCENARIO_BRIGHT_N.cpp,v 1.1 2005-11-15 15:00:33 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * \file
 *  Definition of sclsvrSCENARIO_BRIGHT_N class.
 */

static char *rcsId="@(#) $Id: sclsvrSCENARIO_BRIGHT_N.cpp,v 1.1 2005-11-15 15:00:33 scetre Exp $"; 
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
#include "sclsvrSCENARIO_BRIGHT_N.h"
#include "sclsvrPrivate.h"

/**
 * Class constructor
 */
sclsvrSCENARIO_BRIGHT_N::sclsvrSCENARIO_BRIGHT_N()
{
}

/**
 * Class destructor
 */
sclsvrSCENARIO_BRIGHT_N::~sclsvrSCENARIO_BRIGHT_N()
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
mcsCOMPL_STAT sclsvrSCENARIO_BRIGHT_N::Init(vobsREQUEST * request)
{
    logTrace("sclsvrSCENARIO_BRIGHT_N::Init()");

    // Clear the scenario
    Clear();
    _request.Copy(*request);

    // Clear the list input and list output which will be used
    _starListP.Clear();
    _starListS.Clear();

    // Create a criteria list
    vobsSTAR_COMP_CRITERIA_LIST criteriaList;

    if (AddEntry(vobsCATALOG_MIDI_ID, &_request, NULL, &_starListS, vobsCOPY)
        == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/*___oOo___*/
