/*******************************************************************************
* JMMC project
*
* "@(#) $Id: cmdPARAM.C,v 1.2 2004-11-23 08:36:36 mella Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* mella     15-Nov-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * cmdPARAM class definition.
 */

static char *rcsId="@(#) $Id: cmdPARAM.C,v 1.2 2004-11-23 08:36:36 mella Exp $"; 
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
#include "cmd.h"
#include "cmdPARAM.h"
#include "cmdPrivate.h"

/*
 * Class constructor
 */

/** 
 *  Constructs a new named Parameter 
 *
 * \param name  the name of the parameter.
 * \param desc  
 * \param unit  
 * \param optional  
 *
 */
cmdPARAM::cmdPARAM(string name, string desc, string unit, mcsLOGICAL optional)
{
    logExtDbg("cmdPARAM::cmdPARAM");
    _name = name;
    _desc = desc;
    _unit = unit;
    _optional = optional;
}


/*
 * Class destructor
 */


/** 
 *  Destructs the parameter
 *
 */
cmdPARAM::~cmdPARAM()
{
    logExtDbg("cmdPARAM::~cmdPARAM");
}

/*
 * Public methods
 */

/** 
 *  Get the name of the parameter.
 *
 *  \returns the string containing the name.
 */
string cmdPARAM::getName()
{
    logExtDbg("cmdPARAM::getName()");
    return _name;
}

/** 
 *  Get the description of the parameter.
 *
 *  \returns the string containing the description or empty.
 */
string cmdPARAM::getDesc()
{
    logExtDbg("cmdPARAM::getDesc()");
    return _desc;
}

/** 
 *  Get the unit of the parameter.
 *
 *  \returns the string containing the unit or empty.
 */
string cmdPARAM::getUnit()
{
    logExtDbg("cmdPARAM::getUnit()");
    return _unit;
}

/** 
 *  Get the user value of the parameter.
 *
 *  \returns the string containing the user value.
 */
string cmdPARAM::getUserValue()
{
    logExtDbg("cmdPARAM::getUserValue()");
    return _userValue;
}

/** 
 *  Get the default value of the parameter.
 *
 *  \returns the string containing the user value.
 */
string cmdPARAM::getDefaultValue()
{
    logExtDbg("cmdPARAM::getDefaultValue()");
    return _defaultValue;
}

/** 
 *  Return if the parameter is optional.
 *
 *  \returns mcsTRUE or mcsFALSE
 */
mcsLOGICAL cmdPARAM::isOptional()
{
    logExtDbg("cmdPARAM::isOptional()");
    return _optional;
}

/** 
 * Return if the parameter has got a default value.  
 *
 *  \returns mcsTRUE or mcsFALSE
 */
mcsLOGICAL cmdPARAM::hasDefaultValue()
{
    logExtDbg("cmdPARAM::hasDefaultValue()");
    if(_defaultValue.empty())
    {
        return mcsFALSE;
    }
    else
    {
        return mcsTRUE;
    }
}

/** 
 * Return if the parameter has got a user value.  
 *
 *  \returns mcsTRUE or mcsFALSE
 */
mcsLOGICAL cmdPARAM::isDefined()
{
    logExtDbg("cmdPARAM::isDefined()");
    if(_userValue.empty())
    {
        return mcsFALSE;
    }
    else
    {
        return mcsTRUE;
    }
}

/** 
 *  Return the help of the parameter.
 *
 *  \returns the help string
 */
string cmdPARAM::getHelp()
{
    logExtDbg("cmdPARAM::getHelp()");
    string s;
    if(_optional)
    {
        s.append("* optional parameter \t -- ");
    }
    else if(hasDefaultValue())
    {
        s.append("* default parameter \t -- ");
    }
    else
    {
        s.append("* mandatory parameter\t -- ");
    }
    
    s.append(_name);
    s.append(" -- \n");

    /* If there is one given unit */
    if(! _unit.empty())
    {
        s.append("\tUnit:[");
        s.append(_unit);
        s.append("]");
    }
    
    /* If there is one given unit */
    if(! _userValue.empty())
    {
        s.append("\tUser value:[");
        s.append(_userValue);
        s.append("]");
    }
    
    /* If there is one defaultValue */
    if(hasDefaultValue())
    {
        s.append("\tDefault value:[");
        s.append(_defaultValue);
        s.append("]");
    }
    
    /* If there is one given unit */
    if(! _desc.empty())
    {
        s.append("\n\t");
        s.append(_desc);
    }
    else
    {
        s.append("\n\tNo description");
    }
    
    s.append("\n");
    return s;
}

/** 
 * Set the user value of the parameter. This method must be called only by cmdCMD.
 * The value is extracted from the parameter line.
 *
 * \param value  the new user value.
 *
 *  \returns an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT cmdPARAM::setUserValue(string value)
{
    logExtDbg("cmdPARAM::setUserValue()");
        cout << "value is " << value << endl;
    _userValue=value;
    return SUCCESS;
}

/** 
 * Set the default value of the parameter. This method must be called only
 * by cmdCMD. The value is extracted from the cdf file.
 *
 * \param value  the new default value.
 *
 *  \returns an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT cmdPARAM::setDefaultValue(string value)
{
    logExtDbg("cmdPARAM::setDefaultValue()");
    _defaultValue=value;
    return SUCCESS;
}


/*
 * Protected methods
 */



/*
 * Private methods
 */



/*___oOo___*/
