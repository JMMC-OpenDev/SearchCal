/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrTestSearchCalibratorsClient.cpp,v 1.3 2005-02-15 15:52:00 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * scetre    02-Nov-2004  Created
 *
 ******************************************************************************/

static char *rcsId="@(#) $Id: sclsvrTestSearchCalibratorsClient.cpp,v 1.3 2005-02-15 15:52:00 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers 
 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

/**
 * \namespace std
 * Export standard iostream objects (cin, cout,...).
 */
using namespace std;
#include <unistd.h>
#include <time.h>

/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "msg.h"
#include "gui.h"
/*
 * Local Headers 
 */
#include "sclsvr.h"
#include "sclsvrPrivate.h"


/*
 * Local Variables
 */

 

/* 
 * Signal catching functions  
 */



/* 
 * Main
 */

int main(int argc, char *argv[])
{
    // Initialize MCS services
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        // Error handling if necessary
        
        // Exit from the application with mcsFAILURE
        exit (EXIT_FAILURE);
    }
    guiGUI oneGui;
    oneGui.ConnectToRemoteGui("localhost",6666, mcsGetProcName());
    
    /* Try to connect the msgManager communictaion server */
    if (msgConnect("msgTestClient", NULL) == mcsFAILURE)
    {
        logWarning("msgConnect() failed");
        errDisplayStack();
        errCloseStack();
        exit(EXIT_FAILURE);
    }

    /* Pause for 1 second */
    sleep(1);
    mcsSTRING256 paramList;
    if (argv[1]!=NULL)
    {
        strcpy(paramList, argv[1]);
        int i=2;
        while(argv[i]!=NULL)
        {
            strcat(paramList, " ");
            strcat(paramList, argv[i]);
            i++;
        }
    }
    /* printf("%s\n", paramList); */

    /* Try to send a QUERY command to msgManager */
    if (msgSendCommand("QUERY", "sclsvr", paramList, 0) == mcsFAILURE)
    {
        errDisplayStack();
        errCloseStack();
    }

    /* Try to received message */
    msgMESSAGE message;    
    if (msgReceive(&message, 100000) == mcsFAILURE)
    {
        errDisplayStack();
        errCloseStack();
    }
    else
    {
        printf("MESSAGEBUFFER:\n%s\n", msgGetBodyPtr(&message));
        
        // Build a simple gui
        guiWINDOW window1("Search Calibrators results");
        window1.AttachAGui(&oneGui);

        //with a table
        guiTABLE table1(44,20);
        table1.SetHelp("This table was filled adding rown and column");
        table1.SetLabel("Calibrators Table");
        int r,c;
        char str[32];
        for (c=0;c<20;c++)
        {
            sprintf(str,"Header %d",c);
            table1.SetColumnHeader(c,str);
            for (r=0;r<10;r++)
            {
                sprintf(str,"%d",r+c);
                table1.SetCell(r,c,str);
            }
        }
        window1.Add(&table1);

        // Set a new status message
        string msg("CDS Reply OK");
        oneGui.SetStatus(true, msg);
        window1.Show();
    }

    /* Try to disconnect from msgManager */
    if (msgDisconnect() == mcsFAILURE)
    {
        logWarning("msgDisconnect failed");
        errDisplayStack();
        errCloseStack();
        exit(EXIT_FAILURE);
    }

    // Close MCS services
    mcsExit();
    
    // Exit from the application with mcsSUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
