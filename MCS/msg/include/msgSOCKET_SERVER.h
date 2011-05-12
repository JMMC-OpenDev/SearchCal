#ifndef msgSOCKET_SERVER_H
#define msgSOCKET_SERVER_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of msgSOCKET_SERVER class 
 */


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * MCS Headers 
 */
#include "mcs.h"


/*
 * Local Headers 
 */
#include "msgSOCKET.h"


/*
 * Class declaration
 */

/**
 * Server-side specialized object wrapper around IPv4 BSD socket.
 *  
 * \n
 * \ex
 * Server-side code :
 * \code
 * #include <stdlib.h>
 * #include <iostream>
 * 
 * using namespace std;
 * 
 * #define MODULE_ID "mymod"
 * 
 * #include "mcs.h"
 * #include "log.h"
 * #include "err.h"
 * #include "msg.h"
 * 
 * int main(int argc, char *argv[])
 * {
 *     string buffer;
 *     msgSOCKET tempSocket;
 * 
 *     mcsInit(argv[0]);
 * 
 *     int portNumber = 2005;
 * 
 *     msgSOCKET_SERVER serverSocket;
 *     if (serverSocket.Open(portNumber) == mcsFAILURE)
 *     {
 *         goto errCond;
 *     }
 * 
 *     if (serverSocket.Accept(tempSocket) == mcsFAILURE)
 *     {
 *         goto errCond;
 *     }
 * 
 *     if (tempSocket.Receive(buffer) == mcsFAILURE)
 *     {
 *         goto errCond;
 *     }
 *
 *     cout << "Received : " << buffer << endl;
 *
 *     if (tempSocket.Send(buffer) == mcsFAILURE)
 *     {
 *         goto errCond;
 *     }
 * 
 *     if (serverSocket.Close() == mcsFAILURE)
 *     {
 *         goto errCond;
 *     }
 *    
 *     mcsExit();
 *     exit (EXIT_SUCCESS);
 * 
 * // If an error occured, show the error stack and exit
 * errCond:
 *     if (errStackIsEmpty() == mcsFALSE)
 *     {
 *         errDisplayStack();
 *         errCloseStack();
 *     }
 * 
 *     mcsExit();
 *     exit(EXIT_FAILURE);
 * }
 * \endcode
 */
class msgSOCKET_SERVER : public msgSOCKET
{

public:
    // Brief description of the constructor
    msgSOCKET_SERVER();

    // Brief description of the destructor
    virtual ~msgSOCKET_SERVER();

    // Open a server socket on the given port number
    virtual mcsCOMPL_STAT Open(mcsUINT16 port);

protected:

    
private:
     // Declaration of copy constructor and assignment operator as private
     // methods, in order to hide them from the users.
     msgSOCKET_SERVER(const msgSOCKET_SERVER&);
     msgSOCKET_SERVER& operator=(const msgSOCKET_SERVER&);
};


#endif /*!msgSOCKET_SERVER_H*/

/*___oOo___*/
