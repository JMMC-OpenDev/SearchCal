/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: msgMANAGER.cpp,v 1.19 2005-02-09 16:36:13 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.18  2005/02/04 15:57:06  lafrasse
 * Massive documentation review an refinment (also added automatic CVS log inclusion in every files)
 *
 * Revision 1.17  2005/01/31 13:52:26  gzins
 * Fixed wrong returned completion status value in Forward()
 *
 * Revision 1.16  2005/01/29 20:04:35  gzins
 * Added PROCLIST command handling
 * Managed unicity flag for connected processes
 *
 * Revision 1.15  2005/01/29 09:56:25  gzins
 * Updated to notify client when a server is exiting abnormally
 *
 * Revision 1.14  2005/01/26 17:54:42  gzins
 * Removed reference to action log
 *
 * Revision 1.13  2005/01/26 08:47:18  gzins
 * Added PrepareReply to fix bug related to wrong message type when sending reply to sender.
 *
 * Revision 1.12  2005/01/24 15:02:47  gzins
 * Added CVS logs as modification history
 *
 * lafrasse  21-Jan-2005  Added a timeout when exiting in order to have
 *                        sufficient time to receive the 'EXIT' query answer
 * gzins     07-Jan-2005  Changed SUCCESS/FAILURE to mcsSUCCESS/mcsFAILURE 
 * gzins     22-Dec-2004  Replaced GetBodyPtr by GetBody 
 * gzins     20-Dec-2004  Fixed bug related to the use of GetNextProcess after
 *                        removing a process from the list.
 * gzins     14-Dec-2004  Handled DEBUG command
 * lafrasse  14-Dec-2004  Changed body type from statically sized buffer to a
 *                        misc Dynamic Buffer (no more msgMAXLEN)
 * gzins     12-Dec-2004  Added errno.h header file
 * gzins     09-Dec-2004  Fixed cast problem with new mcsLOGICAL enumerate
 * gzins     08-Dec-2004  Replaced msgMCS_ENVS with envLIST
 * gzins     08-Dec-2004  Updated to support several processes with same name  
 * gzins     06-Dec-2004  Created
 *
 ******************************************************************************/

/**
 * \file
 * \<msgManager\> main class, containing the application infinite loop and all
 * the high-level 'messages management' related methods.
 *
 * \sa msgMANAGER
 */

static char *rcsId="@(#) $Id: msgMANAGER.cpp,v 1.19 2005-02-09 16:36:13 lafrasse Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers 
 */
#include <iostream>
using namespace std;
#include <sys/ioctl.h>
#include <errno.h>

/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "env.h"

/*
 * Local Headers 
 */
#include "msgDEBUG_CMD.h"
#include "msgPROCLIST_CMD.h"
#include "msgMANAGER.h"
#include "msgMESSAGE.h"
#include "msgPrivate.h"
#include "msgErrors.h"



/**
 * Class constructor
 */
msgMANAGER::msgMANAGER()
{
}

/**
 * Class destructor
 */
msgMANAGER::~msgMANAGER()
{
    logExtDbg("msgMANAGER::~msgMANAGER()");
    _connectionSocket.Close();
}



/*
 * Public methods
 */

