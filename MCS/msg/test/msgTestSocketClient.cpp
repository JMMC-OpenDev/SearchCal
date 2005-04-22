/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgTestSocketClient.cpp,v 1.5 2005-04-22 09:25:09 mella Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.4  2005/02/09 16:41:02  lafrasse
* Changed the way mesage are displayed (now use msgMESSAGE 'operator<<' instead of 'Display')
*
* Revision 1.3  2005/02/04 15:57:06  lafrasse
* Massive documentation review an refinment (also added automatic CVS log inclusion in every files)
*
* lafrasse  14-Dec-2004  Added error code management
* lafrasse  24-Nov-2004  Created
*
*******************************************************************************/

/**
 * \file
 * Small test client based on msgSOCKET_CLIENT.
 *
 * \synopsis
 * \<msgTestSocketClient\>
 */

static char *rcsId="@(#) $Id: msgTestSocketClient.cpp,v 1.5 2005-04-22 09:25:09 mella Exp $"; 
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
    if (mcsInit(argv[0]) == mcsFAILURE)
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
    if (clientSocket.Receive(buffer) == mcsFAILURE)
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
        errDisplayStack();
        errCloseStack();
    }

    mcsExit();
    exit(EXIT_FAILURE);
}


/*___oOo___*/
