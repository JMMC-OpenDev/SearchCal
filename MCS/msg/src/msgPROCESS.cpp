/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: msgPROCESS.cpp,v 1.4 2005-01-29 19:56:16 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2005/01/24 15:02:47  gzins
 * Added CVS logs as modification history
 *
 * gzins     06-Dec-2004  Created
 * gzins     07-Jan-2005  Changed SUCCESS/FAILURE to mcsSUCCESS/mcsFAILURE 
 *
 ******************************************************************************/

/**
 * \file
 * msgPROCESS class definition.
 */

static char *rcsId="@(#) $Id: msgPROCESS.cpp,v 1.4 2005-01-29 19:56:16 gzins Exp $"; 
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
#include "msgPROCESS.h"
#include "msgPrivate.h"

/**
 * Class constructor
 */
msgPROCESS::msgPROCESS()
{
    memset(_name, '\0', sizeof(mcsPROCNAME)); 
    _id = -1;
    _unicity = mcsFALSE;
}

/**
 * Class destructor
 */
msgPROCESS::~msgPROCESS()
{
}

/**
 * Set the name of the process; i.e. MCS registering name.
 *
 * \return mcsSUCCESS. 
 */
mcsCOMPL_STAT msgPROCESS::SetName(char *name)
{
    logExtDbg("msgPROCESS::SetName()");

    strncpy(_name , name, sizeof(mcsPROCNAME)); 

    return mcsSUCCESS;
}

/**
 * Get the name of the process; i.e. MCS registering name.
 *
 * \return MCS process name. 
 */
const char *msgPROCESS::GetName() const
{
    logExtDbg("msgPROCESS::GetName()");

    return _name;
}

/**
 * Set the process ID
 *
 * \return mcsSUCCESS. 
 */
mcsCOMPL_STAT msgPROCESS::SetId(mcsINT32 pid)
{
    logExtDbg("msgPROCESS::SetId()");

    _id = pid;

    return mcsSUCCESS;
}

/**
 * Get the process ID
 *
 * \return process ID. 
 */
mcsINT32 msgPROCESS::GetId() const
{
    logExtDbg("msgPROCESS::GetId()");

    return _id;
}

/**
 * Set the unicity flag.
 *
 * If unicity flag is true, this means the process is unique; it is not allowed
 * to have more than one instance connected to the message service at the same
 * time.
 *
 * \return mcsSUCCESS. 
 */
mcsCOMPL_STAT msgPROCESS::SetUnicity(mcsLOGICAL flag)
{
    logExtDbg("msgPROCESS::SetUnicity()");

    _unicity = flag;

    return mcsSUCCESS;
}

/**
 * Get the unicity flag.
 *
 * \return unicity flag.
 */
mcsLOGICAL msgPROCESS::IsUnique() const
{
    logExtDbg("msgPROCESS::GetId()");

    return _unicity;
}

/*___oOo___*/
