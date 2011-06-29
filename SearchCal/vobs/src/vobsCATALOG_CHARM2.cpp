/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 *  Definition of vobsCATALOG_CHARM2 class.
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
#include "vobsCATALOG_CHARM2.h"
#include "vobsPrivate.h"

/**
 * Class constructor
 */
vobsCATALOG_CHARM2::vobsCATALOG_CHARM2() :
                                    vobsREMOTE_CATALOG(vobsCATALOG_CHARM2_ID)
{
}

/**
 * Class destructor
 */
vobsCATALOG_CHARM2::~vobsCATALOG_CHARM2()
{
    miscDynBufDestroy(&_query);    
}

/*
 * Public methods
 */


/*
 * Protected methods
 */
/**
 * Build the specificatic part of the asking.
 *
 * Build the specificatic part of the asking. This is the part of the asking
 * which is write specificaly for each catalog.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 *
 */
mcsCOMPL_STAT vobsCATALOG_CHARM2::WriteQuerySpecificPart(void)
{
    logTrace("vobsCATALOG_CHARM2::GetAskingSpecificParameters()");
   
    // properties to retreive
    miscDynBufAppendString(&_query, "&-out=LD");
    miscDynBufAppendString(&_query, "&-out=e_LD");
    miscDynBufAppendString(&_query, "&-out=UD");
    miscDynBufAppendString(&_query, "&-out=e_UD");
    miscDynBufAppendString(&_query, "&-out=Method");
    miscDynBufAppendString(&_query, "&-out=Lambda");
    miscDynBufAppendString(&_query, "&-out=Kmag");
    miscDynBufAppendString(&_query, "&-out=Vmag");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_B");
    // constraint        
    miscDynBufAppendString(&_query, "&Method=,LBI,LO,SPE");
    
    return mcsSUCCESS;
}


/*
 * Private methods
 */


/*___oOo___*/
