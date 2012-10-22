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
                         const mcsDOUBLE posError,
                         const mcsDOUBLE epochFrom,
                         const mcsDOUBLE epochTo,
                         const vobsSTAR_PROPERTY_ID_LIST* overwritePropertyIDList)
{
    // Set name
    _name = name;
    // Initialize option
    _option = NULL;

    // position error of the catalog
    _posError = posError;

    // Epoch (from) of the catalog
    _epochFrom = epochFrom;
    // Epoch (to) of the catalog
    _epochTo = epochTo;

    if (epochTo == epochFrom)
    {
        _singleEpoch = mcsTRUE;
        _epochMed = _epochFrom;
    }
    else
    {
        _singleEpoch = mcsFALSE;
        // Epoch (median) of the catalog
        _epochMed = 0.5 * (_epochFrom + _epochTo);
    }

    // list of star properties that this catalog can overwrite
    if ((overwritePropertyIDList != NULL) && (overwritePropertyIDList->size() > 0))
    {
        for (vobsSTAR_PROPERTY_ID_LIST::const_iterator propertyIDIterator = overwritePropertyIDList->begin();
             propertyIDIterator != overwritePropertyIDList->end(); propertyIDIterator++)
        {
            _overwritePropertyIDList.push_back(*propertyIDIterator);
        }
    }
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

/*___oOo___*/
