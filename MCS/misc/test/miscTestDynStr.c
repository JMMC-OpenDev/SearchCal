/*******************************************************************************
* JMMC project
*
* "@(#) $Id: miscTestDynStr.c,v 1.2 2004-07-20 14:00:06 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  13-Jul-2004  Created
* lafrasse  20-Jul-2004  Passed some polish
*
*
*******************************************************************************/

static char *rcsId="@(#) $Id: miscTestDynStr.c,v 1.2 2004-07-20 14:00:06 lafrasse Exp $"; 
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
#include "miscDynStr.h"


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

    char           *string      = NULL;

    char           chr          = '\0';

    int            position     = 0;
    int            from         = 0;
    int            to           = 0;
    
    /* miscDynStrAppendString */
    printf("---------------------------------------------------------------\n");
    printf("&dynBuf = NULL :\n");
    printf("----------------\n");
    printf("miscDynStrAppendString(&dynBuf = NULL) : ");
    execStatus = miscDynStrAppendString(NULL, string);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    printf("dynBuf Allocated :\n");
    printf("------------------\n");
    printf("miscDynStrAppendString(NULL) ");
    execStatus = miscDynStrAppendString(&dynBuf, string);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    string = "hello dynStr" ;
    printf("miscDynStrAppendString(\"%s\") ", string);
    execStatus = miscDynStrAppendString(&dynBuf, string);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
	displayDynBuf(&dynBuf);
    printf("\n");

    string    = " ... :)" ;
    printf("miscDynStrAppendString(\"%s\") ", string);
    execStatus = miscDynStrAppendString(&dynBuf, string);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
	displayDynBuf(&dynBuf);
    printf("\n");

    string    = " !!!" ;
    printf("miscDynStrAppendString(\"%s\") ", string);
    execStatus = miscDynStrAppendString(&dynBuf, string);
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

    execStatus = miscDynBufGetByteAt(&dynBuf, &chr, position);
    printf("miscDynBufGetByteAt(%d) ", position);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    position = miscDYN_BUF_BEGINNING_POSITION;
    execStatus = miscDynBufGetByteAt(&dynBuf, &chr, position);
    printf("miscDynBufGetByteAt(%d) = \"%c\" ", position, chr);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    position = 7;
    execStatus = miscDynBufGetByteAt(&dynBuf, &chr, position);
    printf("miscDynBufGetByteAt(%d) = \"%c\" ", position, chr);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    position = miscDynBufGetStoredBytesNumber(&dynBuf) - 1;
    execStatus = miscDynBufGetByteAt(&dynBuf, &chr, position);
    printf("miscDynBufGetByteAt(%d) = \"%c\" ", position, chr);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    position = miscDynBufGetStoredBytesNumber(&dynBuf) + 1;
    execStatus = miscDynBufGetByteAt(&dynBuf, &chr, position);
    printf("miscDynBufGetByteAt(%d) ", position);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");



    /* miscDynBufGetBytesFromTo */
    printf("---------------------------------------------------------------\n");
    from = miscDYN_BUF_BEGINNING_POSITION - 1;
    to = 4;
    string = calloc(sizeof(char), miscDynBufGetAllocatedBytesNumber(&dynBuf)+1);
    string[0] = '\0';
    printf("&dynBuf = NULL :\n");
    printf("----------------\n");
    execStatus = miscDynBufGetBytesFromTo(NULL, &string, from, to);
    printf("miscDynBufGetBytesFromTo(%d, %d) ", from, to);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    printf("dynBuf Allocated :\n");
    printf("------------------\n");
    execStatus = miscDynBufGetBytesFromTo(&dynBuf, &string, from, to);
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

    execStatus = miscDynBufGetBytesFromTo(&dynBuf, &string, from, to);
    string[(to - from) + 1] = '\0';
    printf("miscDynBufGetBytesFromTo(%d, %d) = \"%s\" ", from, to, string);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    string[0] = '\0';
    printf("\n");

    from = 7;
    to = 16;
    string[(to - from) + 1] = '\0';
    execStatus = miscDynBufGetBytesFromTo(&dynBuf, &string, from, to);
    printf("miscDynBufGetBytesFromTo(%d, %d) = \"%s\" ", from, to, string);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    string[0] = '\0';
    printf("\n");

    from = 18;
    to = miscDynBufGetStoredBytesNumber(&dynBuf) - 1;
    execStatus = miscDynBufGetBytesFromTo(&dynBuf, &string, to, from);
    printf("miscDynBufGetBytesFromTo(%d, %d) ", to, from);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    string[0] = '\0';
    printf("\n");

    execStatus = miscDynBufGetBytesFromTo(&dynBuf, &string, from, to);
    string[(to - from) + 1] = '\0';
    printf("miscDynBufGetBytesFromTo(%d, %d) = \"%s\" ", from, to, string);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    string[0] = '\0';
    printf("\n");

    to = miscDynBufGetStoredBytesNumber(&dynBuf) + 1;
    execStatus = miscDynBufGetBytesFromTo(&dynBuf, &string, from, to);
    printf("miscDynBufGetBytesFromTo(%d, %d) ", from, to);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    free(string);
    printf("\n");



    /* miscDynBufReplaceByteAt */
    printf("---------------------------------------------------------------\n");
    position = miscDYN_BUF_BEGINNING_POSITION - 1;
    chr = 'H';
    printf("&dynBuf = NULL :\n");
    printf("----------------\n");
    execStatus = miscDynBufReplaceByteAt(NULL, chr, position);
    printf("miscDynBufReplaceByteAt(%d) ", position);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    printf("dynBuf Allocated :\n");
    printf("------------------\n");
    execStatus = miscDynBufReplaceByteAt(&dynBuf, chr, position);
    printf("miscDynBufReplaceByteAt(%d) ", position);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    position = miscDYN_BUF_BEGINNING_POSITION;
    execStatus = miscDynBufReplaceByteAt(&dynBuf, chr, position);
    printf("miscDynBufReplaceByteAt(%d) = \"%c\" ", position, chr);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    position = 7;
    chr = 'D';
    execStatus = miscDynBufReplaceByteAt(&dynBuf, chr, position);
    printf("miscDynBufReplaceByteAt(%d) = \"%c\" ", position, chr);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    position = 13;
    chr = '\'';
    execStatus = miscDynBufReplaceByteAt(&dynBuf, chr, position);
    printf("miscDynBufReplaceByteAt(%d) = \"%c\" ", position, chr);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    position = miscDynBufGetStoredBytesNumber(&dynBuf) - 1;
    chr = '@';
    execStatus = miscDynBufReplaceByteAt(&dynBuf, chr, position);
    printf("miscDynBufReplaceByteAt(%d) = \"%c\" ", position, chr);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    position = miscDynBufGetStoredBytesNumber(&dynBuf) + 1;
    execStatus = miscDynBufReplaceByteAt(&dynBuf, chr, position);
    printf("miscDynBufReplaceByteAt(%d) ", position);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");



    /* miscDynStrInsertStringAt */
    printf("---------------------------------------------------------------\n");
    printf("&dynBuf = NULL :\n");
    printf("----------------\n");
    execStatus = miscDynStrInsertStringAt(NULL, NULL, 0);
    printf("miscDynStrInsertStringAt(NULL, 0) ");
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    printf("dynBuf Allocated :\n");
    printf("------------------\n");
    position = miscDYN_BUF_BEGINNING_POSITION - 1;
    execStatus = miscDynStrInsertStringAt(&dynBuf, NULL, position);
    printf("miscDynStrInsertStringAt(NULL, %d) ", position);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    position = miscDYN_BUF_BEGINNING_POSITION;
    execStatus = miscDynStrInsertStringAt(&dynBuf, NULL, position);
    printf("miscDynStrInsertStringAt(NULL, %d) ", position);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    string = "Encore un '";
    execStatus = miscDynStrInsertStringAt(&dynBuf, string, position);
    printf("miscDynStrInsertStringAt(\"%s\", %d) ", string, position);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    position = 18;
    string = "misc";
    execStatus = miscDynStrInsertStringAt(&dynBuf, string, position);
    printf("miscDynStrInsertStringAt(\"%s\", %d) ", string, position);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    position = miscDynBufGetStoredBytesNumber(&dynBuf);
    string = "~~~";
    execStatus = miscDynStrInsertStringAt(&dynBuf, string, position);
    printf("miscDynStrInsertStringAt(\"%s\", %d) ", string, position);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    position = miscDynBufGetStoredBytesNumber(&dynBuf) + 1;
    execStatus = miscDynStrInsertStringAt(&dynBuf, string, position);
    printf("miscDynStrInsertStringAt(\"%s\", %d) ", string, position);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");



    /* miscDynStrReplaceStringFromTo */
    printf("---------------------------------------------------------------\n");
    from = miscDYN_BUF_BEGINNING_POSITION - 1;
    to = 9;
    string = NULL;
    printf("&dynBuf = NULL :\n");
    printf("----------------\n");
    printf("miscDynStrReplaceStringFromTo(\"%s\", %d, %d) ", string, from, to);
    execStatus = miscDynStrReplaceStringFromTo(NULL, string, from, to);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    printf("dynBuf Allocated :\n");
    printf("------------------\n");
    printf("miscDynStrReplaceStringFromTo(\"%s\", %d, %d) ", string, from, to);
    execStatus = miscDynStrReplaceStringFromTo(&dynBuf, string, from, to);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    from = miscDYN_BUF_BEGINNING_POSITION;
    string = "Toujours ce";
    printf("miscDynStrReplaceStringFromTo(\"%s\", %d, %d) ", string, from, to);
    execStatus = miscDynStrReplaceStringFromTo(&dynBuf, string, from, to);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    from = 40;
    to = 42;
    string = " !";
    printf("miscDynStrReplaceStringFromTo(\"%s\", %d, %d) ", string, to, from);
    execStatus = miscDynStrReplaceStringFromTo(&dynBuf, string, to, from);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    printf("miscDynStrReplaceStringFromTo(\"%s\", %d, %d) ", string, from, to);
    execStatus = miscDynStrReplaceStringFromTo(&dynBuf, string, from, to);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    from = 31;
    to = miscDynBufGetStoredBytesNumber(&dynBuf);
    printf("miscDynStrReplaceStringFromTo(\"%s\", %d, %d) ", string, from, to);
    execStatus = miscDynStrReplaceStringFromTo(&dynBuf, string, from, to);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    from = 31;
    to = miscDynBufGetStoredBytesNumber(&dynBuf) + 1;
    printf("miscDynStrReplaceStringFromTo(\"%s\", %d, %d) ", string, from, to);
    execStatus = miscDynStrReplaceStringFromTo(&dynBuf, string, from, to);
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
