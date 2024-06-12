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

/*
 * Public functions definition
 */
mcsCOMPL_STAT simcliGetCoordinates(char *name,
                                   char *ra, char *dec,
                                   mcsDOUBLE *pmRa, mcsDOUBLE *pmDec,
                                   char *spType, char *objTypes,
                                   char* mainId)
{
    miscDYN_BUF result;
    miscDynBufInit(&result);
    miscDYN_BUF url;
    miscDynBufInit(&url);

    char * encodedName = miscUrlEncode(name);

    miscDynBufAppendString(&url,"http://simbad.cds.unistra.fr/simbad/sim-script?submit=submit+script&script=%23+only+data%0D%0Aoutput+console%3Doff+script%3Doff+%0D%0A%0D%0A%23+ask+for%3A%0D%0A%23+ra%2C+dec%2C+%26pmRa%2C+%26pmDec%2C+spType%2C+objTypes%2C+mainId%0D%0A%0D%0Aformat+object+form1+%22%25COO%28A%29%3B%25COO%28D%29%3B%25PM%28A%3BD%29%3B%25SP%28S%29%3B%25OTYPELIST%3B%25MAIN_ID%22%0D%0Aquery+id+");
    miscDynBufAppendString(&url,encodedName);
    free(encodedName);

    /* printf("querying simbad ... (%s)\n",miscDynBufGetBuffer(&url)); */

    if (miscPerformHttpGet(miscDynBufGetBuffer(&url), &result, 0) == mcsFAILURE){
        errCloseStack();
        return mcsFAILURE;
    }

    char * response = miscDynBufGetBuffer(&result);
    /* printf(":\n%s", response ); */

    /* fails if sim-script outputs :error:::: */
    if ( IS_NOT_NULL( strstr(response, ":error:") ) ){
        errCloseStack();
        return mcsFAILURE;
    }

    char *token;
    int fieldIndex = 0;

    token = strtok(response, ";");
    while (token != NULL) {
        switch (fieldIndex) {
            case 0:
                strcpy(ra, token);
                break;
            case 1:
                strcpy(dec, token);
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
                strcpy(objTypes, token);
                break;
            case 6:
                strcpy(mainId, token);
                break;
            default:
                errCloseStack();
                miscDynBufDestroy(&result);

                return mcsFAILURE;
        }
        token = strtok(NULL, ";");
        fieldIndex++;
    }

    miscDynBufDestroy(&result);
    return mcsSUCCESS;
}



/*___oOo___*/
