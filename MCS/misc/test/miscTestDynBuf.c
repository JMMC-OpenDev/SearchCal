/*******************************************************************************
* JMMC project
*
* "@(#) $Id: miscTestDynBuf.c,v 1.4 2004-07-13 14:09:39 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  23-Jun-2004  Created
* lafrasse  09-Jul-2004  Passed some polish
*
*
*******************************************************************************/

static char *rcsId="@(#) $Id: miscTestDynBuf.c,v 1.4 2004-07-13 14:09:39 lafrasse Exp $"; 
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
    
    mcsCOMPL_STAT  execStatus;

	miscDYN_BUF    dynBuf;

    char           *bytes       = NULL;
    int            bytesNumber  = 0;

    char           byte         = '\0';

    int            position     = 0;
    int            from         = 0;
    int            to           = 0;
    
    /* miscDynBufInit */
    printf("---------------------------------------------------------------\n");
    printf("&dynBuf = NULL :\n");
    printf("----------------\n");
	displayDynBuf(NULL);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    printf("dynBuf Allocated :\n");
    printf("------------------\n");
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");



    /* miscDynBufAppendBytes */
    printf("---------------------------------------------------------------\n");
    printf("&dynBuf = NULL :\n");
    printf("----------------\n");
    printf("miscDynBufAppendBytes(&dynBuf = NULL) : ");
    execStatus = miscDynBufAppendBytes(NULL, bytes, bytesNumber);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    printf("dynBuf Allocated :\n");
    printf("------------------\n");
    printf("miscDynBufAppendBytes(NULL, 0) ");
    execStatus = miscDynBufAppendBytes(&dynBuf, bytes, bytesNumber);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    bytes = "hello dynBuf\0" ;
    bytesNumber = strlen(bytes);
    printf("miscDynBufAppendBytes(\"%s\", %d) ", bytes, bytesNumber);
    execStatus = miscDynBufAppendBytes(&dynBuf, bytes, bytesNumber);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
	displayDynBuf(&dynBuf);
    printf("\n");

    bytes    = " ... :)\0" ;
    bytesNumber = strlen(bytes);
    printf("miscDynBufAppendBytes(\"%s\", %d) ", bytes, bytesNumber);
    execStatus = miscDynBufAppendBytes(&dynBuf, bytes, bytesNumber);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
	displayDynBuf(&dynBuf);
    printf("\n");

    bytes    = " !!!\0" ;
    bytesNumber = strlen(bytes);
    printf("miscDynBufAppendBytes(\"%s\", %d) ", bytes, bytesNumber);
    execStatus = miscDynBufAppendBytes(&dynBuf, bytes, bytesNumber);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
	displayDynBuf(&dynBuf);
    printf("\n");



    /* miscDynBufGetByteAt */
    printf("---------------------------------------------------------------\n");
    position = miscDYN_BUF_BEGINNING_POSITION - 1;
    printf("&dynBuf = NULL :\n");
    printf("----------------\n");
    execStatus = miscDynBufGetByteAt(NULL, NULL, position);
    printf("miscDynBufGetByteAt(NULL, %d) ", position);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    printf("dynBuf Allocated :\n");
    printf("------------------\n");
    execStatus = miscDynBufGetByteAt(&dynBuf, NULL, position);
    printf("miscDynBufGetByteAt(NULL, %d) ", position);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    execStatus = miscDynBufGetByteAt(&dynBuf, &byte, position);
    printf("miscDynBufGetByteAt(%d) ", position);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    position = miscDYN_BUF_BEGINNING_POSITION;
    execStatus = miscDynBufGetByteAt(&dynBuf, &byte, position);
    printf("miscDynBufGetByteAt(%d) = \"%c\" ", position, byte);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    position = 7;
    execStatus = miscDynBufGetByteAt(&dynBuf, &byte, position);
    printf("miscDynBufGetByteAt(%d) = \"%c\" ", position, byte);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    position = miscDynBufGetStoredBytesNumber(&dynBuf);
    execStatus = miscDynBufGetByteAt(&dynBuf, &byte, position);
    printf("miscDynBufGetByteAt(%d) = \"%c\" ", position, byte);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    position = miscDynBufGetStoredBytesNumber(&dynBuf) + 1;
    execStatus = miscDynBufGetByteAt(&dynBuf, &byte, position);
    printf("miscDynBufGetByteAt(%d) ", position);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");



    /* miscDynBufGetBytesFromTo */
    printf("---------------------------------------------------------------\n");
    from = miscDYN_BUF_BEGINNING_POSITION - 1;
    to = 4;
    bytes = calloc(sizeof(char),
                          miscDynBufGetAllocatedBytesNumber(&dynBuf) + 1);
    bytes[0] = '\0';
    printf("&dynBuf = NULL :\n");
    printf("----------------\n");
    execStatus = miscDynBufGetBytesFromTo(NULL, &bytes, from, to);
    printf("miscDynBufGetBytesFromTo(%d, %d) ", from, to);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    printf("dynBuf Allocated :\n");
    printf("------------------\n");
    execStatus = miscDynBufGetBytesFromTo(&dynBuf, &bytes, from, to);
    printf("miscDynBufGetBytesFromTo(%d, %d) ", from, to);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    from = miscDYN_BUF_BEGINNING_POSITION;
    to = 12;
    execStatus = miscDynBufGetBytesFromTo(&dynBuf, NULL, from, to);
    printf("miscDynBufGetBytesFromTo(NULL, %d, %d) ", from, to);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    execStatus = miscDynBufGetBytesFromTo(&dynBuf, &bytes, from, to);
    bytes[(to - from) + 1] = '\0';
    printf("miscDynBufGetBytesFromTo(%d, %d) = \"%s\" ", from, to, bytes);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    bytes[0] = '\0';
    printf("\n");

    from = 7;
    to = 16;
    bytes[(to - from) + 1] = '\0';
    execStatus = miscDynBufGetBytesFromTo(&dynBuf, &bytes, from, to);
    printf("miscDynBufGetBytesFromTo(%d, %d) = \"%s\" ", from, to, bytes);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    bytes[0] = '\0';
    printf("\n");

    from = 18;
    to = miscDynBufGetStoredBytesNumber(&dynBuf);
    execStatus = miscDynBufGetBytesFromTo(&dynBuf, &bytes, to, from);
    printf("miscDynBufGetBytesFromTo(%d, %d) ", to, from);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    bytes[0] = '\0';
    printf("\n");

    execStatus = miscDynBufGetBytesFromTo(&dynBuf, &bytes, from, to);
    bytes[(to - from) + 1] = '\0';
    printf("miscDynBufGetBytesFromTo(%d, %d) = \"%s\" ", from, to, bytes);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    bytes[0] = '\0';
    printf("\n");

    to = miscDynBufGetStoredBytesNumber(&dynBuf) + 1;
    execStatus = miscDynBufGetBytesFromTo(&dynBuf, &bytes, from, to);
    printf("miscDynBufGetBytesFromTo(%d, %d) ", from, to);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    free(bytes);
    printf("\n");



    /* miscDynBufReplaceByteAt */
    printf("---------------------------------------------------------------\n");
    position = miscDYN_BUF_BEGINNING_POSITION - 1;
    byte = 'H';
    printf("&dynBuf = NULL :\n");
    printf("----------------\n");
    execStatus = miscDynBufReplaceByteAt(NULL, byte, position);
    printf("miscDynBufReplaceByteAt(%d) ", position);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    printf("dynBuf Allocated :\n");
    printf("------------------\n");
    execStatus = miscDynBufReplaceByteAt(&dynBuf, byte, position);
    printf("miscDynBufReplaceByteAt(%d) ", position);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    position = miscDYN_BUF_BEGINNING_POSITION;
    execStatus = miscDynBufReplaceByteAt(&dynBuf, byte, position);
    printf("miscDynBufReplaceByteAt(%d) = \"%c\" ", position, byte);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    position = 7;
    byte = 'D';
    execStatus = miscDynBufReplaceByteAt(&dynBuf, byte, position);
    printf("miscDynBufReplaceByteAt(%d) = \"%c\" ", position, byte);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    position = 13;
    byte = '\'';
    execStatus = miscDynBufReplaceByteAt(&dynBuf, byte, position);
    printf("miscDynBufReplaceByteAt(%d) = \"%c\" ", position, byte);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    position = miscDynBufGetStoredBytesNumber(&dynBuf);
    byte = '@';
    execStatus = miscDynBufReplaceByteAt(&dynBuf, byte, position);
    printf("miscDynBufReplaceByteAt(%d) = \"%c\" ", position, byte);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    position = miscDynBufGetStoredBytesNumber(&dynBuf) + 1;
    execStatus = miscDynBufReplaceByteAt(&dynBuf, byte, position);
    printf("miscDynBufReplaceByteAt(%d) ", position);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");



    /* miscDynBufInsertBytesAt */
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
    position = miscDYN_BUF_BEGINNING_POSITION - 1;
    execStatus = miscDynBufInsertBytesAt(&dynBuf, NULL, 0, position);
    printf("miscDynBufInsertBytesAt(NULL, 0, %d) ", position);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    position = miscDYN_BUF_BEGINNING_POSITION;
    execStatus = miscDynBufInsertBytesAt(&dynBuf, NULL, 0, position);
    printf("miscDynBufInsertBytesAt(NULL, 0, %d) ", position);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    bytes = "Encore un '\0";
    bytesNumber = 0;
    printf("miscDynBufInsertBytesAt(\"%s\", %d, %d) ", bytes, bytesNumber,
           position);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    bytesNumber = strlen(bytes);
    execStatus = miscDynBufInsertBytesAt(&dynBuf, bytes, bytesNumber, position);
    printf("miscDynBufInsertBytesAt(\"%s\", %d, %d) ", bytes, bytesNumber,
           position);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    position = 18;
    bytes = "misc\0";
    bytesNumber = 0;
    execStatus = miscDynBufInsertBytesAt(&dynBuf, bytes, bytesNumber, position);
    printf("miscDynBufInsertBytesAt(\"%s\", %d, %d) ", bytes, bytesNumber,
           position);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    bytesNumber = strlen(bytes);
    execStatus = miscDynBufInsertBytesAt(&dynBuf, bytes, bytesNumber, position);
    printf("miscDynBufInsertBytesAt(\"%s\", %d, %d) ", bytes, bytesNumber,
           position);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    position = miscDynBufGetStoredBytesNumber(&dynBuf);
    bytes = "~~~\0";
    bytesNumber = 0;
    execStatus = miscDynBufInsertBytesAt(&dynBuf, bytes, bytesNumber, position);
    printf("miscDynBufInsertBytesAt(\"%s\", %d, %d) ", bytes, 0,
           position);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    bytesNumber = strlen(bytes);
    execStatus = miscDynBufInsertBytesAt(&dynBuf, bytes, bytesNumber, position);
    printf("miscDynBufInsertBytesAt(\"%s\", %d, %d) ", bytes, bytesNumber,
           position);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    position = miscDynBufGetStoredBytesNumber(&dynBuf) + 1;
    execStatus = miscDynBufInsertBytesAt(&dynBuf, bytes, bytesNumber, position);
    printf("miscDynBufInsertBytesAt(\"%s\", %d, %d) ", bytes, bytesNumber,
           position);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");



    /* miscDynBufReplaceBytesFromTo */
    printf("---------------------------------------------------------------\n");
    from = miscDYN_BUF_BEGINNING_POSITION - 1;
    to = 9;
    bytes = NULL;
    bytesNumber = 0;
    printf("&dynBuf = NULL :\n");
    printf("----------------\n");
    printf("miscDynBufReplaceBytesFromTo(\"%s\", %d, %d, %d) ", bytes,
           bytesNumber, from, to);
    execStatus = miscDynBufReplaceBytesFromTo(NULL, bytes, bytesNumber, from,
                                              to);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    printf("dynBuf Allocated :\n");
    printf("------------------\n");
    printf("miscDynBufReplaceBytesFromTo(\"%s\", %d, %d, %d) ", bytes,
           bytesNumber, from, to);
    execStatus = miscDynBufReplaceBytesFromTo(&dynBuf, bytes, bytesNumber, from,
                                              to);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    from = miscDYN_BUF_BEGINNING_POSITION;
    bytes = "Toujours ce\0";
    bytesNumber = strlen(bytes);
    printf("miscDynBufReplaceBytesFromTo(\"%s\", %d, %d, %d) ", bytes,
           bytesNumber, from, to);
    execStatus = miscDynBufReplaceBytesFromTo(&dynBuf, bytes, bytesNumber, from, 
                                              to);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    from = 40;
    to = 42;
    bytes = " !\0";
    bytesNumber = 0;
    printf("miscDynBufReplaceBytesFromTo(\"%s\\0\", %d, %d, %d) ", bytes,
           bytesNumber, to, from);
    execStatus = miscDynBufReplaceBytesFromTo(&dynBuf, bytes, bytesNumber, to,
                                              from);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    printf("miscDynBufReplaceBytesFromTo(\"%s\\0\", %d, %d, %d) ", bytes,
           bytesNumber, from, to);
    execStatus = miscDynBufReplaceBytesFromTo(&dynBuf, bytes, bytesNumber, from, 
                                              to);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    from = 31;
    to = miscDynBufGetStoredBytesNumber(&dynBuf);
    bytesNumber = strlen(bytes) + 1;
    printf("miscDynBufReplaceBytesFromTo(\"%s\\0\", %d, %d, %d) ", bytes,
           bytesNumber, from, to);
    execStatus = miscDynBufReplaceBytesFromTo(&dynBuf, bytes, bytesNumber, from, 
                                              to);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    from = 31;
    to = miscDynBufGetStoredBytesNumber(&dynBuf) + 1;
    printf("miscDynBufReplaceBytesFromTo(\"%s\\0\", %d, %d, %d) ", bytes,
           bytesNumber, from, to);
    execStatus = miscDynBufReplaceBytesFromTo(&dynBuf, bytes, bytesNumber, from, 
                                              to);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    /* miscDynBufDeleteBytesFromTo */
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

    /* miscDynBufStrip */
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
    
    /* miscDynBufReset */
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
    
    /* miscDynBufDestroy */
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
