/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgTestSocketServer.cpp,v 1.2 2004-12-15 15:53:38 lafrasse Exp $"
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
 * Small test server based on msgSOCKET_SERVER.
 *
 * \synopsis
 * \<msgTestSocketServer\>
 */

static char *rcsId="@(#) $Id: msgTestSocketServer.cpp,v 1.2 2004-12-15 15:53:38 lafrasse Exp $"; 
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
    string buffer;
    msgSOCKET tempSocket;

    // Initialize MCS services
    if (mcsInit(argv[0]) == FAILURE)
    {
        // Exit from the application with FAILURE
        exit (EXIT_FAILURE);
    }

    int portNumber = 1980;

    cout << "Server creating a new msgSOCKET_SERVER object ... ";
    msgSOCKET_SERVER serverSocket;
    cout << "OK" << endl;

    cout << "Server binding the new socket to port '" << portNumber << "' ... ";
    if (serverSocket.Open(portNumber) == FAILURE)
    {
        cout << "KO" << endl;
        goto errCond;
    }
    cout << "OK" << endl << endl;

    cout << "Server creating a new msgSOCKET object ... ";
    cout << "OK" << endl;
    cout << "Server waiting for a connection on port '" << portNumber
         << "' ... ";
    if (serverSocket.Accept(tempSocket) == FAILURE)
    {
        cout << "KO" << endl;
        goto errCond;
    }
    cout << "OK" << endl << endl;

    cout << "Server receiving '";
    if (tempSocket.Receive(buffer) == FAILURE)
    {
        cout << "KO" << endl;
        goto errCond;
    }
    cout << buffer << "' from client ... OK" << endl;
    buffer = "Hello client, server speaking !";
    cout << "Server sending   '" << buffer << "' to client ... ";
    if (tempSocket.Send(buffer) == FAILURE)
    {
        cout << "KO" << endl;
        goto errCond;
    }
    cout << "OK" << endl;
    buffer = "Bye client said server !";
    cout << "Server sending   '" << buffer << "' to client ... ";
    if (tempSocket.Send(buffer) == FAILURE)
    {
        cout << "KO" << endl;
        goto errCond;
    }
    cout << "OK" << endl;
    cout << "Server receiving '";
    if (tempSocket.Receive(buffer) == FAILURE)
    {
        cout << "KO" << endl;
        goto errCond;
    }
    cout << buffer << "' from client ... OK" << endl << endl;

    cout << "Server receiving '";
    if (tempSocket.Receive(msg, 1000) == FAILURE)
    {
        cout << "KO" << endl;
        goto errCond;
    }
    msg.Display();
    cout << "' from client ... OK" << endl << endl;
    if (msg.SetSender("server") == FAILURE)
    {
        goto errCond;
    }
    if (msg.SetRecipient("client") == FAILURE)
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
    if (msg.SetBody("Bye client said server !",
                     sizeof("Bye client said server !")) == FAILURE)
    {
        goto errCond;
    }
    cout << "Server sending '";
    msg.Display();
    cout << "' to client ... ";
    if (tempSocket.Send(msg) == FAILURE)
    {
        cout << "KO" << endl;
        goto errCond;
    }
    cout << "OK" << endl << endl;

    cout << "Server closing msgSOCKET socket to client ... ";
    if (tempSocket.Close() == FAILURE)
    {
        cout << "KO" << endl;
        goto errCond;
    }
    cout << "OK" << endl;
    cout << "Server closing msgSOCKET_SERVER listening socket ... ";
    if (serverSocket.Close() == FAILURE)
    {
        cout << "KO" << endl;
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