/**
 * Manager initialization.
 *
 * Registers the application to MCS services, parses the received
 * command-line parameters, and open the main listening socket.
 *
 * \param argc number of arguments supplied to the method in the argv array
 * \param argv array of pointers to the strings which are those arguments
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT msgMANAGER::Init(int argc, char *argv[])
{
    logExtDbg("msgMANAGER::Init()");

    // Initialiaze MCS services
    if (mcsInit(argv[0]) != mcsSUCCESS)
    {
        return mcsFAILURE;
    }

    // Parses command-line options 
    if (ParseOptions(argc, argv) != mcsSUCCESS)
    {
        return mcsFAILURE;
    }
    
    // Port number of the current environment
    envLIST envList;
    mcsINT32    portNumber;
    portNumber = envList.GetPortNumber();
    if (portNumber == -1)
    {
        return mcsFAILURE;
    }

    // Open connection socket
    logTest("Environment '%s', port : %d", mcsGetEnvName(), portNumber);
    if (_connectionSocket.Open(portNumber) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/**
 * Main loop.
 *
 * Infinite loop that :
 * \li wait for messages;
 * \li accept and register new processes;
 * \li forward messages between processes;
 * \li execute the application own commands.
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT msgMANAGER::MainLoop()
{
    logExtDbg("msgMANAGER::MainLoop()");

    // For ever...
    for(;;)
    {
        // Set the set of descriptors for reading
        fd_set readMask;
        FD_ZERO(&readMask);

        // For each connected processes
        for (unsigned int el = 0; el < _processList.Size(); el++)
        {
            // Get the socket descriptor
            int sd = 
              _processList.GetNextProcess((mcsLOGICAL)(el==0))->GetDescriptor();

            // Add it to the list of descriptors for reading
            if (sd != -1)
            {
                FD_SET(sd, &readMask);
            }
        }

        // Add connection socket
        FD_SET(_connectionSocket.GetDescriptor(), &readMask);
        
        // Wait for message
        int status;
        logTest("Waiting message...");
        status = select(msgMANAGER_SELECT_WIDTH, &readMask, (fd_set *) NULL,
                        (fd_set *) NULL, (struct timeval *) NULL );
        
        /* If an error occured in the select() function... */
        if (status == -1)
        {
            // Raise an error
            logError("ERROR : select() failed - %s\n", strerror(errno));
            
            /* Wait 1 second in order not to loop infinitly (and thus using
             * 100% of the CPU) if it is a recurrent error
             */
            sleep(1);
        }

        // If a new connection demand was received...
        if (FD_ISSET(_connectionSocket.GetDescriptor() , &readMask))
        {
            logTest("Connection demand received...");

            // Init the new connection
            if (AcceptConnection() == mcsFAILURE)
            {
                errCloseStack();
            }
        }
        else // A message from an already connected process was received...
        {
            logTest("Message received...");
            mcsINT32   nbBytesToRead ;

            // Look for the process which sent message
            msgPROCESS *process = _processList.GetNextProcess(mcsTRUE);
            while (process != NULL)
            {
                // If it is the sending process 
                if ((process->GetDescriptor() != -1) && 
                    FD_ISSET(process->GetDescriptor(), &readMask))
                {
                    // Stop
                    break;
                }
                // Get the next process 
                process = _processList.GetNextProcess(mcsFALSE);
            }

            // If the sending process has been found
            if (process != NULL)
            {
                // If there is some data to be read...
                ioctl(process->GetDescriptor(), FIONREAD,
                      (unsigned long *)&nbBytesToRead);
                if (nbBytesToRead != 0)
                {
                    // Read the new message
                    msgMESSAGE msg;
                    if (process->Receive(msg, 0) == mcsFAILURE)
                    {
                        errCloseStack();
                    }
                    else
                    {
                        // If the new message is a command...
                        if (msg.GetType() == msgTYPE_COMMAND)
                        {
                            // If the command is intended to msgManger...
                            if ((strcmp(msg.GetRecipient(), "msgManager") == 0)
                                || (strcmp(msg.GetCommand(), msgPING_CMD_NAME)
                                 == 0))
                            {
                                // Handle the received command
                                if (HandleCmd(msg) == mcsFAILURE)
                                {
                                    errCloseStack();
                                }
                            }
                            else // If the command is not for msgManager
                            {
                                // Forward to the destination process
                                if (Forward(msg) == mcsFAILURE)
                                {
                                    errCloseStack();
                                }
                                else
                                {
                                    // Get process which should sent the waited
                                    // reply
                                    msgPROCESS *replySender;        
                                    replySender = _processList.GetProcess
                                        (msg.GetRecipient());

                                    // Add entry in the list of  waited command
                                    // replies
                                    _waitedCmdReplyList.push_back
                                        (pair<int, msgMESSAGE>
                                         (replySender->GetDescriptor(), msg));
                                }
                            }
                        }
                        else // If the message is a reply...
                        {
                            // Send reply to the sender process
                            SendReply(msg);
                            
                            // Removed reply from the awaited commamd reply list
                            RemoveProcessWaitingFor(msg.GetCommandId());
                        }
                    }
                }
                else // If there was nothing to read...
                {
                    logWarning("Connection with '%s' process lost",
                               process->GetName());

                    // Close the connection
                    mcsINT32 procDescriptor=process->GetDescriptor();
                    if (_processList.Remove(procDescriptor) == mcsFAILURE)
                    {
                        errCloseStack();
                    }

                    // Release process which are waiting for from this process
                    if (ReleaseWaitingProcess(procDescriptor) == mcsFAILURE)
                    {
                        errCloseStack();
                    }
                }
            }
            else
            {
                logWarning("Message received from unregister I/O stream");
            }
        } // End if
    } // For ever end

    return mcsSUCCESS;
}



