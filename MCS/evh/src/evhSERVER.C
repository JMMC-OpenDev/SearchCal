/*******************************************************************************
* JMMC project
*
* "@(#) $Id: evhSERVER.C,v 1.1 2004-11-17 10:27:20 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     09-Nov-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * evhSERVER class definition.
 */

static char *rcsId="@(#) $Id: evhSERVER.C,v 1.1 2004-11-17 10:27:20 gzins Exp $"; 
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
#include "evhSERVER.h"
#include "evhPrivate.h"

/*
 * Class constructor
 */
evhSERVER::evhSERVER()
{
}

/*
 * Class destructor
 */
evhSERVER::~evhSERVER()
{
}


mcsCOMPL_STAT evhSERVER::Init()
{
    logExtDbg("evhSERVER::Init()");

    evhCMD_KEY key("VERSION");
    evhCMD_CALLBACK cb(this, (evhCMD_CB_METHOD)&evhSERVER::VersionCB);
    AddCallback(key, cb);
    
    return SUCCESS;
}


/*___oOo___*/
