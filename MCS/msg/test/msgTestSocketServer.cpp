/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgTestSocketServer.cpp,v 1.1 2004-11-26 13:11:28 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  24-Nov-2004  Created
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

static char *rcsId="@(#) $Id: msgTestSocketServer.cpp,v 1.1 2004-11-26 13:11:28 lafrasse Exp $"; 
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
    serverSocket.Open(portNumber);
    cout << "OK" << endl << endl;

    cout << "Server creating a new msgSOCKET object ... ";
    msgSOCKET tempSocket;
    cout << "OK" << endl;
    cout << "Server waiting for a connection on port '" << portNumber
         << "' ... ";
    serverSocket.Accept(tempSocket);
    cout << "OK" << endl << endl;

    string buffer;
    cout << "Server receiving '";
    tempSocket.Receive(buffer);
    cout << buffer << "' from client ... OK" << endl;
    buffer = "Hello client, server speaking !";
    cout << "Server sending   '" << buffer << "' to client ... ";
    tempSocket.Send(buffer);
    cout << "OK" << endl;
    buffer = "Bye client said server !";
    cout << "Server sending   '" << buffer << "' to client ... ";
    tempSocket.Send(buffer);
    cout << "OK" << endl;
    cout << "Server receiving '";
    tempSocket.Receive(buffer);
    cout << buffer << "' from client ... OK" << endl << endl;

    msgMESSAGE msg;
    cout << "Server receiving '";
    tempSocket.Receive(msg, 1000);
    msg.Display();
    cout << "' from client ... OK" << endl << endl;
    msg.SetSender("server");
    msg.SetRecipient("client");
    msg.SetType(msgTYPE_COMMAND);
    msg.SetCommand("COMMAND");
    msg.SetBody("Bye client said server !", sizeof("Bye client said server !"));
    cout << "Server sending '";
    msg.Display();
    cout << "' to client ... ";
    tempSocket.Send(msg);
    cout << "OK" << endl << endl;

    cout << "Server closing msgSOCKET socket to client ... ";
    tempSocket.Close();
    cout << "OK" << endl;
    cout << "Server closing msgSOCKET_SERVER listening socket ... ";
    serverSocket.Close();
    cout << "OK" << endl;

    // Close MCS services
    mcsExit();
    
    // Exit from the application with SUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
