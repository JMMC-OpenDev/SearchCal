/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: msgPROCESS.cpp,v 1.3 2005-01-24 15:02:47 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * gzins     06-Dec-2004  Created
 * gzins     07-Jan-2005  Changed SUCCESS/FAILURE to mcsSUCCESS/mcsFAILURE 
 *
 ******************************************************************************/

/**
 * \file
 * msgPROCESS class definition.
 */

static char *rcsId="@(#) $Id: msgPROCESS.cpp,v 1.3 2005-01-24 15:02:47 gzins Exp $"; 
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

/*___oOo___*/
