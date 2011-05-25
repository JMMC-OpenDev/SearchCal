/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Definition vobsCATALOG class .
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: vobsCATALOG.cpp,v 1.16 2006-03-03 15:03:27 scetre Exp $"; 

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
vobsCATALOG::vobsCATALOG(const char *name)
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
const char *vobsCATALOG::GetName(void)
{
    logTrace("vobsCATALOG::GetName()");
    
    return _name.c_str();
}

/*___oOo___*/
