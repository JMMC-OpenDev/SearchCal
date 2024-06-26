/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Test program for hash table management functions.
 */


/* 
 * System Headers 
 */
#include <stdlib.h>
#include <stdio.h>


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "err.h"
#include "log.h"


/*
 * Local Headers 
 */
#include "misc.h"


/*
 * Local Variables
 */

 

/* 
 * Signal catching functions  
 */



/* 
 * Main
 */
char *data[] = 
{   
    "alpha", "bravo", "charlie", "delta", "echo", "foxtrot", "golf", "hotel",
    "india", "juliet", "kilo", "lima", "mike", "november", "oscar", "papa",
    "quebec", "romeo", "sierra", "tango", "uniform", "victor", "whisky",
    "x-ray", "yankee", "zulu"
};

int main (int argc, char *argv[])
{
    /* Configure logging service */
    logSetStdoutLogLevel(logTEST);
    logSetPrintDate(mcsFALSE);
    logSetPrintFileLine(mcsFALSE);

    miscHASH_TABLE hashTable;
    char *dataPtr;
    /* Initializes MCS services */
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        /* Error handling if necessary */

        /* Exit from the application with mcsFAILURE */
        exit (EXIT_FAILURE);
    }

    miscHashCreate(&hashTable, 100);

    /* Test of miscGetHashValue function */
    printf("\nmiscHashAddElement() Function Test :\n");
    int i;
    for (i = 0; i < 24; i++)
    {
        printf("   %-10s added\n", data[i]);
        dataPtr = miscDuplicateString(data[i]);
        if (miscHashAddElement(&hashTable, data[i],
                               (void **)&dataPtr, mcsTRUE)== mcsFAILURE)
        {
            errCloseStack();
            exit (EXIT_FAILURE);
        }
    }
    /* Test of miscHashDisplay function */
    printf("\nmiscHashDisplay() Function Test :\n");
    miscHashDisplay (&hashTable);

    /* Test of miscHashGetElement function */
    printf("\nmiscHashGetElement() Function Test :\n");
    printf("  key = %s - data = %s\n", data[3], 
           (char *)miscHashGetElement(&hashTable, data[3]));

    /* Test of miscHashDeleteElement function */
    printf("\nmiscHashDeleteElement() Function Test :\n");
    printf("  %-10s deleted\n", data[3]); 
    if (miscHashDeleteElement(&hashTable, data[3])== mcsFAILURE)
    {
        errCloseStack();
        exit (EXIT_FAILURE);
    }


    /* Test of miscHashDisplay function */
    printf("\nmiscHashDisplay() Function Test :\n");
    miscHashDisplay (&hashTable);

    /* Test of miscHashGetNextElement function */
    printf("\nmiscHashGetNextElement() Function Test :\n");
    dataPtr = miscHashGetNextElement(&hashTable, mcsTRUE);
    while (dataPtr != NULL)
    {
        printf("  %-10s\n", dataPtr); 
        dataPtr = miscHashGetNextElement(&hashTable, mcsFALSE);
    }


    miscHashDelete(&hashTable);

    /* Close MCS services */
    mcsExit();
    
    /* Exit from the application with mcsSUCCESS */
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
