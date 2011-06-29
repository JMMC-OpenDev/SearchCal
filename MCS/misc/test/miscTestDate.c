/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/* 
 * System Headers 
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "err.h"


/*
 * Local Headers 
 */
#include "miscDate.h"


/* 
 * Main
 */

int main (int argc, char *argv[])
{
    /* Give process name to mcs library */
    mcsInit(argv[0]);
    
    mcsSTRING32 timeStr;
    mcsUINT32   precision;

    /* Test of miscGetUtcTimeStr() */
    printf("miscGetUtcTimeStr() Function Test :\n\n");
    for (precision = 0; precision < 7; precision++)
    {
        printf("   UTC Time    (precision s=%u)  =  ", precision);

        if (miscGetUtcTimeStr(precision, timeStr) == mcsFAILURE)
        {
            printf("mcsFAILURE.\n");
            errCloseStack();
        }
        else
        {
            printf("'%s'\n", timeStr);
        }
    }
    printf("\n\n");

    /* Test of miscGetLocalTimeStr() */
    printf("miscGetLocalTimeStr() Function Test :\n\n");
    for (precision = 0; precision < 7; precision++)
    {
        printf("   Local Time  (precision s=%u)  =  ", precision);

        if (miscGetLocalTimeStr(precision, timeStr) == mcsFAILURE)
        {
            printf("mcsFAILURE.\n");
            errCloseStack();
        }
        else
        {
            printf("'%s'\n", timeStr);
        }
    }
    printf("\n\n");

    exit(EXIT_SUCCESS);
}

/*___oOo___*/
