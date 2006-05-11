/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: thrdTestSemaphore.c,v 1.2 2006-05-11 13:04:57 mella Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/12/19 16:48:30  lafrasse
 * Added semaphore support
 *
 ******************************************************************************/


static char *rcsId __attribute__ ((unused)) ="@(#) $Id: thrdTestSemaphore.c,v 1.2 2006-05-11 13:04:57 mella Exp $";

/* 
 * System Headers 
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"


/*
 * Local Headers 
 */
#include "thrdThread.h"
#include "thrdSemaphore.h"
#include "thrdPrivate.h"


/*
 * Gloval variables
 */
thrdSEMAPHORE emptyBufferSemaphore;
thrdSEMAPHORE fullBufferSemaphore;
mcsSTRING256  message;
mcsLOGICAL    finished;


/*
 * Local functions
 */
thrdFCT_RET consumerFunction(thrdFCT_ARG param)
{
    do
    {
        logDebug("Consumer waiting for the buffer to be written by producer.");
        /* Wait for a new message to be posted */
        if (thrdSemaphoreWait(fullBufferSemaphore) == mcsFAILURE)
        {
            errCloseStack();
        }
        logDebug("Consumer received the signal that producer wrote buffer.");
        
        if (finished == mcsFALSE)
        {
            logInfo("Consumer read '%s' message.", message);
        }
        else
        {
            logInfo("Consumer ended consumption.");
        }
    
        logDebug("Consumer signals producer that the buffer has been read.");
        /* Signal buffer emptyness */
        if (thrdSemaphoreSignal(emptyBufferSemaphore) == mcsFAILURE)
        {
            errCloseStack();
        }
    }
    while (finished == mcsFALSE);

    return NULL;
}
 
thrdFCT_RET producerFunction(thrdFCT_ARG param)
{
    logDebug("Producer waiting for the buffer to be read by consumer.");
    /* Wait for a new message to be posted */
    if (thrdSemaphoreWait(emptyBufferSemaphore) == mcsFAILURE)
    {
        errCloseStack();
    }
    logDebug("Producer received the signal that consumer read buffer.");
    
    if (param != NULL)
    {
        finished = mcsFALSE;
        strncpy(message, (char*)param, sizeof(message));
        logInfo("Producer wrote '%s' message.", message);
    }
    else
    {
        finished = mcsTRUE;
        logInfo("Producer ended production.");
    }

    logDebug("Producer signals consumer that the buffer has been wrote.");
    /* Signal buffer emptyness */
    if (thrdSemaphoreSignal(fullBufferSemaphore) == mcsFAILURE)
    {
        errCloseStack();
    }

    return NULL;
}
 

/* 
 * Main
 */
int main (int argc, char *argv[])
{
    /* Initializes MCS services */
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        /* Exit from the application with FAILURE */
        exit (EXIT_FAILURE);
    }

    logSetStdoutLogLevel(logINFO);

    /* Semaphores initialisation */
    if (thrdSemaphoreInit(&emptyBufferSemaphore, 1) == mcsFAILURE)
    {
        errCloseStack();
        exit(EXIT_FAILURE);
    }
    if (thrdSemaphoreInit(&fullBufferSemaphore, 0) == mcsFAILURE)
    {
        errCloseStack();
        exit(EXIT_FAILURE);
    }

    /* Thread creation */
    thrdTHREAD           consumer;
    consumer.function  = consumerFunction;
    consumer.parameter = NULL;
    thrdThreadCreate(&consumer);

    /* Write some messages in the shared buffer */
    producerFunction("message 1");
    producerFunction("message 2");
    producerFunction("LAST message");
    producerFunction(NULL);

    /* Wait for the thread end */
    thrdThreadWait(&consumer);

    /* Semaphores destruction */
    if (thrdSemaphoreDestroy(emptyBufferSemaphore) == mcsFAILURE)
    {
        errCloseStack();
        exit(EXIT_FAILURE);
    }
    if (thrdSemaphoreDestroy(fullBufferSemaphore) == mcsFAILURE)
    {
        errCloseStack();
        exit(EXIT_FAILURE);
    }

    /* Close MCS services */
    mcsExit();
    
    /* Exit from the application with SUCCESS */
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
