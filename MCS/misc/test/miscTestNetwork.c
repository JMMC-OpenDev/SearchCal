/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

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

    /* miscUrl*() Function Test  */
    printf("-------------------------------\n");
    printf("miscUrlEncode/Decode() Function Test :\n");
    printf("-------------------------------\n");
 
    char * url = "http://www.jmmc.fr/&% toto=";
    char * encodedUrl = miscUrlEncode(url);
    char * decodedUrl = miscUrlDecode(encodedUrl);
    printf("miscUrlEncode(%s) = '%s'\n", url, encodedUrl);
    printf("miscUrlDecode(%s) = '%s'\n", encodedUrl, decodedUrl);
    free(encodedUrl);
    free(decodedUrl);

    encodedUrl = miscUrlEncode(NULL);
    decodedUrl = miscUrlDecode(NULL);
    printf("miscUrlEncode(NULL) = '%s'\n", encodedUrl);
    printf("miscUrlDecode(NULL) = '%s'\n", decodedUrl);
    free(encodedUrl);
    free(decodedUrl);

    printf("\n\n");



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
