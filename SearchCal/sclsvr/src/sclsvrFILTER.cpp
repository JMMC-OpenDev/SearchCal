/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrFILTER.cpp,v 1.1 2005-06-01 14:18:54 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * \file
 *  Definition of sclsvrFILTER class.
 */

static char *rcsId="@(#) $Id: sclsvrFILTER.cpp,v 1.1 2005-06-01 14:18:54 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

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
#include "sclsvrFILTER.h"
#include "sclsvrPrivate.h"

/**
 * Class constructor
 */
sclsvrFILTER::sclsvrFILTER()
{
    // by default, the filter is disabled
    Disable();
}

/**
 * Class destructor
 */
sclsvrFILTER::~sclsvrFILTER()
{
}

/*
 * Public methods
 */
/**
 * Get the filter name
 *
 * \return filter name
 */
char * sclsvrFILTER::GetName()
{
    logTrace("sclsvrFILTER::GetName()");

    return _name;
}

/**
 * Say if the filter is enable or not
 */
mcsLOGICAL sclsvrFILTER::IsEnable()
{
    logTrace("sclsvrFILTER::IsEnable()");

    return _isEnable;
}

/**
 * Set as enable the filter 
 */
mcsCOMPL_STAT sclsvrFILTER::Enable()
{
    logTrace("sclsvrFILTER::Enable()");
    
    _isEnable = mcsTRUE;
    
    return mcsSUCCESS;
}

/**
 * Set as disable the filter
 */
mcsCOMPL_STAT sclsvrFILTER::Disable()
{
    logTrace("sclsvrFILTER::Disable()");

    _isEnable = mcsFALSE;
    
    return mcsSUCCESS;
}

/*
 * Protected methods
 */


/*
 * Private methods
 */


/*___oOo___*/
