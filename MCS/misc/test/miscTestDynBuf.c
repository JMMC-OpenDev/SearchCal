/*******************************************************************************
* JMMC project
*
* "@(#) $Id: miscTestDynBuf.c,v 1.3 2004-07-12 10:24:26 gluck Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  23-Jun-2004  Created
* lafrasse  09-Jul-2004  Passed some polish
*
*
*******************************************************************************/

static char *rcsId="@(#) $Id: miscTestDynBuf.c,v 1.3 2004-07-12 10:24:26 gluck Exp $"; 
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
#include "miscDynBuf.h"


/*
 * Local Functionss 
 */
void displayExecStatus(mcsCOMPL_STAT);
void displayDynBuf(miscDYN_BUF*);


/* 
 * Main
 */

#define SUCCEED "SUCCEED"
#define FAILED "FAILED"

int main (int argc, char *argv[])
{
    /* Give process name to mcs library */
    mcsInit(argv[0]);
    
    mcsCOMPL_STAT execStatus;

    printf("---------------------------------------------------------------\n");
    printf("&dynBuf = NULL :\n");
    printf("----------------\n");
	displayDynBuf(NULL);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    printf("dynBuf Allocated :\n");
    printf("------------------\n");
	miscDYN_BUF dynBuf;
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();

    printf("---------------------------------------------------------------\n");
    char *firstStr = NULL;
    printf("&dynBuf = NULL :\n");
    printf("----------------\n");
    printf("miscDynBufAppendBytes(&dynBuf = NULL) : ");
    execStatus = miscDynBufAppendBytes(NULL, firstStr, 0);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    printf("dynBuf Allocated :\n");
    printf("------------------\n");
    printf("miscDynBufAppendBytes(NULL, 0) ");
    execStatus = miscDynBufAppendBytes(&dynBuf, firstStr, 0);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    firstStr = "hello dynBuf" ;
    int firstLength = strlen(firstStr);
    printf("miscDynBufAppendBytes(\"%s\", %d) ", firstStr, firstLength);
    execStatus = miscDynBufAppendBytes(&dynBuf, firstStr, firstLength);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
	displayDynBuf(&dynBuf);
    printf("\n");

    firstStr    = " ... :)" ;
    firstLength = strlen(firstStr);
    printf("miscDynBufAppendBytes(\"%s\", %d) ", firstStr, firstLength);
    execStatus = miscDynBufAppendBytes(&dynBuf, firstStr, firstLength);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
	displayDynBuf(&dynBuf);
    printf("\n");

    firstStr    = " !!!" ;
    firstLength = strlen(firstStr);
    printf("miscDynBufAppendBytes(\"%s\", %d) ", firstStr, firstLength);
    execStatus = miscDynBufAppendBytes(&dynBuf, firstStr, firstLength);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
	displayDynBuf(&dynBuf);

    printf("---------------------------------------------------------------\n");
    int at = miscDYN_BUF_BEGINNING_POSITION - 1;
    char tmp;
    printf("&dynBuf = NULL :\n");
    printf("----------------\n");
    execStatus = miscDynBufGetByteAt(NULL, NULL, at);
    printf("miscDynBufGetByteAt(NULL, %d) ", at);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    printf("dynBuf Allocated :\n");
    printf("------------------\n");
    execStatus = miscDynBufGetByteAt(&dynBuf, NULL, at);
    printf("miscDynBufGetByteAt(NULL, %d) ", at);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    execStatus = miscDynBufGetByteAt(&dynBuf, &tmp, at);
    printf("miscDynBufGetByteAt(%d) ", at);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    at = miscDYN_BUF_BEGINNING_POSITION;
    execStatus = miscDynBufGetByteAt(&dynBuf, &tmp, at);
    printf("miscDynBufGetByteAt(%d) = \"%c\" ", at, tmp);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    at = 7;
    execStatus = miscDynBufGetByteAt(&dynBuf, &tmp, at);
    printf("miscDynBufGetByteAt(%d) = \"%c\" ", at, tmp);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    at = 14;
    execStatus = miscDynBufGetByteAt(&dynBuf, &tmp, at);
    printf("miscDynBufGetByteAt(%d) = \"%c\" ", at, tmp);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    at = miscDynBufGetStoredBytesNumber(&dynBuf) + 1;
    execStatus = miscDynBufGetByteAt(&dynBuf, &tmp, at);
    printf("miscDynBufGetByteAt(%d) ", at);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();

    printf("---------------------------------------------------------------\n");
    int from = miscDYN_BUF_BEGINNING_POSITION - 1;
    int to = 4;
    char *tmpBuf = calloc(sizeof(char),
                          miscDynBufGetAllocatedBytesNumber(&dynBuf) + 1);
    tmpBuf[0] = '\0';
    printf("&dynBuf = NULL :\n");
    printf("----------------\n");
    execStatus = miscDynBufGetBytesFromTo(NULL, &tmpBuf, from, to);
    printf("miscDynBufGetBytesFromTo(%d, %d) ", from, to);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    printf("dynBuf Allocated :\n");
    printf("------------------\n");
    execStatus = miscDynBufGetBytesFromTo(&dynBuf, &tmpBuf, from, to);
    printf("miscDynBufGetBytesFromTo(%d, %d) ", from, to);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    from = miscDYN_BUF_BEGINNING_POSITION;
    to = 5;
    execStatus = miscDynBufGetBytesFromTo(&dynBuf, NULL, from, to);
    printf("miscDynBufGetBytesFromTo(NULL, %d, %d) ", from, to);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    execStatus = miscDynBufGetBytesFromTo(&dynBuf, &tmpBuf, from, to);
    tmpBuf[(to - from) + 1] = '\0';
    printf("miscDynBufGetBytesFromTo(%d, %d) = \"%s\" ", from, to, tmpBuf);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    tmpBuf[0] = '\0';
    printf("\n");

    from = 7;
    to = miscDynBufGetStoredBytesNumber(&dynBuf);
    execStatus = miscDynBufGetBytesFromTo(&dynBuf, &tmpBuf, to, from);
    printf("miscDynBufGetBytesFromTo(%d, %d) ", to, from);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    tmpBuf[0] = '\0';
    printf("\n");

    execStatus = miscDynBufGetBytesFromTo(&dynBuf, &tmpBuf, from, to);
    tmpBuf[(to - from) + 1] = '\0';
    printf("miscDynBufGetBytesFromTo(%d, %d) = \"%s\" ", from, to, tmpBuf);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    tmpBuf[0] = '\0';
    printf("\n");

    to = miscDynBufGetStoredBytesNumber(&dynBuf) + 1;
    execStatus = miscDynBufGetBytesFromTo(&dynBuf, &tmpBuf, from, to);
    printf("miscDynBufGetBytesFromTo(%d, %d) ", from, to);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    free(tmpBuf);

    printf("---------------------------------------------------------------\n");
    at = miscDYN_BUF_BEGINNING_POSITION - 1;
    tmp = 'H';
    printf("&dynBuf = NULL :\n");
    printf("----------------\n");
    execStatus = miscDynBufReplaceByteAt(NULL, tmp, at);
    printf("miscDynBufReplaceByteAt(%d) ", at);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    printf("dynBuf Allocated :\n");
    printf("------------------\n");
    execStatus = miscDynBufReplaceByteAt(&dynBuf, tmp, at);
    printf("miscDynBufReplaceByteAt(%d) ", at);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    at = miscDYN_BUF_BEGINNING_POSITION;
    execStatus = miscDynBufReplaceByteAt(&dynBuf, tmp, at);
    printf("miscDynBufReplaceByteAt(%d) = \"%c\" ", at, tmp);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    at = 7;
    tmp = 'D';
    execStatus = miscDynBufReplaceByteAt(&dynBuf, tmp, at);
    printf("miscDynBufReplaceByteAt(%d) = \"%c\" ", at, tmp);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    at = 13;
    tmp = '\'';
    execStatus = miscDynBufReplaceByteAt(&dynBuf, tmp, at);
    printf("miscDynBufReplaceByteAt(%d) = \"%c\" ", at, tmp);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    at = miscDynBufGetStoredBytesNumber(&dynBuf) + 1;
    execStatus = miscDynBufReplaceByteAt(&dynBuf, tmp, at);
    printf("miscDynBufReplaceByteAt(%d) ", at);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    printf("---------------------------------------------------------------\n");
    printf("&dynBuf = NULL :\n");
    printf("----------------\n");
    execStatus = miscDynBufInsertBytesAt(NULL, NULL, 0, 0);
    printf("miscDynBufInsertBytesAt(NULL, 0, 0) ");
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    printf("dynBuf Allocated :\n");
    printf("------------------\n");
    at = miscDYN_BUF_BEGINNING_POSITION - 1;
    execStatus = miscDynBufInsertBytesAt(&dynBuf, NULL, 0, at);
    printf("miscDynBufInsertBytesAt(NULL, 0, %d) ", at);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    at = miscDYN_BUF_BEGINNING_POSITION;
    execStatus = miscDynBufInsertBytesAt(&dynBuf, NULL, 0, at);
    printf("miscDynBufInsertBytesAt(NULL, 0, %d) ", at);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    firstStr = "Encore un '\0";
    firstLength = 0;
    execStatus = miscDynBufInsertBytesAt(&dynBuf, firstStr, firstLength, at);
    printf("miscDynBufInsertBytesAt(\"%s\", %d, %d) ", firstStr, firstLength,
           at);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    firstLength = strlen(firstStr);
    execStatus = miscDynBufInsertBytesAt(&dynBuf, firstStr, firstLength, at);
    printf("miscDynBufInsertBytesAt(\"%s\", %d, %d) ", firstStr, firstLength,
           at);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    at = 18;
    firstStr = "misc\0";
    firstLength = 0;
    execStatus = miscDynBufInsertBytesAt(&dynBuf, firstStr, firstLength, at);
    printf("miscDynBufInsertBytesAt(\"%s\", %d, %d) ", firstStr, firstLength,
           at);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    firstLength = strlen(firstStr);
    execStatus = miscDynBufInsertBytesAt(&dynBuf, firstStr, firstLength, at);
    printf("miscDynBufInsertBytesAt(\"%s\", %d, %d) ", firstStr, firstLength,
           at);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    at = miscDynBufGetStoredBytesNumber(&dynBuf);
    firstStr = "~~~\0";
    firstLength = strlen(firstStr);
    execStatus = miscDynBufInsertBytesAt(&dynBuf, firstStr, firstLength, at);
    printf("miscDynBufInsertBytesAt(\"%s\", %d, %d) ", firstStr, firstLength,
           at);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    at = miscDynBufGetStoredBytesNumber(&dynBuf) + 1;
    execStatus = miscDynBufInsertBytesAt(&dynBuf, firstStr, firstLength, at);
    printf("miscDynBufInsertBytesAt(\"%s\", %d, %d) ", firstStr, firstLength,
           at);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    printf("---------------------------------------------------------------\n");
    from = miscDYN_BUF_BEGINNING_POSITION - 1;
    to = 9;
    firstStr = "Toujours ce\0";
    firstLength = strlen(firstStr);
    printf("&dynBuf = NULL :\n");
    printf("----------------\n");
    printf("miscDynBufReplaceBytesFromTo(\"%s\", %d, %d, %d) ", firstStr,
           firstLength, from, to);
    execStatus = miscDynBufReplaceBytesFromTo(NULL, firstStr, firstLength, from,
                                              to);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    printf("dynBuf Allocated :\n");
    printf("------------------\n");
    printf("miscDynBufReplaceBytesFromTo(\"%s\", %d, %d, %d) ", firstStr,
           firstLength, from, to);
    execStatus = miscDynBufReplaceBytesFromTo(&dynBuf, firstStr, firstLength,
                                              from, to);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    from = miscDYN_BUF_BEGINNING_POSITION;
    printf("miscDynBufReplaceBytesFromTo(\"%s\", %d, %d, %d) ", firstStr,
           firstLength, from, to);
    execStatus = miscDynBufReplaceBytesFromTo(&dynBuf, firstStr, firstLength,
                                              from, to);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    from = 40;
    to = 42;
    firstStr = " !\0";
    firstLength = 0;
    printf("miscDynBufReplaceBytesFromTo(\"%s\\0\", %d, %d, %d) ", firstStr,
           firstLength, to, from);
    execStatus = miscDynBufReplaceBytesFromTo(&dynBuf, firstStr, firstLength,
                                              to, from);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    firstStr = " !\0";
    firstLength = 0;
    printf("miscDynBufReplaceBytesFromTo(\"%s\\0\", %d, %d, %d) ", firstStr,
           firstLength, from, to);
    execStatus = miscDynBufReplaceBytesFromTo(&dynBuf, firstStr, firstLength,
                                              from, to);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    from = 31;
    to = miscDynBufGetStoredBytesNumber(&dynBuf);
    firstLength = strlen(firstStr) + 1;
    printf("miscDynBufReplaceBytesFromTo(\"%s\\0\", %d, %d, %d) ", firstStr,
           firstLength, from, to);
    execStatus = miscDynBufReplaceBytesFromTo(&dynBuf, firstStr, firstLength,
                                              from, to);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    from = 31;
    to = miscDynBufGetStoredBytesNumber(&dynBuf) + 1;
    firstStr = " !\0";
    firstLength = strlen(firstStr) + 1;
    printf("miscDynBufReplaceBytesFromTo(\"%s\\0\", %d, %d, %d) ", firstStr,
           firstLength, from, to);
    execStatus = miscDynBufReplaceBytesFromTo(&dynBuf, firstStr, firstLength,
                                              from, to);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    printf("---------------------------------------------------------------\n");
    from = miscDYN_BUF_BEGINNING_POSITION - 1;
    to = 13;
    printf("&dynBuf = NULL :\n");
    printf("----------------\n");
    printf("miscDynBufDeleteBytesFromTo(%d, %d) ", from, to);
    execStatus = miscDynBufDeleteBytesFromTo(NULL, from, to);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    printf("dynBuf Allocated :\n");
    printf("------------------\n");
    printf("miscDynBufDeleteBytesFromTo(%d, %d) ", from, to);
    execStatus = miscDynBufDeleteBytesFromTo(&dynBuf, from, to);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    from = miscDYN_BUF_BEGINNING_POSITION;
    printf("miscDynBufDeleteBytesFromTo(%d, %d) ", from, to);
    execStatus = miscDynBufDeleteBytesFromTo(&dynBuf, from, to);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    from = 7;
    to = 10;
    printf("miscDynBufDeleteBytesFromTo(%d, %d) ", to, from);
    execStatus = miscDynBufDeleteBytesFromTo(&dynBuf, to, from);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    printf("miscDynBufDeleteBytesFromTo(%d, %d) ", from, to);
    execStatus = miscDynBufDeleteBytesFromTo(&dynBuf, from, to);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    from = 13;
    to = miscDynBufGetStoredBytesNumber(&dynBuf) - 1;
    printf("miscDynBufDeleteBytesFromTo(%d, %d) ", from, to);
    execStatus = miscDynBufDeleteBytesFromTo(&dynBuf, from, to);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    from = 12;
    to = miscDynBufGetStoredBytesNumber(&dynBuf) + 1;
    printf("miscDynBufDeleteBytesFromTo(%d, %d) ", from, to);
    execStatus = miscDynBufDeleteBytesFromTo(&dynBuf, from, to);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    printf("---------------------------------------------------------------\n");
    printf("&dynBuf = NULL :\n");
    printf("----------------\n");
    printf("miscDynBufStrip() ");
    execStatus = miscDynBufStrip(NULL);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    printf("dynBuf Allocated :\n");
    printf("------------------\n");
    printf("miscDynBufStrip() ");
    execStatus = miscDynBufStrip(&dynBuf);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");
    
    printf("---------------------------------------------------------------\n");
    printf("&dynBuf = NULL :\n");
    printf("----------------\n");
    printf("miscDynBufReset() ");
    execStatus = miscDynBufReset(NULL);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    printf("dynBuf Allocated :\n");
    printf("------------------\n");
    printf("miscDynBufReset() ");
    execStatus = miscDynBufReset(&dynBuf);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");
    
    printf("---------------------------------------------------------------\n");
    printf("&dynBuf = NULL :\n");
    printf("----------------\n");
    printf("miscDynBufDestroy() ");
    execStatus = miscDynBufDestroy(NULL);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    printf("dynBuf Allocated :\n");
    printf("------------------\n");
    printf("miscDynBufDestroy() ");
    execStatus = miscDynBufDestroy(&dynBuf);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");
    
	exit(0);
}


void displayExecStatus(mcsCOMPL_STAT execStatus)
{
	char *errorStr = NULL;

    if (execStatus == FAILURE)
    {
        errorStr = FAILED;
    }
    else
    {
        errorStr = SUCCEED;
    }

    printf("%s\n", errorStr);

    return;
}

void displayDynBuf(miscDYN_BUF *dynBuf)
{
    printf("miscDynBufGetStoredBytesNumber = \"%d\".\n",
            miscDynBufGetStoredBytesNumber(dynBuf));

    printf("miscDynBufGetAllocatedBytesNumber = \"%d\".\n",
            miscDynBufGetAllocatedBytesNumber(dynBuf));

    printf("miscDynBufGetBufferPointer = \"%s\".\n",
            miscDynBufGetBufferPointer(dynBuf));
}

/*___oOo___*/
