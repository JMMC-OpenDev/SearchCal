/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgTestSocketClient.cpp,v 1.1 2004-11-26 13:11:28 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  24-Nov-2004  Created
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

static char *rcsId="@(#) $Id: msgTestSocketClient.cpp,v 1.1 2004-11-26 13:11:28 lafrasse Exp $"; 
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

    cout << "Client creating a new msgSOCKET_CLIENT object ... ";
    msgSOCKET_CLIENT clientSocket;
    cout << "OK" << endl;
    cout << "Client connecting to server on localhost, port '" << portNumber
         << "'... ";
    clientSocket.Open("localhost", portNumber);
    cout << "OK" << endl << endl;

    string buffer = "Hello server, client speaking !";
    cout << "Client sending   '" << buffer << "' to server ... ";
    clientSocket.Send(buffer);
    cout << "OK" << endl;
    cout << "Client receiving '";
    clientSocket.Receive(buffer);
    cout << buffer << "' from server ... OK" << endl;
    cout << "Client receiving '";
    clientSocket.Receive(buffer);
    cout << buffer << "' from server ... OK" << endl;
    buffer = "Bye server said client !";
    cout << "Client sending   '" << buffer << "' to server ... ";
    clientSocket.Send(buffer);
    cout << "OK" << endl << endl;

    msgMESSAGE msg;
    msg.SetSender("client");
    msg.SetRecipient("server");
    msg.SetType(msgTYPE_COMMAND);
    msg.SetCommand("COMMAND");
    msg.SetBody("Hello server, client speaking !",
                sizeof("Hello server, client speaking !"));
    cout << "Client sending '";
    msg.Display();
    cout << "' to server ... ";
    clientSocket.Send(msg);
    cout << "OK" << endl << endl;
    cout << "Client receiving '";
    clientSocket.Receive(msg, 1000);
    msg.Display();
    cout << "' from server ... OK" << endl << endl;

    cout << "Client deconnecting from server ... ";
    clientSocket.Close();
    cout << "OK" << endl;

    // Close MCS services
    mcsExit();
    
    // Exit from the application with SUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
