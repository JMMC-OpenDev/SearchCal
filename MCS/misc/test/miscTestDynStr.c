/*******************************************************************************
* JMMC project
*
* "@(#) $Id: miscTestDynStr.c,v 1.3 2004-07-23 14:29:59 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  13-Jul-2004  Created
* lafrasse  20-Jul-2004  Passed some polish
* lafrasse  23-Jul-2004  Added error management to
*                        miscDynBufGetStoredBytesNumber and
*                        miscDynBufGetAllocatedBytesNumber, plus
*                        miscDynStrGetStringFromTo parameter refinments.
*
*
*******************************************************************************/

static char *rcsId="@(#) $Id: miscTestDynStr.c,v 1.3 2004-07-23 14:29:59 lafrasse Exp $"; 
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

    mcsUINT32      position     = 0;
    mcsUINT32      from         = 0;
    mcsUINT32      to           = 0;
    
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

    miscDynBufGetStoredBytesNumber(&dynBuf, &position);
    string = "~~~";
    execStatus = miscDynStrInsertStringAt(&dynBuf, string, position);
    printf("miscDynStrInsertStringAt(\"%s\", %d) ", string, position);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    miscDynBufGetStoredBytesNumber(&dynBuf, &position);
    position += 1;
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
    miscDynBufGetStoredBytesNumber(&dynBuf, &to);
    printf("miscDynStrReplaceStringFromTo(\"%s\", %d, %d) ", string, from, to);
    execStatus = miscDynStrReplaceStringFromTo(&dynBuf, string, from, to);
    displayExecStatus(execStatus);
	displayDynBuf(&dynBuf);
    errDisplayStack();
    errCloseStack();
    printf("\n");

    from = 31;
    miscDynBufGetStoredBytesNumber(&dynBuf, &to);
    to += 1;
    printf("miscDynStrReplaceStringFromTo(\"%s\", %d, %d) ", string, from, to);
    execStatus = miscDynStrReplaceStringFromTo(&dynBuf, string, from, to);
    displayExecStatus(execStatus);
    errDisplayStack();
    errCloseStack();
    printf("\n");



    miscDynBufDestroy(&dynBuf);
    
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
    mcsUINT32 bytesNumber = 0;

    printf("miscDynBufGetStoredBytesNumber = ");
    if (miscDynBufGetStoredBytesNumber(dynBuf, &bytesNumber) == FAILURE)
    {
        printf("FAILURE.\n");
    }
    else
    {
        printf("'%d'.\n", bytesNumber);
    }

    printf("miscDynBufGetAllocatedBytesNumber = ");
    if (miscDynBufGetAllocatedBytesNumber(dynBuf, &bytesNumber) == FAILURE)
    {
        printf("FAILURE.\n");
    }
    else
    {
        printf("'%d'.\n", bytesNumber);
    }

    printf("miscDynBufGetBufferPointer = \"%s\".\n",
            miscDynBufGetBufferPointer(dynBuf));
}

/*___oOo___*/
