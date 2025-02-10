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
 * Return the string literal representing the origin index
 * @return string literal "NO CATALOG", "MIXED CATALOG", "computed", "II/297/irc" ... "B/wds/wds"
 */
const char* vobsGetOriginIndex(vobsORIGIN_INDEX originIndex)
{
    return vobsORIGIN_STR[originIndex];
}

/**
 * Return the integer literal representing the origin index
 * @return integer literal "0" (NO CATALOG), "1" (MIXED CATALOG), "2" (computed), "3" (II/297/irc) ... "21" (B/wds/wds)
 */
const char* vobsGetOriginIndexAsInt(vobsORIGIN_INDEX originIndex)
{
    return vobsORIGIN_INT[originIndex];
}

/** Initialize static members */
vobsSTAR_PROPERTY_META_PTR_LIST vobsSTAR_PROPERTY_META::vobsStar_PropertyMetaList;

/**
 * Class constructor
 * 
 * @param id property identifier (UCD)
 * @param name property name 
 * @param type property type
 * @param unit property unit, "" by default or for 'NULL'.
 * @param format format used to set property (%s or %.5g by default or for 'NULL').
 * @param link link for this property (none by default or for 'NULL').
 * @param description property description (none by default or for 'NULL').
 * @param isError flag to indicate if this meta data describes a property error (true) or a property value (false)
 */
vobsSTAR_PROPERTY_META::vobsSTAR_PROPERTY_META(const char* id,
                                               const char* name,
                                               const vobsPROPERTY_TYPE type,
                                               const char* unit,
                                               const char* format,
                                               const char* link,
                                               const char* description,
                                               const bool  isError)
{
    _id   = id;
    _name = name;
    _type = type;
    _unit = IS_STR_EMPTY(unit) ? NULL : unit;

    if (IS_NULL(format))
    {
        const char* defaultFormat = "%s";
        switch (type)
        {
            default:
            case vobsSTRING_PROPERTY:
                defaultFormat = "%s";
                break;

            case vobsFLOAT_PROPERTY:
                defaultFormat = FORMAT_DEFAULT; // 1.123456e-5 (scientific notation with up to 6 digits)
                break;

            case vobsINT_PROPERTY:
            case vobsLONG_PROPERTY:
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

    _link        = IS_STR_EMPTY(link) ? NULL : link;
    _description = IS_STR_EMPTY(description) ? NULL : description;

    _isError     = isError;
    _errorMeta   = NULL;
}

/**
 * Destructor
 */
vobsSTAR_PROPERTY_META::~vobsSTAR_PROPERTY_META()
{
    if (IS_NOT_NULL(_errorMeta))
    {
        delete _errorMeta;
    }
}

/*___oOo___*/
