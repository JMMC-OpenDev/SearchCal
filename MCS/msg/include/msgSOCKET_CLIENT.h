#ifndef msgSOCKET_CLIENT_H
#define msgSOCKET_CLIENT_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of msgSOCKET_CLIENT class 
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
 * Client-side specialized object wrapper around IPv4 BSD socket.
 * 
 * \n
 * \ex
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
 *     string buffer = "Hello server, client speaking !";
 * 
 *     mcsInit(argv[0]);
 * 
 *     int portNumber = 2005;
 * 
 *     msgSOCKET_CLIENT clientSocket;
 *     if (clientSocket.Open("localhost", portNumber) == mcsFAILURE)
 *     {
 *         goto errCond;
 *     }
 * 
 *     if (clientSocket.Send(buffer) == mcsFAILURE)
 *     {
 *         goto errCond;
 *     }
 * 
 *     if (clientSocket.Receive(buffer) == mcsFAILURE)
 *     {
 *         goto errCond;
 *     }
 * 
 *     cout << "Received : " << buffer << endl;
 * 
 *     if (clientSocket.Close() == mcsFAILURE)
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
class msgSOCKET_CLIENT : public msgSOCKET
{
public:
    // Brief description of the constructor
    msgSOCKET_CLIENT();

    // Brief description of the destructor
    virtual ~msgSOCKET_CLIENT();

    // Open a client socket to a given server port number on a given host
    virtual mcsCOMPL_STAT Open(std::string host, mcsUINT16 port);

protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    msgSOCKET_CLIENT(const msgSOCKET_CLIENT&);
    msgSOCKET_CLIENT& operator=(const msgSOCKET_CLIENT&);
};

#endif /*!msgSOCKET_CLIENT_H*/

/*___oOo___*/
