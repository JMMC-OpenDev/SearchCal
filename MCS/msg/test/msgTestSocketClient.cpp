/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgTestSocketClient.cpp,v 1.2 2004-12-15 15:53:38 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  24-Nov-2004  Created
* lafrasse  14-Dec-2004  Added error code management
*
*
*******************************************************************************/

/**
 * \file
 * Small test client based on msgSOCKET_CLIENT.
 *
 * \synopsis
 * \<msgTestSocketClient\>
 */

static char *rcsId="@(#) $Id: msgTestSocketClient.cpp,v 1.2 2004-12-15 15:53:38 lafrasse Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers 
 */
#include <stdlib.h>
#include <iostream>

/**
 * \namespace std
 * Export standard iostream objects (cin, cout,...).
 */
using namespace std;


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"


/*
 * Local Headers 
 */
#include "msg.h"
#include "msgPrivate.h"


/* 
 * Main
 */
int main(int argc, char *argv[])
{
    msgMESSAGE msg;
    string buffer = "Hello server, client speaking !";

    // Initialize MCS services
    if (mcsInit(argv[0]) == FAILURE)
    {
        // Exit from the application with FAILURE
        exit (EXIT_FAILURE);
    }

    int portNumber = 1980;

    cout << "Client creating a new msgSOCKET_CLIENT object ... ";
    msgSOCKET_CLIENT clientSocket;
    cout << "OK" << endl;
    cout << "Client connecting to server on localhost, port '" << portNumber
         << "'... ";
    if (clientSocket.Open("localhost", portNumber) == FAILURE)
    {
        cout << "KO" << endl;
        goto errCond;
    }
    cout << "OK" << endl << endl;

    cout << "Client sending   '" << buffer << "' to server ... ";
    if (clientSocket.Send(buffer) == FAILURE)
    {
        cout << "KO" << endl;
        goto errCond;
    }
    cout << "OK" << endl;
    cout << "Client receiving '";
    if (clientSocket.Receive(buffer) == FAILURE)
    {
        cout << "KO" << endl;
        goto errCond;
    }
    
    cout << buffer << "' from server ... OK" << endl;
    cout << "Client receiving '";
    if (clientSocket.Receive(buffer) == FAILURE)
    {
        cout << "KO" << endl;
        goto errCond;
    }
    cout << buffer << "' from server ... OK" << endl;
    buffer = "Bye server said client !";
    cout << "Client sending   '" << buffer << "' to server ... ";
    if (clientSocket.Send(buffer) == FAILURE)
    {
        cout << "KO" << endl;
        goto errCond;
    }
    cout << "OK" << endl << endl;
    
    if (msg.SetSender("client") == FAILURE)
    {
        goto errCond;
    }
    if (msg.SetRecipient("server") == FAILURE)
    {
        goto errCond;
    }
    if (msg.SetType(msgTYPE_COMMAND) == FAILURE)
    {
        goto errCond;
    }
    if (msg.SetCommand("COMMAND") == FAILURE)
    {
        goto errCond;
    }
    if (msg.SetBody("Hello server, client speaking !",
                     sizeof("Hello server, client speaking !")) == FAILURE)
    {
        goto errCond;
    }
    cout << "Client sending '";
    msg.Display();
    cout << "' to server ... ";
    if (clientSocket.Send(msg) == FAILURE)
    {
        cout << "KO" << endl;
        goto errCond;
    }
    cout << "OK" << endl << endl;
    cout << "Client receiving '";
    if (clientSocket.Receive(msg, 1000) == FAILURE)
    {
        cout << "KO" << endl;
        goto errCond;
    }
    msg.Display();
    cout << "' from server ... OK" << endl << endl;

    cout << "Client deconnecting from server ... ";
    if (clientSocket.Close() == FAILURE)
    {
        cout << "KO" << endl;
        goto errCond;
    }
    cout << "OK" << endl;

    // Close MCS services
    mcsExit();
    
    // Exit from the application with SUCCESS
    exit (EXIT_SUCCESS);

// If an error occured, show the error stack and exit
errCond:
    if (errStackIsEmpty() == mcsFALSE)
    {
        errDisplayStack();
        errCloseStack();
    }

    mcsExit();
    exit(EXIT_FAILURE);
}


/*___oOo___*/
