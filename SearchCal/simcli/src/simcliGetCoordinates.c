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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

/*
 * MCS Headers
 */
#include "mcs.h"
#include "err.h"
#include "log.h"
#include "miscNetwork.h"
#include "thrd.h"


/*
 * Local Headers
 */
#include "simcli.h"
#include "simcliPrivate.h"


/* trace flag */
#define TRACE               0
#define TRACE_RATE_LIMIT    0

/* Max request rate (nb / sec) */
#define MAX_RATE        10
#define MIN_INTERVAL    (1000 / MAX_RATE)

/** simbad error block separator */
#define MARKER_ERROR    "::error"
/** simbad data block separator */
#define MARKER_DATA     "::data"

#define MAX_ERROR_LEN   (16 * 1024)

/**
 * Shared mutex to handle max requests to simbad
 */
thrdMUTEX simcliMutex = MCS_MUTEX_STATIC_INITIALIZER;

#define SIMCLI_LOCK() {                            \
    if (thrdMutexLock(&simcliMutex) == mcsFAILURE) \
    {                                              \
        return mcsFAILURE;                         \
    }                                              \
}

#define SIMCLI_UNLOCK() {                            \
    if (thrdMutexUnlock(&simcliMutex) == mcsFAILURE) \
    {                                                \
        return mcsFAILURE;                           \
    }                                                \
}

/*
 * Public functions definition
 */
