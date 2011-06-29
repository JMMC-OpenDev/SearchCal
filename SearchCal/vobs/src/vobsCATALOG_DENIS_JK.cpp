/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 *  Definition of vobsCATALOG_DENIS_JK class.
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
vobsCATALOG_DENIS_JK::vobsCATALOG_DENIS_JK() 
    : vobsREMOTE_CATALOG("J/A+A/413/1037")
{
}

/**
 * Class destructor
 */
vobsCATALOG_DENIS_JK::~vobsCATALOG_DENIS_JK()
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
 *
 * @return always mcsSUCCESS 
 *
 */
mcsCOMPL_STAT vobsCATALOG_DENIS_JK::WriteQuerySpecificPart(void)
{
    logTrace("vobsCATALOG_DENIS_JK::GetAskingSpecificParameters()");
   
    // properties to retreive
    miscDynBufAppendString(&_query, "&-out=Jmag");
    miscDynBufAppendString(&_query, "&-out=Ksmag");
    miscDynBufAppendString(&_query, "&-out=Var");
    // constraints
    miscDynBufAppendString(&_query, "&Var=%3C4");
            
    return mcsSUCCESS;
}


/*
 * Private methods
 */


/*___oOo___*/
