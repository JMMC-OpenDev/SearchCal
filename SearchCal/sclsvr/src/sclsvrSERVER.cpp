/*******************************************************************************
* JMMC project
*
* "@(#) $Id: sclsvrSERVER.cpp,v 1.3 2005-01-25 11:05:39 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     23-Nov-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * sclsvrSERVER class definition.
 */

static char *rcsId="@(#) $Id: sclsvrSERVER.cpp,v 1.3 2005-01-25 11:05:39 gzins Exp $"; 
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
    logExtDbg("sclsvrSERVER::AppInit()");

    evhCMD_KEY key(sclsvrGETCAL_CMD_NAME);
    evhCMD_CALLBACK cb(this, (evhCMD_CB_METHOD)&sclsvrSERVER::GetCalCB);
    AddCallback(key, cb);
    
    evhCMD_KEY key1(sclsvrGETSTAR_CMD_NAME);
    evhCMD_CALLBACK cb1(this, (evhCMD_CB_METHOD)&sclsvrSERVER::GetStarCB);
    AddCallback(key1, cb1);
    
    return mcsSUCCESS;
}
 


/*
 * Protected methods
 */



/*
 * Private methods
 */



/*___oOo___*/
