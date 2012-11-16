/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Definition vobsCATALOG class .
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
#include "vobsCATALOG.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"
#include "vobsSTAR.h"

/*
 * Class constructor
 */

/**
 * Build a catalog object.
 */
vobsCATALOG::vobsCATALOG(const char* name,
                         const mcsDOUBLE precision,
                         const mcsDOUBLE epochFrom,
                         const mcsDOUBLE epochTo,
                         const mcsLOGICAL hasProperMotion,
                         const mcsLOGICAL multipleRows,
                         const vobsSTAR_PROPERTY_MASK* overwritePropertyMask)
{
    // Define meta data:
    _meta = new vobsCATALOG_META(name, precision, epochFrom, epochTo, hasProperMotion, multipleRows, overwritePropertyMask);
}


/*
 * Class destructor
 */

/**
 * Delete a catalog object. 
 */
vobsCATALOG::~vobsCATALOG()
{
    delete(_meta);
}


/*
 * Public methods
 */

/*___oOo___*/
