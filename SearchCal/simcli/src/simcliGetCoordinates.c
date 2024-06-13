/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Brief description of the class file, which ends at this dot.
 *
 */


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
#include "miscNetwork.h"


/*
 * Local Headers
 */
#include "simcli.h"
#include "simcliPrivate.h"


/* trace flag */
#define TRACE   0

/*
 * Public functions definition
 */
mcsCOMPL_STAT simcliGetCoordinates(char *name,
                                   char *ra, char *dec,
                                   mcsDOUBLE *pmRa, mcsDOUBLE *pmDec,
                                   char *spType, char *objTypes,
                                   char *mainId)
{
    miscDYN_BUF result;
    miscDynBufInit(&result);
    miscDYN_BUF url;
    miscDynBufInit(&url);

    /* reset outputs */
    strcpy(ra, "\0");
    strcpy(dec, "\0");
    *pmRa = 0.0;
    *pmDec = 0.0;
    strcpy(spType, "\0");
    strcpy(objTypes, "\0");
    strcpy(mainId, "\0");

    /* Replace '_' by ' ' */
    char starName[256];
    char* p;
    strncpy(starName, name, sizeof (starName) - 1);
    p = strchr(starName, '_');
    while (p != NULL)
    {
        *p = ' ';
        p = strchr(starName, '_');
    }

    char* encodedName = miscUrlEncode(starName);

    miscDynBufAppendString(&url, "http://simbad.cds.unistra.fr/simbad/sim-script?submit=submit+script&script=%23+only+data%0D%0Aoutput+console%3Doff+script%3Doff+%0D%0A%0D%0A%23+ask+for%3A%0D%0A%23+ra%2C+dec%2C+%26pmRa%2C+%26pmDec%2C+spType%2C+objTypes%2C+mainId%0D%0A%0D%0Aformat+object+form1+%22%25COO%28d%3BA%29%3B%25COO%28d%3BD%29%3B%25PM%28A%3BD%29%3B%25SP%28S%29%3B%25OTYPELIST%3B%25MAIN_ID%3B%22%0D%0Aquery+id+");
    miscDynBufAppendString(&url, encodedName);
    free(encodedName);

    if (TRACE) printf("querying simbad ... (%s)\n", miscDynBufGetBuffer(&url));

    if (miscPerformHttpGet(miscDynBufGetBuffer(&url), &result, 0) == mcsFAILURE)
    {
        errCloseStack();
        return mcsFAILURE;
    }

    char* response = miscDynBufGetBuffer(&result);
    if (TRACE) printf("Response:\n%s\n---\n", response);

    /* fails if sim-script outputs :error:::: */
    if (IS_NOT_NULL(strstr(response, ":error:")))
    {
        errCloseStack();
        miscDynBufDestroy(&result);
        return mcsFAILURE;
    }

    char *token;
    int fieldIndex = 0;

    token = strtok(response, ";");
    while (token != NULL)
    {
        /* skip blanking values */
        if (token[0] != '~')
        {
            if (TRACE) printf("parsing field[%d]: '%s'\n", fieldIndex, token);

            switch (fieldIndex)
            {
                case 0:
                    mcsDOUBLE ss;
                    sscanf(token, "%lf", &ss);
                    ss = ss / 15.0;
                    int HH = ss;
                    int MM = ((ss - HH)*60.0);
                    mcsDOUBLE SS = (ss - HH)*3600 - (MM * 60);
                    sprintf(ra, "%02d %02d %010.7lf", HH, MM, SS);
                    break;
                case 1:
                    mcsDOUBLE ds;
                    sscanf(token, "%lf", &ds);
                    char sign = (ds < 0.0) ? '-' : '+';
                    ds = (ds < 0.0) ? -ds : ds;
                    int DH = ds;
                    int DM = ((ds - DH)*60.0);
                    mcsDOUBLE DS = (ds - DH)*3600 - (DM * 60);
                    sprintf(dec, "%c%d %02d %09.6f", sign, DH, DM, DS);
                    break;
                case 2:
                    *pmRa = atof(token);
                    break;
                case 3:
                    *pmDec = atof(token);
                    break;
                case 4:
                    strcpy(spType, token);
                    break;
                case 5:
                    strcpy(objTypes, ",");
                    strcat(objTypes, token);
                    strcat(objTypes, ",");
                    break;
                case 6:
                    /* trim space character (left/right) */
                    /* get the first token */
                    char *tok = strtok(token, " ");

                    /* walk through other tokens */
                    while (tok != NULL)
                    {
                        if (strlen(mainId) != 0)
                        {
                            strcat(mainId, " ");
                        }
                        strcat(mainId, tok);

                        tok = strtok(NULL, " ");
                    }
                    break;
                default:
                    miscDynBufDestroy(&result);
                    return mcsFAILURE;
            }
        }
        token = strtok(NULL, ";");
        fieldIndex++;
    }

    miscDynBufDestroy(&result);
    return mcsSUCCESS;
}
/*___oOo___*/
