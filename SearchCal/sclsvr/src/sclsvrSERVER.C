/*******************************************************************************
* JMMC project
*
* "@(#) $Id: sclsvrSERVER.C,v 1.1 2004-11-25 13:12:55 scetre Exp $"
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

static char *rcsId="@(#) $Id: sclsvrSERVER.C,v 1.1 2004-11-25 13:12:55 scetre Exp $"; 
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

    evhCMD_KEY key("SEARCH");
    evhCMD_CALLBACK cb(this, (evhCMD_CB_METHOD)&sclsvrSERVER::SearchCB);
    AddCallback(key, cb);
    

    return SUCCESS;
}
 


/*
 * Protected methods
 */



/*
 * Private methods
 */



/*___oOo___*/