/*
 * Protected methods
 */

/**
 * Parses the given command-line parameters and change the objet behavior
 * accordingly.
 *
 * \param argc number of arguments supplied to the method in the argv array
 * \param argv array of pointers to the strings which are those arguments
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT msgMANAGER::ParseOptions(mcsINT32 argc, char *argv[])
{
    mcsINT32  level;
    mcsINT32  optInd;

    logExtDbg ("evhTASK::ParseOptions ()");

    // For each command option
    for (optInd =  1; optInd < argc; optInd++)
    {
        // If help option specified
        if (strcmp(argv[optInd], "-h") == 0)
        {
            // Print usage
            Usage();
            exit (EXIT_SUCCESS);
        }
        // Else if '-version' option specified
        else if (strcmp(argv[optInd], "-version") == 0)
        {
            // Prints the version number of the SW
            printf ("%s\n", GetSwVersion());
            exit (EXIT_SUCCESS);
        }
        // Else if logging level specified
        else if (strcmp(argv[optInd], "-l") == 0)
        { 
            // Set new logging level
            if ((optInd + 1) < argc)
            {
                optInd += 1;
                optarg = argv[optInd];
                if ( sscanf (optarg, "%d", &level) != 1)
                {
                    logError ("%s: Argument to option %s is invalid: '%s'",
                              mcsGetProcName(), argv[optInd-1], optarg);
                    return mcsFAILURE;
                }
                logSetFileLogLevel((logLEVEL)level);
            }
            else
            {
                logError ("%s: Option %s requires an argument",
                          mcsGetProcName(), argv[optInd]);
                return mcsFAILURE;
            }
        }
        // Else if stdout level specified
        else if (strcmp(argv[optInd], "-v") == 0)
        {
            // Set new stdout log level
            if ((optInd + 1) < argc)
            {
                optInd += 1;
                optarg = argv[optInd];
                if ( sscanf (optarg, "%d", &level) != 1)
                {
                    logError ("%s: Argument to option %s is invalid: '%s'",
                              mcsGetProcName(), argv[optInd-1], optarg);
                    return mcsFAILURE;
                }
                logSetStdoutLogLevel((logLEVEL)level);
            }
            else
            {
                logError ("%s: Option %s requires an argument",
                          mcsGetProcName(), argv[optInd]);
                return mcsFAILURE;
            }
        }
        // Else if '-noDate' option specified
        else if (strcmp(argv[optInd], "-noDate") == 0)
        {
            // Turns off the display of date
            logSetPrintDate(mcsFALSE);
        }
        // Else if '-noFileLine' option specified
        else if (strcmp(argv[optInd], "-noFileLine") == 0)
        {
            // Turns off the display of file/line
            logSetPrintFileLine(mcsFALSE);
        }
        // Else option is unknown 
        else
        {
            logError ("%s: Invalid option %s", 
                      mcsGetProcName(), argv[optInd] );
            return mcsFAILURE;
        }
    }
    return mcsSUCCESS;
}

/**
 * Displays the application command-line handled options on the standard output.
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT msgMANAGER::Usage(void)
{
    cout << "Usage:" << mcsGetProcName() << " [OPTIONS]"<< endl;
    cout <<" Standard options: -l <level>   set file log level" << endl;
    cout <<"                   -v <level>   set stdout log level" << endl;
    cout <<"                   -h           print this help" << endl;
    cout <<"                   -version     print the version number of the ";
    cout <<"software" << endl;
    cout <<"                   -noDate      turn off the display of date";
    cout <<" in stdout log" << endl;
    cout <<"                                messages" << endl;
    cout <<"                   -noFileLine  turn off the display of file name";
    cout <<" and line number" << endl;
    cout <<"                                in stdout log messages" << endl; 

    return mcsSUCCESS;
}

/**
 * Give back the version number of the application.
 *
 * \return the software version as a character pointer.
 */
