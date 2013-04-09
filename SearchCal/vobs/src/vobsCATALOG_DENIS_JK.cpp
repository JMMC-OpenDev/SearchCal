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

// LBO: REMOVE CLASS ASAP

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


/*___oOo___*/
