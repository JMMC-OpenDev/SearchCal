/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsSTAR_PROPERTY_META class definition.
 */

/* 
 * System Headers 
 */
#include <iostream>
#include <sstream> 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
#include "vobsSTAR_PROPERTY_META.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/**
 * Class constructor
 * 
 * @param id property identifier
 * @param name property name 
 * @param type property type
 * @param unit property unit, vobsSTAR_PROP_NOT_SET by default or for 'NULL'.
 * @param format format used to set property (%s or %.5g by default or for 'NULL').
 * @param link link for this property (none by default or for 'NULL').
 * @param description property description (none by default or for 'NULL').
 */
vobsSTAR_PROPERTY_META::vobsSTAR_PROPERTY_META(const char* id,
                                               const char* name,
                                               const vobsPROPERTY_TYPE type,
                                               const char* unit,
                                               const char* format,
                                               const char* link,
                                               const char* description)
{
    _id = id;
    _name = name;
    _type = type;

    _unit = (isNull(unit) || (strlen(unit) == 0)) ? vobsSTAR_VAL_NOT_SET : unit;

    if (isNull(format))
    {
        const char* defaultFormat = "%s";
        switch (type)
        {
            default:
            case vobsSTRING_PROPERTY:
                defaultFormat = "%s";
                break;

            case vobsFLOAT_PROPERTY:
                defaultFormat = "%.5g"; // 3.1234e-5 (scientific notation)
                break;
                
            case vobsINT_PROPERTY:
            case vobsBOOL_PROPERTY:
                defaultFormat = "%.0lf"; // double to integer conversion
                break;
        }
        _format = defaultFormat;
    }
    else
    {
        _format = format;
    }

    _link = (isNull(link) || (strlen(link) == 0)) ? NULL : link;
    _description = (isNull(description) || (strlen(description) == 0)) ? NULL : description;
}

/**
 * Destructor
 */
vobsSTAR_PROPERTY_META::~vobsSTAR_PROPERTY_META()
{
}

/*___oOo___*/
