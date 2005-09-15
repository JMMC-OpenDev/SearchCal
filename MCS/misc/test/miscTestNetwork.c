/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: miscTestNetwork.c,v 1.6 2005-09-15 14:26:20 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.5  2005/09/15 14:19:27  scetre
 * Added miscGetHostByName in the miscNetwork file
 *
 * Revision 1.4  2005/02/15 09:44:37  gzins
 * Added CVS log as file modification history
 *
 * lafrasse  03-Aug-2004  Created
 *
 ******************************************************************************/

static char *rcsId="@(#) $Id: miscTestNetwork.c,v 1.6 2005-09-15 14:26:20 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "err.h"


/*
 * Local Headers 
 */
#include "miscNetwork.h"


/* 
 * Main
 */

int main (int argc, char *argv[])
{
    /* Give process name to mcs library */
    mcsInit(argv[0]);

    mcsBYTES256  string;
    mcsUINT32    length = 256;

    /* Test of miscGetHostName() */
    printf("miscGetHostName() Function Test :\n\n");
    printf("miscGetHostName(NULL, 0)  = ");
    if (miscGetHostName(NULL, 0) == mcsFAILURE)
    {
        printf("mcsFAILURE.\n");
        errCloseStack();
    }
    else
    {
        printf("%s\n", string);
    }

    printf("miscGetHostName(string, 0)  = ");
    if (miscGetHostName(string, 0) == mcsFAILURE)
    {
        printf("mcsFAILURE.\n");
        errCloseStack();
    }
    else
    {
        printf("%s\n", string);
    }

    printf("miscGetHostName(string, length)  = ");
    if (miscGetHostName(string, length) == mcsFAILURE)
    {
        printf("mcsFAILURE.\n");
        errCloseStack();
    }
    else
    {
        printf("%s\n", string);
    }
    printf("\n\n");

    mcsSTRING32 host;
    mcsSTRING32 hostIp; 
    strcpy(host, "vizier.u-strasbg.fr");
    //strcpy(host, "130.79.128.13");
    if (miscGetHostByName(hostIp, host) == mcsFAILURE)
    {
        printf("mcsFAILURE.\n");
        errCloseStack();
    }

    printf("IP of '%s' = '%s'\n", host, hostIp);
    
    mcsExit();
    exit (EXIT_SUCCESS);
}

/*___oOo___*/
