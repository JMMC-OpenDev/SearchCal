/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: msgPROCESS.cpp,v 1.6 2005-02-09 16:38:11 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.5  2005/02/04 15:57:06  lafrasse
 * Massive documentation review an refinment (also added automatic CVS log inclusion in every files)
 *
 * Revision 1.4  2005/01/29 19:56:16  gzins
 * Added SetId/GetId and SetUnicity/IsUnique methods
 *
 * Revision 1.3  2005/01/24 15:02:47  gzins
 * Added CVS logs as modification history
 *
 * gzins     07-Jan-2005  Changed SUCCESS/FAILURE to mcsSUCCESS/mcsFAILURE 
 * gzins     06-Dec-2004  Created
 *
 ******************************************************************************/

/**
 * \file
 * Storage class used to hold all the data about each process connected to the
 * \<msgManager\>.
 *
 * \sa msgPROCESS
 */

static char *rcsId="@(#) $Id: msgPROCESS.cpp,v 1.6 2005-02-09 16:38:11 lafrasse Exp $"; 
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
 * Set the process name.
 *
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT msgPROCESS::SetName(const char *name)
{
    logExtDbg("msgPROCESS::SetName()");

    strncpy(_name , name, sizeof(mcsPROCNAME)); 

    return mcsSUCCESS;
}

/**
 * Get the process name.
 *
 * \return a character pointer on the process name
 */
const char *msgPROCESS::GetName(void) const
{
    logExtDbg("msgPROCESS::GetName()");

    return _name;
}

/**
 * Set the process identifier
 *
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT msgPROCESS::SetId(const mcsINT32 pid)
{
    logExtDbg("msgPROCESS::SetId()");

    _id = pid;

    return mcsSUCCESS;
}

/**
 * Get the process identifier
 *
 * \return the process identifier
 */
mcsINT32 msgPROCESS::GetId(void) const
{
    logExtDbg("msgPROCESS::GetId()");

    return _id;
}

/**
 * Specify wether only one instance or multiple instances of the process are
 * allowed to be connected to \<msgManager\> at the same time.
 *
 * \param flag if equals to mcsTRUE then your process will be the only one with
 * its name allowed to be connected to the \<msgManager\> at a given time,
 * otherwise multiple instance of the process will be allowed at the same time
 *
 * \return mcsSUCCESS
 */
mcsCOMPL_STAT msgPROCESS::SetUnicity(const mcsLOGICAL flag)
{
    logExtDbg("msgPROCESS::SetUnicity()");

    _unicity = flag;

    return mcsSUCCESS;
}

/**
 * Retrieve wether only one instance or multiple instances of the process are
 * allowed to be connected to \<msgManager\> at the same time.
 *
 * \return mcsTRUE if your process is the only one with its name allowed to be
 * connected to the \<msgManager\> at a given time, mcsFALSE otherwise
 */
mcsLOGICAL msgPROCESS::IsUnique(void) const
{
    logExtDbg("msgPROCESS::GetId()");

    return _unicity;
}

/*___oOo___*/
