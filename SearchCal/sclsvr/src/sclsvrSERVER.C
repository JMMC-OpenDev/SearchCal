/*******************************************************************************
* JMMC project
*
* "@(#) $Id: sclsvrSERVER.C,v 1.2 2004-11-30 15:48:09 scetre Exp $"
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

static char *rcsId="@(#) $Id: sclsvrSERVER.C,v 1.2 2004-11-30 15:48:09 scetre Exp $"; 
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
    
    evhCMD_KEY key1("GETSTAR");
    evhCMD_CALLBACK cb1(this, (evhCMD_CB_METHOD)&sclsvrSERVER::GetStarCB);
    AddCallback(key1, cb1);
    
    return SUCCESS;
}
 


/*
 * Protected methods
 */



/*
 * Private methods
 */



/*___oOo___*/
