/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 *  Definition of vobsCATALOG_DENIS_JK class.
 * 
 * The DENIS_JK catalog ["J/A+A/413/1037/table1"] is used in secondary requests for BRIGHT scenarios 
 * to get johnson J and K magnitudes
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

/*
 * Local Headers 
 */
#include "vobsCATALOG_DENIS_JK.h"
#include "vobsPrivate.h"

/**
 * Class constructor
 */
vobsCATALOG_DENIS_JK::vobsCATALOG_DENIS_JK() : vobsREMOTE_CATALOG(vobsCATALOG_DENIS_JK_ID)
{
}

/**
 * Class destructor
 */
vobsCATALOG_DENIS_JK::~vobsCATALOG_DENIS_JK()
{
}


// Get the Julian date of source measurement (TIME_DATE) stored in the 'vobsSTAR_JD_DATE' property
// miscDynBufAppendString(&_query, "&-out=ObsJD");
// TODO: fix it in ProcessList()
// JD-2400000 	d 	Mean JD (= JD-2400000) of observation (time.epoch)

/*
 * Protected methods
 */

/**
 * Method to process the output star list from the catalog
 * 
 * @param list a vobsSTAR_LIST as the result of the search
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
//mcsCOMPL_STAT vobsCATALOG_DENIS::ProcessList(vobsSTAR_LIST &list)


/*___oOo___*/
