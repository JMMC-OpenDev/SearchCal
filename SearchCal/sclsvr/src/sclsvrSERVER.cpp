/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrSERVER.cpp,v 1.6 2005-06-01 14:18:54 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.5  2005/03/06 20:29:40  gzins
 * Added GetSwVersion method
 *
 * Revision 1.4  2005/01/26 13:13:04  gzins
 * Added CVS log as modification history.
 * Added CDF file when attaching command callbacks.
 *
 ******************************************************************************/

/**
 * \file
 * Definition of the sclsvrSERVER class.
 */

static char *rcsId="@(#) $Id: sclsvrSERVER.cpp,v 1.6 2005-06-01 14:18:54 scetre Exp $"; 
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
#include "sclsvrSERVER.h"
#include "sclsvrGETCAL_CMD.h"
#include "sclsvrGETSTAR_CMD.h"
#include "sclsvrPrivate.h"
#include "sclsvrVersion.h"

/*
 * Class constructor
 */
sclsvrSERVER::sclsvrSERVER()
{
}

/*
 * Class destructor
 */
sclsvrSERVER::~sclsvrSERVER()
{
}

/*
 * Public methods
 */
mcsCOMPL_STAT sclsvrSERVER::AppInit()
{
    logTrace("sclsvrSERVER::AppInit()");

    evhCMD_KEY key(sclsvrGETCAL_CMD_NAME, sclsvrGETCAL_CDF_NAME);
    evhCMD_CALLBACK cb(this, (evhCMD_CB_METHOD)&sclsvrSERVER::GetCalCB);
    AddCallback(key, cb);
    
    evhCMD_KEY key1(sclsvrGETSTAR_CMD_NAME, sclsvrGETSTAR_CDF_NAME);
    evhCMD_CALLBACK cb1(this, (evhCMD_CB_METHOD)&sclsvrSERVER::GetStarCB);
    AddCallback(key1, cb1);
    
    return mcsSUCCESS;
}
 

/**
 * Return the version number of the software.
 */
const char *sclsvrSERVER::GetSwVersion()
{
    return sclsvrVERSION;
}

/*
 * Protected methods
 */



/*
 * Private methods
 */



/*___oOo___*/
