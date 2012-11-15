/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsCATALOG_HIP2 class definition.
 * 
 * The HIP2 catalog ["I/311/hip2"] is used in secondary requests for BRIGHT scenarios 
 * to get precise parallax, proper motion and their errors
 */


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
#include "misc.h"

/*
 * Local Headers 
 */
#include "vobsCATALOG_HIP2.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Class constructor
 */
vobsCATALOG_HIP2::vobsCATALOG_HIP2() : vobsREMOTE_CATALOG(vobsCATALOG_HIP2_ID, 1.0, 1991.25, 1991.25, mcsTRUE, mcsFALSE,
                                                          GetOverwriteProperties())
{
}

/*
 * Class destructor
 */
vobsCATALOG_HIP2::~vobsCATALOG_HIP2()
{
}


/*
 * Private methods
 */

/**
 * Build the specific part of the asking.
 *
 * Build the specific part of the asking. This is the part of the asking
 * which is write specificaly for each catalog.
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsCATALOG_HIP2::WriteQuerySpecificPart(void)
{
    // SECONDARY REQUEST: cone search arround given star coordinates for BRIGHT scenarios

    // Get the HIP identifier (ID_MAIN) stored in the 'vobsSTAR_ID_HIP' property
    miscDynBufAppendString(&_query, "&-out=HIP");

    // Get the error e_RArad (ERROR) stored in the 'vobsSTAR_POS_EQ_RA_ERROR' property
    miscDynBufAppendString(&_query, "&-out=e_RArad");

    // Get the error e_DErad (ERROR) stored in the 'vobsSTAR_POS_EQ_DEC_ERROR' property
    miscDynBufAppendString(&_query, "&-out=e_DErad");
    
    // Get the proper motion pmRA (POS_EQ_PMRA) stored in the 'vobsSTAR_POS_EQ_PMRA' property
    miscDynBufAppendString(&_query, "&-out=pmRA");

    // Get the proper motion pmDE (POS_EQ_PMDEC) stored in the 'vobsSTAR_POS_EQ_PMDEC' property
    miscDynBufAppendString(&_query, "&-out=pmDE");

    // Get the error e_pmRA (ERROR) stored in the 'vobsSTAR_POS_EQ_PMRA_ERROR' property
    miscDynBufAppendString(&_query, "&-out=e_pmRA");

    // Get the error e_pmDE (ERROR) stored in the 'vobsSTAR_POS_EQ_PMDEC_ERROR' property
    miscDynBufAppendString(&_query, "&-out=e_pmDE");

    // Get the parallax Plx (POS_PARLX_TRIG) stored in the 'vobsSTAR_POS_PARLX_TRIG' property
    miscDynBufAppendString(&_query, "&-out=Plx");

    // Get the parallax error e_Plx (ERROR) stored in the 'vobsSTAR_POS_PARLX_TRIG_ERROR' property
    miscDynBufAppendString(&_query, "&-out=e_Plx");

    return mcsSUCCESS;
}


/*___oOo___*/
