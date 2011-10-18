/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 *  Definition of vobsCATALOG_UNSO class.
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
#include "vobsCATALOG_UNSO.h"
#include "vobsPrivate.h"

/**
 * Class constructor
 */
vobsCATALOG_UNSO::vobsCATALOG_UNSO()  : vobsREMOTE_CATALOG(vobsCATALOG_UNSO_ID)
{
}

/**
 * Class destructor
 */
vobsCATALOG_UNSO::~vobsCATALOG_UNSO()
{
}

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
mcsCOMPL_STAT vobsCATALOG_UNSO::WriteQuerySpecificPart(void)
{
    // pmRA(POS_EQ_PMRA) / pmDE(POS_EQ_PMDEC) columns
    miscDynBufAppendString(&_query, "&-out=pmRA");
    miscDynBufAppendString(&_query, "&-out=pmDE");
    
    // Imag column (PHOT_PHG_I) : TODO decide which column to use
    //miscDynBufAppendString(&_query, "&-out=Imag");
    
    // get all PHOT_PHG_I columns
    miscDynBufAppendString(&_query, "&-out=*PHOT_PHG_I");
    
    // R1mag / R2mag columns (PHOT_PHG_R)
    miscDynBufAppendString(&_query, "&-out=R1mag");
    miscDynBufAppendString(&_query, "&-out=R2mag");
    // B1mag / B2mag columns (PHOT_PHG_B)
    miscDynBufAppendString(&_query, "&-out=B1mag");
    miscDynBufAppendString(&_query, "&-out=B2mag");
    
    // order by distance
    miscDynBufAppendString(&_query, "&-sort=_r");
    
    return mcsSUCCESS;
}


/*___oOo___*/
