/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Definition of vobsFILTER class.
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
#include "vobsFILTER.h"
#include "vobsPrivate.h"

/**
 * Class constructor
 * 
 * @param filterId a pointer on an already allocated character buffer.
 */
vobsFILTER::vobsFILTER(const char* filterId)
{
    // Store the filter Id
    _id = filterId;

    // Disable the filter by default
    Disable();
}

/**
 * Class destructor
 */
vobsFILTER::~vobsFILTER()
{
}

/*
 * Public methods
 */

/**
 * Return whether the filter is enable or not
 *
 * @return mcsTRUE if the filter is enable, mcsFALSE otherwise.
 */
mcsLOGICAL vobsFILTER::IsEnabled(void) const
{
    return _isEnable;
}

/**
 * Enable the filter
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsFILTER::Enable(void)
{
    _isEnable = mcsTRUE;

    return mcsSUCCESS;
}

/**
 * Disable the filter
 *
 * @return always mcsSUCCESS 
 */
mcsCOMPL_STAT vobsFILTER::Disable(void)
{
    _isEnable = mcsFALSE;

    return mcsSUCCESS;
}

/**
 * Get the filter Id.
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */
const char* vobsFILTER::GetId(void) const
{
    return _id;
}

mcsCOMPL_STAT vobsFILTER::DumpAsXML(miscoDYN_BUF& buffer) const
{
    if (IS_TRUE(IsEnabled()))
    {
        /* minimal implementation giving only id */
        FAIL(buffer.AppendString("      <filter>"));
        FAIL(buffer.AppendString(GetId()));
        FAIL(buffer.AppendString("</filter>\n"));
    }
    return mcsSUCCESS;
}

/*
 * Protected methods
 */


/*
 * Private methods
 */


/*___oOo___*/
