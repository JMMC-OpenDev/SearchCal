/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgTestSocketClientTimeOut.cpp,v 1.3 2006-05-11 13:04:56 mella Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.2  2005/09/12 13:08:05  scetre
* Added test on Socket client with time out when receiving CDS data
*
* Revision 1.1  2005/09/12 13:06:49  scetre
* Added test on Socket client with time out when receiving CDS data
*
* Revision 1.5  2005/04/22 09:25:09  mella
* Replace FAILURE by mcsFAILURE
*
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

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: msgTestSocketClientTimeOut.cpp,v 1.3 2006-05-11 13:04:56 mella Exp $";

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
    static char * buffer = "GET http://vizier.u-strasbg.fr/viz-bin/asu-xml?-source=II/225/catalog\n";
    string buffer2;

    // Initialize MCS services
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        // Exit from the application with FAILURE
        exit (EXIT_FAILURE);
    }

    int portNumber = 80;
    // vizier CDS IP
    const std::string host = "130.79.128.13";
    cout << "Client creating a new msgSOCKET_CLIENT object ... ";
    msgSOCKET_CLIENT clientSocket;
    cout << "OK" << endl;
    cout << "Client connecting to server on , vizier.u-strasbg.fr port '" << portNumber
        << "'... ";
    if (clientSocket.Open(host, portNumber) == mcsFAILURE)
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
    if (clientSocket.Receive(buffer2, 1000) == mcsFAILURE)
    {
        goto errCond;
    }
    cout << buffer2  ;
    
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

