/*******************************************************************************
* JMMC project
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* gzins     15-Jun-2004  created
*
*-----------------------------------------------------------------------------*/

static char *rcsId="@(#) $Id: mcs.c,v 1.2 2004-06-21 16:37:30 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers
 */
#include <string.h>

/* 
 * Local Headers
 */
#include "mcs.h"
#include "mcsPrivate.h"

/*
 * Local variables 
 */
static mcsPROCNAME mcsProcName = mcsUNKNOWN_PROC;

/*
 * Local functions 
 */
static mcsCOMPL_STAT mcsStoreProcName (const char *procName);

/**
 * \mainpage mcs : MCS services 
 * 
 * \n
 * \section description Description
 * This module is provided mcsInit() function which has to be used to register
 * process to the MCS services. A process has to be registered before calling a
 * MCS services (log, msg, ...), as shown in the following example.
 * 
 * \section example Code example
 * \code
 *  #include "mcs.h"
 *  ...
 *   
 *  int main(int argc, char *argv[])
 *  {
 *      if (mcsInit(argv[0]) == FAILURE)
 *      {
 *          Process Error condition
 *          exit (EXIT_FAILURE);
 *      }
 *      ...
 *
 *      mcsExit();
 *
 *      exit (EXIT_SUCCESS);
 *  }
 *
 * \endcode
 */

/**
 * Initializes the MCS services.
 * It register application to the MCS services. If process is not identified,
 * then default name is mcsUNKNOWN_PROC.
 *
 * \param procName name of the process.
 *
 * \warning This routine must be called ONCE, normally in the initialisation
 * phase of the applications, BEFORE any call to a MCS function (log, msg,...)
 * can be performed.
 *
 * \return SUCCESS
 *
 * \sa mcsGetProcName
 */
mcsCOMPL_STAT mcsInit(const mcsPROCNAME  procName)
{
    /* Store the application name */
    mcsStoreProcName(procName);

    return SUCCESS;
}

/**
 * Returns the process name.
 *
 * It returns the name which has been used to register to the process using
 * mcsInit().
 *
 * \return the process name.
 *
 * \sa mcsInit
 */
const char *mcsGetProcName()
{
    return ((const char*)mcsProcName);
}

/**
 * Closes MCS services. 
 *
 * It performs the necessary clean up for applications making use of MCS.
 *
 * \warning Any application program, which has previously used mcsInit() must
 * call mcsExit() before terminating.
 *
 * \sa mcsInit
 */
void mcsExit()
{
    /* Store the application name */
    mcsStoreProcName(mcsUNKNOWN_PROC);
}

/*
 * Local functions
 */
/**
 * Stores the curent process name.
 *
 * Strips off from the process name the possible path, which could have been
 * specified to invoke the process.  For example  : "../bin/myProg" becomes
 * only "myProg". If the parameter given to mcsStoreMyName() is NULL, the name
 * of the process is set to mcsUNKNOWN_PROC.
 *
 * THIS FUNCTION IS FOR INTERNAL USE ONLY.
 *
 * \return SUCCESS
 *
 * \sa mcsInit
 */
mcsCOMPL_STAT mcsStoreProcName (const char *procName)
{
    char *pchar, *path;
    pchar = (char *) procName;
    path  = strrchr(procName,'/');

    if (path != (char *) NULL)  
    { 
        pchar = path + 1; 
    }   

    strncpy((char *)mcsProcName, pchar, (sizeof(mcsProcName)-1));

    return SUCCESS;
}
/*___oOo___*/
