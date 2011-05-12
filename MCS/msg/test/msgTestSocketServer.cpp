/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Small test server based on msgSOCKET_SERVER.
 *
 * \synopsis
 * \<msgTestSocketServer\>
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: msgTestSocketServer.cpp,v 1.5 2006-05-11 13:04:56 mella Exp $";

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
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        // Exit from the application with FAILURE
        exit (EXIT_FAILURE);
    }

    int portNumber = 1980;

    cout << "Server creating a new msgSOCKET_SERVER object ... ";
    msgSOCKET_SERVER serverSocket;
    cout << "OK" << endl;

    cout << "Server binding the new socket to port '" << portNumber << "' ... ";
    if (serverSocket.Open(portNumber) == mcsFAILURE)
    {
        cout << "KO" << endl;
        goto errCond;
    }
    cout << "OK" << endl << endl;

    cout << "Server creating a new msgSOCKET object ... ";
    cout << "OK" << endl;
    cout << "Server waiting for a connection on port '" << portNumber
         << "' ... ";
    if (serverSocket.Accept(tempSocket) == mcsFAILURE)
    {
        cout << "KO" << endl;
        goto errCond;
    }
    cout << "OK" << endl << endl;

    cout << "Server receiving '";
    if (tempSocket.Receive(buffer) == mcsFAILURE)
    {
        cout << "KO" << endl;
        goto errCond;
    }
    cout << buffer << "' from client ... OK" << endl;
    buffer = "Hello client, server speaking !";
    cout << "Server sending   '" << buffer << "' to client ... ";
    if (tempSocket.Send(buffer) == mcsFAILURE)
    {
        cout << "KO" << endl;
        goto errCond;
    }
    cout << "OK" << endl;
    buffer = "Bye client said server !";
    cout << "Server sending   '" << buffer << "' to client ... ";
    if (tempSocket.Send(buffer) == mcsFAILURE)
    {
        cout << "KO" << endl;
        goto errCond;
    }
    cout << "OK" << endl;
    cout << "Server receiving '";
    if (tempSocket.Receive(buffer) == mcsFAILURE)
    {
        cout << "KO" << endl;
        goto errCond;
    }
    cout << buffer << "' from client ... OK" << endl << endl;

    cout << "Server receiving '";
    if (tempSocket.Receive(msg, 1000) == mcsFAILURE)
    {
        cout << "KO" << endl;
        goto errCond;
    }
    cout << msg << "' from client ... OK" << endl << endl;
    if (msg.SetSender("server") == mcsFAILURE)
    {
        goto errCond;
    }
    if (msg.SetRecipient("client") == mcsFAILURE)
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
    if (msg.SetBody("Bye client said server !",
                     sizeof("Bye client said server !")) == mcsFAILURE)
    {
        goto errCond;
    }
    cout << "Server sending '" << msg << "' to client ... ";
    if (tempSocket.Send(msg) == mcsFAILURE)
    {
        cout << "KO" << endl;
        goto errCond;
    }
    cout << "OK" << endl << endl;

    cout << "Server closing msgSOCKET socket to client ... ";
    if (tempSocket.Close() == mcsFAILURE)
    {
        cout << "KO" << endl;
        goto errCond;
    }
    cout << "OK" << endl;
    cout << "Server closing msgSOCKET_SERVER listening socket ... ";
    if (serverSocket.Close() == mcsFAILURE)
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
