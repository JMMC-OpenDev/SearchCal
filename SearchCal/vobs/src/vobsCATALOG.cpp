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
/**
 * Export standard iostream objects (cin, cout,...).
 */

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
vobsCATALOG::vobsCATALOG(const char* name)
{
    // Set name
    _name = name;
}


/*
 * Class destructor
 */

/**
 * Delete a catalog object. 
 */
vobsCATALOG::~vobsCATALOG()
{
}


/*
 * Public methods
 */

/**
 * Get a catalog name in a string format
 *
 * @return catalog name or NULL if not set.
 */
const char* vobsCATALOG::GetName(void)
{
    return _name;
}

/*___oOo___*/