const char *msgMANAGER::GetSwVersion(void)
{
    return "SW version number no set"; 
}

/**
 * Accept and register the connection of a new process.
 *
 * Verify that the new process name is unic, otherwise reject the connection
 * request.
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT msgMANAGER::AcceptConnection()
{
    logExtDbg("msgMANAGER::AcceptConnection()");

    // Accept the new connection
    msgPROCESS *newProcess = new msgPROCESS();
    if (_connectionSocket.Accept(*newProcess) != mcsSUCCESS)
    {
        errCloseStack();
    }
    else
    {
        // Receive the registering command
        msgMESSAGE msg;
        if (newProcess->Receive(msg, 1000) == mcsFAILURE)
        {
            // Close the connection
            delete(newProcess);
            errCloseStack();
        }
        else
        {
            // If the registering command is received...
            if (strcmp(msg.GetCommand(), msgREGISTER_CMD_NAME) == 0)
            {
                logTest("Connection demand received from %s ...",
                        msg.GetSender());

                logTest("'%s' connection accepted", msg.GetSender());

                /* Retreives information on process. If no process information
                 * in message, ignore error.
                 */
                mcsINT32   pid=-1;
                mcsLOGICAL unicityFlag=mcsFALSE;
                sscanf(msg.GetBody(), "%d %d", &pid, &unicityFlag);

                // If process must be unique, amd process already registered
                if ((unicityFlag == mcsTRUE) &&
                    (_processList.GetProcess(msg.GetSender()) != NULL))
                {
                    // Report error
                    errAdd (msgERR_DUPLICATE_PROC, msg.GetSender());

                    // Send an error reply
                    PrepareReply(msg);
                    SendReply(msg, newProcess);

                    // Delete process
                    delete (newProcess);
                }
                // Else
                else
                {
                    // Add the new process to the process list
                    newProcess->SetName(msg.GetSender());
                    newProcess->SetId(pid);
                    newProcess->SetUnicity(unicityFlag);
                    if (_processList.AddAtTail(newProcess) == mcsFAILURE)
                    {
                        errCloseStack();
                    }

                    // Send a registering validation messsage
                    msg.SetBody("OK");
                    PrepareReply(msg);
                    SendReply(msg);
                }
            }
            else // Wrong message received...
            {
                // Close the connection
                logWarning("Received a '%s' message instead of '%s'",
                           "- '%s' process connection refused",
                           msg.GetCommand(), msgREGISTER_CMD_NAME,
                           msg.GetSender());
                delete (newProcess);
            }
        }
    }
    return mcsSUCCESS;
}