mcsCOMPL_STAT simcliGetCoordinates(char *name,
                                   char *ra, char *dec,
                                   mcsDOUBLE *pmRa, mcsDOUBLE *pmDec,
                                   mcsDOUBLE *plx, mcsDOUBLE *ePlx,
                                   mcsDOUBLE *magV, mcsDOUBLE *eMagV,
                                   char *spType, char *objTypes,
                                   char *mainId)
{
    miscDYN_BUF url;
    miscDYN_BUF result;
    FAIL(miscDynBufInit(&url));
    FAIL(miscDynBufInit(&result));

    /* reset outputs */
    strncpy(ra, "\0", mcsLEN32 - 1);
    strncpy(dec, "\0", mcsLEN32 - 1);
    *pmRa = NAN;
    *pmDec = NAN;
    *plx = NAN;
    *ePlx = NAN;
    *magV = NAN;
    *eMagV = NAN;
    strncpy(spType, "\0", mcsLEN64 - 1);
    strncpy(objTypes, "\0", mcsLEN256 - 1);
    strncpy(mainId, "\0",  mcsLEN64 - 1);

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

    FAIL(miscDynBufAppendString(&url, "http://simbad.cds.unistra.fr/simbad/sim-script?script="));

    char* script = miscUrlEncode("output console=off script=off\n"
                                 "format object form1 \""
                                 "%COO(d;A);%COO(d;D);"  /* 0-1: RA and DEC coordinates as sexagesimal values */
                                 "%PM(A;D);"             /* 2-3: Proper motion with error */
                                 "%PLX(V;E);"            /* 4-5: Parallax with error */
                                 "%FLUXLIST(V;n=F E,) ;"  /* 6: Fluxes(V only) in 'Band=Value Error' format (extra space to avoid missing field ';;' => ';') */
                                 "%SP(S);"               /* 7: Spectral types enumeration */
                                 "%OTYPELIST;"           /* 8: Object types enumeration */
                                 "%MAIN_ID;"             /* 9: Main identifier (display) */
                                 "\"\n"
                                 "query id ");
    FAIL(miscDynBufAppendString(&url, script));
    free(script);

    char* encodedName = miscUrlEncode(starName);
    FAIL(miscDynBufAppendString(&url, encodedName));
    free(encodedName);

    if (TRACE) printf("querying SIMBAD ... (%s)\n", miscDynBufGetBuffer(&url));

    /* Call simbad but check rate limiter ~ 10 query/second */
    static long lastTimeMs = 0L;
    long timeMs = 0L;
    struct timeval time;

    /* may wait and block lock for other queries */
    SIMCLI_LOCK();

    /* Get local time */
    gettimeofday(&time, NULL);
    timeMs = time.tv_sec * 1000L + time.tv_usec / 1000L;

    if (lastTimeMs != 0L)
    {
        long deltaMs = timeMs - lastTimeMs;
        if (TRACE_RATE_LIMIT) printf("Rate limiter: deltaMs: %ld ms\n", deltaMs);

        if (deltaMs < MIN_INTERVAL)
        {
            long timeToSleep = (MIN_INTERVAL - deltaMs);

            if (TRACE_RATE_LIMIT) printf("Rate limiter: sleep: %ld ms\n", timeToSleep);
            usleep(timeToSleep * 1000L);
        }
    }
    lastTimeMs = timeMs;

    SIMCLI_UNLOCK();

    /** TODO: retry (3) */

    mcsINT8 executionStatus = miscPerformHttpGet(miscDynBufGetBuffer(&url), &result, 0);

    miscDynBufDestroy(&url);

    if (executionStatus != 0)
    {
        errCloseStack();
        miscDynBufDestroy(&result);
        return mcsFAILURE;
    }

    char* response = miscDynBufGetBuffer(&result);
    logDebug("SIMBAD Response:\n%s\n---\n", response);

    /* If there was an error during query */
    char* posStart = strstr(response, MARKER_ERROR);
    if (posStart != NULL)
    {
        /* sample error (name not found): */
        /*
         ::error:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

         [3] java.text.ParseException: Unrecognized identifier: aasioi
         [5] java.text.ParseException: Unrecognized identifier: bad
         [6] Identifier not found in the database : NAME TEST

         ::data::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
         XXXXX;XXXX;XXX...
         */
        char* responseEnd = response + strlen(response);

        /* try to get error message: */
        /* posEnd points to '::data ...' */
        char* posEnd = strstr(posStart, MARKER_DATA);

        if (posEnd == NULL)
        {
            posEnd = responseEnd;
        }

        int len = posEnd - posStart;
        if (len > MAX_ERROR_LEN)
        {
            len = MAX_ERROR_LEN;
        }

        char substr[len];
        strncpy(substr, posStart, len);

        logInfo("CDS SIMBAD error:\n%s\n", substr);

        /* try to get data block: */
        if (posEnd == responseEnd)
        {
            miscDynBufDestroy(&result);
            return mcsFAILURE;
        }

        posStart = strstr(posEnd, "\n");
        if (posStart == NULL)
        {
            miscDynBufDestroy(&result);
            return mcsFAILURE;
        }
        /* fix data stream: */
        response = posStart;
    }

    /* Parsing result: */
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
                case 0: /* RA */
                    mcsDOUBLE ss;
                    sscanf(token, "%lf", &ss);
                    ss = ss / 15.0;
                    int HH = ss;
                    int MM = ((ss - HH)*60.0);
                    mcsDOUBLE SS = (ss - HH)*3600 - (MM * 60);
                    /* 32 chars should be enough: */
                    snprintf(ra, mcsLEN32 - 1, "%02d %02d %010.7lf", HH, MM, SS);
                    break;
                case 1: /* DE */
                    mcsDOUBLE ds;
                    sscanf(token, "%lf", &ds);
                    char sign = (ds < 0.0) ? '-' : '+';
                    ds = (ds < 0.0) ? -ds : ds;
                    int DH = ds;
                    int DM = ((ds - DH)*60.0);
                    mcsDOUBLE DS = (ds - DH)*3600 - (DM * 60);
                    /* 32 chars should be enough: */
                    snprintf(dec, mcsLEN32 - 1, "%c%d %02d %09.6f", sign, DH, DM, DS);
                    break;
                case 2: /* PMRA */
                    *pmRa = atof(token);
                    break;
                case 3: /* PMDE */
                    *pmDec = atof(token);
                    break;
                case 4: /* PLX */
                    *plx = atof(token);
                    break;
                case 5: /* E_PLX */
                    *ePlx = atof(token);
                    break;
                case 6: /* V=Value Error,... */
                    if (token[0] == 'V')
                    {
                        sscanf(token + 2, "%lf %lf", magV, eMagV);
                    }
                    break;
                case 7: /* SP_TYPE */
                    /* 64 chars should be enough: */
                    strncpy(spType, token, mcsLEN64 - 1);
                    break;
                case 8: /* OBJ_TYPES */
                     /* 256 chars should be enough: */
                    strncpy(objTypes, ",", mcsLEN256 - 1);
                    strncat(objTypes, token, mcsLEN256 - 1);
                    strncat(objTypes, ",", mcsLEN256 - 1);
                    break;
                case 9: /* MAIN_ID */
                    /* 64 chars should be enough: */
                    
                    /* trim space character (left/right) */
                    /* get the first token */
                    char *tok = strtok(token, " ");

                    /* walk through other tokens */
                    while (tok != NULL)
                    {
                        if (strlen(mainId) != 0)
                        {
                            strncat(mainId, " ", mcsLEN64 - 1);
                        }
                        strncat(mainId, tok, mcsLEN64 - 1);

                        tok = strtok(NULL, " ");
                    }
                    break;
                default:
                    miscDynBufDestroy(&result);
                    return mcsSUCCESS;
            }
        }
        token = strtok(NULL, ";");
        fieldIndex++;

        if ((token != NULL) && (token[0] == '\n') && (fieldIndex != 0))
        {
            logError("SIMBAD Response may have several entries:\n%s\n---\n", response);
            miscDynBufDestroy(&result);
            return mcsFAILURE;
        }
    }
    miscDynBufDestroy(&result);
    return mcsSUCCESS;
}
/*___oOo___*/
