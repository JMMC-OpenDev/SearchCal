/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Storage class used to hold all the data about each process connected to the
 * \<msgManager\>.
 *
 * \sa msgPROCESS
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: msgPROCESS.cpp,v 1.7 2006-05-11 13:04:56 mella Exp $";
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