/**
 * Forward a received command to its recipient process.
 *
 * \param msg the received message to be forwarded
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT msgMANAGER::Forward(msgMESSAGE &msg)
{
    msgPROCESS *recipient;

    logExtDbg("msgMANAGER::Forward()");
    logTest("Received '%s' command from '%s' for '%s'", msg.GetCommand(),
            msg.GetSender(), msg.GetRecipient());

    // Find the recipient process in the process list
    recipient = _processList.GetProcess(msg.GetRecipient());
    if (recipient == NULL)
    {
        // Report error to the sender
        errAdd(msgERR_RECIPIENT_NOT_CONNECTED, msg.GetRecipient(),
               msg.GetCommand());
        PrepareReply(msg);
        SendReply(msg);
        return mcsFAILURE;
    }
    else
    {
        // If the command could not be delivered to the recipient process...
        if (recipient->Send(msg) == mcsFAILURE)
        {
            // Report this to the sender
            PrepareReply(msg);
            SendReply(msg);
            return mcsFAILURE;
        }
    }
       
    return mcsSUCCESS;
}

/**
 * Prepare a reply from a given message.
 *
 * It prepares the message as a reply to be returned to the sender. This action
 * consists in :
 * \li setting the message 'last reply' flag or not;
 * \li setting the message type (REPLY or ERROR_REPLY) according to the error
 * stack current state (empty or not);
 * \li putting the error list in the message buffer if the error stack is not
 * empty.
 *
 * \param msg the message to reply
 * \param lastReply a boolean flag to specify wether the current message is the
 * last one or not (TRUE by default)
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT msgMANAGER::PrepareReply(msgMESSAGE &msg,
                                       mcsLOGICAL lastReply)
{
    logExtDbg("msgMANAGER::PrepareReply()");

    char *replyType;

    // Build the reply message header
    msg.SetLastReplyFlag(lastReply);

    // If there is no error in the error stack
    if (errStackIsEmpty() == mcsTRUE)
    {
        // Set message type to REPLY
        msg.SetType(msgTYPE_REPLY);
        replyType = "reply";
    }
    else
    {
        // Put the MCS error stack data in the message body
        char errStackContent[errSTACK_SIZE * errMSG_MAX_LEN];
        if (errPackStack(errStackContent, 
                         sizeof(errStackContent)) == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // Store the message body size
        msg.SetBody(errStackContent, strlen(errStackContent) + 1);

        // Set message type to ERROR_REPLY
        msg.SetType(msgTYPE_ERROR_REPLY);
        replyType = "error reply";

        // Empty error stack
        errResetStack();
    }

    return mcsSUCCESS;
}

/**
 * Send a reply message to a process.
 *
 * \param msg the message to reply
 * \param sender the process that should get the reply (the original request
 * sender by default)
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT msgMANAGER::SendReply (msgMESSAGE &msg,
                                     msgPROCESS *sender)
{
    logExtDbg("msgMANAGER::SendReply()");
    logTest("Sending '%s' reply : %s", msg.GetCommand(), msg.GetBody());

    // If not specified, get the original request sender
    if (sender == NULL)
    {
        sender = _processList.GetProcess(msg.GetSender(), msg.GetSenderId());
    }

    // If no sender was found
    if (sender == NULL)
    {
        // Raise the error to the sender
        errAdd(msgERR_SENDER_NOT_CONNECTED, msg.GetSender(), msg.GetCommand());
        
        // Close error stack
        errResetStack();
        return mcsFAILURE;
    }
    
    // Send the reply
    if (sender->Send(msg) == mcsFAILURE)
    {
        // Close error stack
        errResetStack();
        return mcsFAILURE;
    }

    return mcsSUCCESS;        
}

/**
 * \<msgManager\> own commands handler.
 *
 * Those commands are :
 * <table>
 * <tr><td> DEBUG </td><td> log level management </td></tr>
 * <tr><td> PROCLIST </td><td> returns the list of connected processes
 * </td></tr>
 * <tr><td> EXIT </td><td> safely abort the \<msgManager\> process </td></tr>
 * <tr><td> PING </td><td> test if a specified process is connected to
 * \<msgManager\>; </td></tr>
 * <tr><td> VERSION </td><td> give back the current \<msgManager\> version
 * number </td></tr>
 * <tr><td> <em>CLOSE </td><td> close the connection with \<msgManager\>
 * (internal use)</em> </td></tr>
 * </table>
 *
 * \param msg a received command message
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT msgMANAGER::HandleCmd (msgMESSAGE &msg)
{
    logExtDbg("msgMANAGER::HandleCmd()");
    logTest("Received internal '%s' command from '%s'", msg.GetCommand(),
            msg.GetSender());
    
    // If the received command is a PING request...
    if (strcmp(msg.GetCommand(), msgPING_CMD_NAME) == 0)
    {
        // If the command recipient is connected...
        if ((strcmp (msg.GetRecipient(), "msgManager") == 0) ||
            (_processList.GetProcess(msg.GetRecipient()) != NULL))
        {
            // Build an OK reply message
            msg.SetBody("OK");
        }
        else
        {
            // Raise the error
            errAdd(msgERR_PING, msg.GetRecipient());
        }

        // Send the built reply message
        PrepareReply(msg);
        SendReply(msg);
    } 
    // Else if the received command is a VERSION request...
    else if (strcmp(msg.GetCommand(), msgVERSION_CMD_NAME) == 0)
    {
        // Reply the msgManager CVS verson number
        mcsSTRING256 buffer;
        memset(buffer, '\0', sizeof(buffer));
        sprintf(buffer, rcsId);
        msg.SetBody(buffer);
        PrepareReply(msg);
        SendReply(msg);
    }  
    // Else if the received command is a DEBUG request...
    else if (strcmp(msg.GetCommand(), msgDEBUG_CMD_NAME) == 0)
    {
        msgDEBUG_CMD debugCmd(msg.GetCommand(), msg.GetBody());

        // Parses command
        if (debugCmd.Parse() == mcsFAILURE)
        {
            PrepareReply(msg);
            SendReply(msg);
            return mcsFAILURE;
        }
        
        // If 'stdoutLevel' parameter is specified...
        if (debugCmd.IsDefinedStdoutLevel() == mcsTRUE)
        {
            // Set new level
            mcsINT32 level;
            debugCmd.GetStdoutLevel(&level);
            logSetStdoutLogLevel((logLEVEL)level);
        }

        // If 'logfileLevel' parameter is specified...
        if (debugCmd.IsDefinedLogfileLevel() == mcsTRUE)
        {
            // Set new level
            mcsINT32 level;
            debugCmd.GetLogfileLevel(&level);
            logSetFileLogLevel((logLEVEL)level);
        }
        
        // If 'printDate' parameter is specified...
        if (debugCmd.IsDefinedPrintDate() == mcsTRUE)
        {
            // Set new level
            mcsLOGICAL flag;
            debugCmd.GetPrintDate(&flag);
            logSetPrintDate(flag);
        }
        
        // If 'printFileLine' parameter is specified...
        if (debugCmd.IsDefinedPrintFileLine() == mcsTRUE)
        {
            // Set new level
            mcsLOGICAL flag;
            debugCmd.GetPrintFileLine(&flag);
            logSetPrintFileLine(flag);
        }
        
        // Send an hand-checking message
        msg.SetBody("OK");
        PrepareReply(msg);
        SendReply(msg);
    }  
    // Else if the received command is a PROCLIST request...
    else if (strcmp(msg.GetCommand(), msgPROCLIST_CMD_NAME) == 0)
    {
        msgPROCLIST_CMD procListCmd(msg.GetCommand(), msg.GetBody());

        // Parses command
        if (procListCmd.Parse() == mcsFAILURE)
        {
            PrepareReply(msg);
            SendReply(msg);
            return mcsFAILURE;
        }
        
        // Get the list of connected processes
        msg.ClearBody();
        for (unsigned int el = 0; el < _processList.Size(); el++)
        {
            msgPROCESS *process; 
            process = _processList.GetNextProcess((mcsLOGICAL)(el==0));
            if (process != NULL)
            {
                mcsSTRING256 buffer;
                sprintf(buffer, "%s %d %c", 
                        process->GetName(), process->GetId(),
                        (process->IsUnique() == mcsTRUE) ? 'T' : 'F'); 
                msg.AppendStringToBody(buffer);

                // If it is not the last process, add separator
                if (el < (_processList.Size() - 1))
                {
                    msg.AppendStringToBody(", ");
                }
            }
        }

        // Send an hand-checking message
        PrepareReply(msg);
        SendReply(msg);
    }  
    // Else if the received command is a CLOSE request...
    else if (strcmp(msg.GetCommand(), msgCLOSE_CMD_NAME) == 0)
    {
        // Send an hand-checking message
        msg.SetBody("OK");
        PrepareReply(msg);
        SendReply(msg);

        // Close the connection
        logTest("Connection with process '%s' closed", msg.GetSender());
        msgPROCESS *sender;
        sender = _processList.GetProcess(msg.GetSender(), msg.GetSenderId());
        if (_processList.Remove(sender->GetDescriptor()) == mcsFAILURE)
        {
            errCloseStack();
        }
    }
    // Else if the received command is a EXIT request...
    else if (strcmp(msg.GetCommand(), msgEXIT_CMD_NAME) == 0)
    {
        // Send an hand-checking message
        msg.SetBody("OK");
        PrepareReply(msg);
        SendReply(msg);

        // Wait 1 second to have enough time to get the 'EXIT' query answer
        sleep(1);

        // Quit msgManager process
        logTest("msgManager exiting...");
        exit(EXIT_SUCCESS);
    }
    // Else if the received command is an unknown request...
    else
    {
        logError("'%s' received an unknown '%s' command",
                 msg.GetSender(),
                 msg.GetCommand());

        errAdd(msgERR_CMD_NOT_SUPPORTED,  msg.GetCommand());
        PrepareReply(msg);
        SendReply(msg);

        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/**
 * Release all the processes waiting for a given process answer.
 *
 * Sends an error reply to all the previously waiting process, specifying that
 * the given waited process has abnornaly exited.
 * 
 * \param procDescriptor socket descriptor of the dead process.
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT msgMANAGER::ReleaseWaitingProcess(mcsINT32 procDescriptor)
{
    logExtDbg("msgMANAGER::ReleaseWaitingProcess()");

    // For each waited command replies 
    list< pair<int, msgMESSAGE> >::iterator iter;
    for (iter = _waitedCmdReplyList.begin();
         iter != _waitedCmdReplyList.end(); ++iter)
    {
        // If process descriptors match
        if ((*iter).first == procDescriptor)
        {
            // Send error reply
            errAdd(msgERR_PROC_ABNORMALLY_EXIT,
                   ((*iter).second).GetRecipient());
            PrepareReply((*iter).second);
            SendReply((*iter).second);            
        }
    }
    
    // Purge waited command replies
    iter = _waitedCmdReplyList.begin();
    while (iter != _waitedCmdReplyList.end())
    {
        // If the callback list is empty 
        if ((*iter).first == procDescriptor)
        {
            // Remove it, and restart at the beginning of the list
            _waitedCmdReplyList.erase(iter);
            iter = _waitedCmdReplyList.begin();
        }
        else
        {
            iter++;
        }
    }

    logDebug("Nb waited command replies = %d", _waitedCmdReplyList.size()); 

    return mcsSUCCESS;
}

/**
 * Remove received reply from waited command replies.
 *
 * It removes the received reply from the list containing all the awaited
 * replies.
 * 
 * \param commandId the command identifier of the received reply.
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT msgMANAGER::RemoveProcessWaitingFor(mcsINT32 commandId)
{
    logExtDbg("msgMANAGER::RemoveProcessWaitingFor()");

    // Remove received reply from list
    list< pair<int, msgMESSAGE> >::iterator iter;
    iter = _waitedCmdReplyList.begin();
    while (iter != _waitedCmdReplyList.end())
    {
        // If the callback list is empty 
        if (((*iter).second).GetCommandId() == commandId)
        {
            // Remove it, and restart at the beginning of the list
            _waitedCmdReplyList.erase(iter);
            iter = _waitedCmdReplyList.begin();
        }
        else
        {
            iter++;
        }
    }

    logDebug("Nb waited command replies = %d", _waitedCmdReplyList.size()); 

    return mcsSUCCESS;
}



/*
 * Private methods
 */


/*___oOo___*/
