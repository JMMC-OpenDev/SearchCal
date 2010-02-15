/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: miscTestNetwork.c,v 1.11 2010-02-15 15:59:55 mella Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.10  2010/01/15 17:05:45  lafrasse
 * Updated miscPerformHttpGet() to use miscDynBufExecuteCommand().
 *
 * Revision 1.9  2008/04/04 12:30:04  lafrasse
 * Added miscPerformHttpGet() function.
 *
 * Revision 1.8  2006/05/11 13:04:56  mella
 * Changed rcsId declaration to perform good gcc4 and gcc3 compilation
 *
 * Revision 1.7  2005/10/06 15:11:23  lafrasse
 * Corrections in order to ensure compilation of src and test again
 *
 * Revision 1.6  2005/09/15 14:26:20  scetre
 * Added miscGetHostByName in the miscNetwork file and test
 *
 * Revision 1.5  2005/09/15 14:19:27  scetre
 * Added miscGetHostByName in the miscNetwork file
 *
 * Revision 1.4  2005/02/15 09:44:37  gzins
 * Added CVS log as file modification history
 *
 * lafrasse  03-Aug-2004  Created
 *
 ******************************************************************************/

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: miscTestNetwork.c,v 1.11 2010-02-15 15:59:55 mella Exp $";
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

    /* miscGetHostName() Function Test  */
    mcsBYTES256  string;
    mcsUINT32    length = 256;
    printf("-------------------------------\n");
    printf("miscGetHostName() Function Test :\n");
    printf("-------------------------------\n");
    printf("miscGetHostName(NULL, 0) = ");
    if (miscGetHostName(NULL, 0) == mcsFAILURE)
    {
        printf("mcsFAILURE.\n");
        errCloseStack();
    }
    else
    {
        printf("'%s'\n", string);
    }
    printf("\n");
    printf("miscGetHostName(string, 0) = ");
    if (miscGetHostName(string, 0) == mcsFAILURE)
    {
        printf("mcsFAILURE.\n");
        errCloseStack();
    }
    else
    {
        printf("'%s'\n", string);
    }
    printf("\n");
    printf("miscGetHostName(string, length) = ");
    if (miscGetHostName(string, length) == mcsFAILURE)
    {
        printf("mcsFAILURE.\n");
        errCloseStack();
    }
    else
    {
        printf("'%s'\n", string);
    }
    printf("\n\n");

    /* miscGetHostByName() Function Test  */
    printf("---------------------------------\n");
    printf("miscGetHostByName() Function Test :\n");
    printf("---------------------------------\n");
    char* host = "vizier.u-strasbg.fr";
    mcsSTRING32 hostIp;
    printf("miscGetHostByName('%s') = ", host);
    if (miscGetHostByName(hostIp, host) == mcsFAILURE)
    {
        printf("mcsFAILURE.\n");
        errCloseStack();
    }
    else
    {
        printf("'%s'\n", hostIp);
    }
    printf("\n\n");

    /* miscPerformHttpPost() Function Test  */
    printf("----------------------------------\n");
    printf("miscPerformHttpPost() Function Test :\n");
    printf("----------------------------------\n");
    miscDYN_BUF result;
    miscDynBufInit(&result);
    char* postUri = "http://vizier.u-strasbg.fr/viz-bin/asu-xml";
    char* postData = "-source=I/280&-c.ra=22:57:39.05&-c.dec=-29:37:20.1&Vmag=0.00..4.00&-c.eq=J2000&-out.max=100&-c.geom=b&-c.bm=3391/1200&-c.u=arcmin&-out.add=_RAJ2000,_DEJ2000&-oc=hms&-out=*POS_EQ_PMDEC&-out=*POS_EQ_PMRA&-out=*POS_PARLX_TRIG&-out=e_Plx&-out=*SPECT_TYPE_MK&-out=*PHOT_JHN_B&-out=*PHOT_JHN_V&-out=v1&-out=v2&-out=v3&-out=d5&-out=HIP&-out=HD&-out=DM&-out=TYC1&-sort=_r&SpType=%5bOBAFGKM%5d*";
    printf("miscPerformHttpPost('%.50s ...') ", postUri);
    if (miscPerformHttpPost(postUri, postData, &result, 0) == mcsFAILURE)
    {
        printf("= mcsFAILURE.\n");
        errCloseStack();
    }
    else
    {
        printf(":\n%s", miscDynBufGetBuffer(&result));
    }
    printf("\n");
 
    /* miscPerformHttpGet() Function Test  */
    printf("----------------------------------\n");
    printf("miscPerformHttpGet() Function Test :\n");
    printf("----------------------------------\n");
    miscDynBufInit(&result);
    char* uri = "http://vizier.u-strasbg.fr/viz-bin/asu-xml?-source=I/280&-c.ra=22:57:39.05&-c.dec=-29:37:20.1&Vmag=0.00..4.00&-c.eq=J2000&-out.max=100&-c.geom=b&-c.bm=3391/1200&-c.u=arcmin&-out.add=_RAJ2000,_DEJ2000&-oc=hms&-out=*POS_EQ_PMDEC&-out=*POS_EQ_PMRA&-out=*POS_PARLX_TRIG&-out=e_Plx&-out=*SPECT_TYPE_MK&-out=*PHOT_JHN_B&-out=*PHOT_JHN_V&-out=v1&-out=v2&-out=v3&-out=d5&-out=HIP&-out=HD&-out=DM&-out=TYC1&-sort=_r&SpType=%5bOBAFGKM%5d*";
    printf("miscPerformHttpGet('%.50s ...') ", uri);
    if (miscPerformHttpGet(uri, &result, 0) == mcsFAILURE)
    {
        printf("= mcsFAILURE.\n");
        errCloseStack();
    }
    else
    {
        printf(":\n%s", miscDynBufGetBuffer(&result));
    }
    printf("\n");
    uri = "http://vizier.u-strasbg.fr/viz-bin/asu-xml?-source=I/280";
    printf("miscPerformHttpGet('%s') ", uri);
    if (miscPerformHttpGet(uri, &result, 0) == mcsFAILURE)
    {
        printf("= mcsFAILURE.\n");
        errCloseStack();
    }
    else
    {
        printf(":\n%s", miscDynBufGetBuffer(&result));
    }
    printf("\n");
    uri = "http://apple.co";
    printf("miscPerformHttpGet('%.50s ...') ", uri);
    if (miscPerformHttpGet(uri, &result, 0) == mcsFAILURE)
    {
        printf("= mcsFAILURE.\n");
        errCloseStack();
    }
    else
    {
        printf(":\n%s", miscDynBufGetBuffer(&result));
    }
    printf("\n");
    uri = "htp://apple.com";
    printf("miscPerformHttpGet('%.50s ...') ", uri);
    if (miscPerformHttpGet(uri, &result, 0) == mcsFAILURE)
    {
        printf("= mcsFAILURE.\n");
        errCloseStack();
    }
    else
    {
        printf(":\n%s", miscDynBufGetBuffer(&result));
    }
    printf("\n\n");

    mcsExit();
    exit (EXIT_SUCCESS);
}

/*___oOo___*/
