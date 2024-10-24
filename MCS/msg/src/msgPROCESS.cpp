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

/*
 * System Headers
 */
#include <iostream>
#include <string.h>
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
    memset(_name, '\0', mcsPROCNAME_LEN);
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
    strncpy(_name , name, mcsPROCNAME_LEN - 1);

    return mcsSUCCESS;
}

/**
 * Get the process name.
 *
 * \return a character pointer on the process name
 */
const char *msgPROCESS::GetName(void) const
{
    return _name;
}

/**
 * Set the process identifier
 *
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT msgPROCESS::SetId(const mcsINT32 pid)
{
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
    return _id;
}

/**
 * Specify whether only one instance or multiple instances of the process are
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
    _unicity = flag;

    return mcsSUCCESS;
}

/**
 * Retrieve whether only one instance or multiple instances of the process are
 * allowed to be connected to \<msgManager\> at the same time.
 *
 * \return mcsTRUE if your process is the only one with its name allowed to be
 * connected to the \<msgManager\> at a given time, mcsFALSE otherwise
 */
mcsLOGICAL msgPROCESS::IsUnique(void) const
{
    return _unicity;
}

/*___oOo___*/
