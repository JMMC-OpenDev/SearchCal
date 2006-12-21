/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrSCENARIO_BRIGHT_N.cpp,v 1.4 2006-12-21 15:16:05 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2006/03/03 15:25:23  scetre
 * Changed rcsId to rcsId __attribute__ ((unused))
 *
 * Revision 1.2  2005/11/16 14:28:02  scetre
 * Used criteria list as member of the class. All necessary criteria list are now built at the beginning of the Init() method
 *
 * Revision 1.1  2005/11/15 15:00:33  scetre
 * Added scenario K V and N
 *
 ******************************************************************************/

/**
 * \file
 *  Definition of sclsvrSCENARIO_BRIGHT_N class.
 */

static char *rcsId __attribute__ ((unused))="@(#) $Id: sclsvrSCENARIO_BRIGHT_N.cpp,v 1.4 2006-12-21 15:16:05 lafrasse Exp $"; 

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
sclsvrSCENARIO_BRIGHT_N::sclsvrSCENARIO_BRIGHT_N(sdbENTRY* progress):
    vobsSCENARIO(progress)
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

    if (AddEntry(vobsCATALOG_MIDI_ID, &_request, NULL, &_starListS, vobsCOPY)
        == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/*___oOo___*/
