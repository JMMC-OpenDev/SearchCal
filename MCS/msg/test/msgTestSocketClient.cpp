/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Small test client based on msgSOCKET_CLIENT.
 *
 * \synopsis
 * \<msgTestSocketClient\>
 */


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
    string buffer = "GET /\n";

    // Initialize MCS services
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        // Exit from the application with FAILURE
        exit (EXIT_FAILURE);
    }

    int portNumber = 80;

    cout << "Client creating a new msgSOCKET_CLIENT object ... ";
    msgSOCKET_CLIENT clientSocket;
    cout << "OK" << endl;
    cout << "Client connecting to server on localhost, port '" << portNumber
         << "'... ";
    if (clientSocket.Open("localhost", portNumber) == mcsFAILURE)
    {
        cout << "KO" << endl;
        goto errCond;
    }
    cout << "OK" << endl << endl;

    cout << "Client sending   '" << buffer << "' to server ... ";
    if (clientSocket.Send(buffer) == mcsFAILURE)
    {
        cout << "KO" << endl;
        goto errCond;
    }
    cout << "OK" << endl;
    cout << "Client receiving '";
    if (clientSocket.Receive(buffer, 5000) == mcsFAILURE)
    {
        cout << "KO" << endl;
        goto errCond;
    }
    
    cout << buffer << "' from server ... OK" << endl;
    cout << "Client receiving '";
    if (clientSocket.Receive(buffer) == mcsFAILURE)
    {
        cout << "KO" << endl;
        goto errCond;
    }
    cout << buffer << "' from server ... OK" << endl;
    buffer = "Bye server said client !";
    cout << "Client sending   '" << buffer << "' to server ... ";
    if (clientSocket.Send(buffer) == mcsFAILURE)
    {
        cout << "KO" << endl;
        goto errCond;
    }
    cout << "OK" << endl << endl;
    
    if (msg.SetSender("client") == mcsFAILURE)
    {
        goto errCond;
    }
    if (msg.SetRecipient("server") == mcsFAILURE)
    {
        goto errCond;
    }
    if (msg.SetType(msgTYPE_COMMAND) == mcsFAILURE)
    {
        goto errCond;
    }
    if (msg.SetCommand("COMMAND") == mcsFAILURE)
    {
        goto errCond;
    }
    if (msg.SetBody("Hello server, client speaking !",
                     sizeof("Hello server, client speaking !")) == mcsFAILURE)
    {
        goto errCond;
    }
    cout << "Client sending '" << msg << "' to server ... ";
    if (clientSocket.Send(msg) == mcsFAILURE)
    {
        cout << "KO" << endl;
        goto errCond;
    }
    cout << "OK" << endl << endl;
    cout << "Client receiving '";
    if (clientSocket.Receive(msg, 1000) == mcsFAILURE)
    {
        cout << "KO" << endl;
        goto errCond;
    }
    cout << msg << "' from server ... OK" << endl << endl;

    cout << "Client deconnecting from server ... ";
    if (clientSocket.Close() == mcsFAILURE)
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
        errCloseStack();
    }

    mcsExit();
    exit(EXIT_FAILURE);
}


/*___oOo___*/
