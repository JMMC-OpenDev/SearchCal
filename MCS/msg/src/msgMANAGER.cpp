/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgMANAGER.cpp,v 1.2 2004-12-08 17:37:29 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     06-Dec-2004  Created
* gzins     08-Dec-2004  Updated to support several processes with same name  
*
*
*******************************************************************************/

/**
 * \file
 * msgMANAGER class definition.
 */

static char *rcsId="@(#) $Id: msgMANAGER.cpp,v 1.2 2004-12-08 17:37:29 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers 
 */
#include <iostream>
using namespace std;
#include <sys/ioctl.h>

/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"

/*
 * Local Headers 
 */
#include "msgMANAGER.h"
#include "msgMCS_ENVS.h"
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
 * Initialization of manager.
 *
 * It registers application to MCS services, parses the
 * command-line parameters and open the connection socket.
 *
 * \return SUCCESS, or FAILURE if an error occurs.
 */
mcsCOMPL_STAT msgMANAGER::Init(int argc, char *argv[])
{
    logExtDbg("msgMANAGER::Init()");

    // Initialiaze MCS services
    if (mcsInit(argv[0]) != SUCCESS)
    {
        return FAILURE;
    }

    // Parses command-line options 
    if (ParseOptions(argc, argv) != SUCCESS)
    {
        return FAILURE;
    }
    
    // Port number of the current environment
    msgMCS_ENVS envList;
    mcsINT32    portNumber;
    portNumber = envList.GetPortNumber();
    if (portNumber == -1)
    {
        return FAILURE;
    }

    // Open connection socket
    logTest("Environment '%s', port : %d", mcsGetEnvName(), portNumber);
    if (_connectionSocket.Open(portNumber) == FAILURE)
    {
        return FAILURE;
    }

    return SUCCESS;
}

/**
 * Main loop.
 *
 */
mcsCOMPL_STAT msgMANAGER::MainLoop()
{
    logExtDbg("msgMANAGER::MainLoop()");

    /* For ever... */
    for(;;)
    {
        fd_set   readMask;

        // Set the set of descriptors for reading
        FD_ZERO(&readMask);

        // For each connected processes
        for (unsigned int el = 0; el < _processList.Size(); el++)
        {
            // Get the socket descriptor
            int sd = _processList.GetNextProcess((el==0))->GetDescriptor();

            // Add it to the list of descriptors for reading
            if (sd != -1)
            {
                FD_SET(sd, &readMask);
            }
        }
        // End for

        // Add connection socket
        FD_SET(_connectionSocket.GetDescriptor(), &readMask);
        
        // Wait for message
        int status;
        logInfo("Waiting message...");
        status = select(msgMANAGER_SELECT_WIDTH, &readMask, (fd_set *) NULL,
                        (fd_set *) NULL, (struct timeval *) NULL );
        
        /* If an error occured in the select() function... */
        if (status == -1)
        {
            /* Raise an error */
            logError("ERROR : select() failed - %s\n", strerror(errno));
            
            /* Wait 1 second in order not to loop infinitly (and thus using
             * 100% of the CPU) if it is a recurrent error
             */
            sleep(1);
        }

        /* If a new connection demand was received... */
        if (FD_ISSET(_connectionSocket.GetDescriptor() , &readMask))
        {
            logInfo("Connection demand received...");

            /* Init the new connection */
            if (SetConnection() == FAILURE)
            {
                errCloseStack();
            }
        }
        else /* A message from an already connected process was received... */
        {
            logInfo("Message received...");
            mcsINT32   nbBytesToRead ;

            // For each connected processes
            for (unsigned int el = 0; el < _processList.Size(); el++)
            {
                // Get the socket descriptor
                msgPROCESS *process = _processList.GetNextProcess((el==0));

                // Add it to the list of descriptors for reading
                if ((process->GetDescriptor() != -1) && 
                    FD_ISSET(process->GetDescriptor(), &readMask))
                {
                    // If there is some data to be read...
                    ioctl(process->GetDescriptor(), FIONREAD,
                          (unsigned long *)&nbBytesToRead);
                    if (nbBytesToRead != 0)
                    {
                        // Read the new message
                        msgMESSAGE msg;
                        if (process->Receive(msg, 1000) == FAILURE)
                        {
                            errCloseStack();
                        }
                        else
                        {
                            // If the new message is a command...
                            if (msg.GetType() == msgTYPE_COMMAND)
                            {
                                // If the command is intended to msgManger...
                                if ((strcmp(msg.GetRecipient(), "msgManager")
                                     == 0)
                                    ||
                                    (strcmp (msg.GetCommand(), msgPING_CMD)
                                     == 0))
                                {
                                    /* Handle the received command */
                                    if (HandleCmd(msg) == FAILURE)
                                    {
                                        errCloseStack();
                                    }
                                }
                                else // If the command is not for msgManager
                                {
                                    // Forward to the destination process
                                    if (Forward(msg) == FAILURE)
                                    {
                                        errCloseStack();
                                    }
                                }
                            }
                            else // If the message is a reply...
                            {
                                // Send reply to the sender process
                                if (SendReply(msg, msg.IsLastReply()) == FAILURE)
                                {
                                    errCloseStack();
                                }
                            }
                        }
                    }
                    else // If there was nothing to read...
                    {
                        logWarning("Connection with '%s' process lost",
                                   process->GetName());

                        /* Close the connection */
                        if (_processList.Remove(process->GetDescriptor())
                            == FAILURE)
                        {
                            errCloseStack();
                        }
                    }
                }
            }
            // End for
        }

    } // For ever end

    return SUCCESS;
}

/*
 * Protected methods
 */
/**
 * Parses the options of the application.
 *
 * It parses the standard command-line options.
 * \param argc count of the arguments supplied to the method
 * \param argv array of pointers to the strings which are those arguments
 * processed or not.
 *
 * \return On success, SUCCESS is returned. On error, FAILURE is returned, and
 * error message is printed out accordingly.
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
                    return FAILURE;
                }
                logSetFileLogLevel((logLEVEL)level);
            }
            else
            {
                logError ("%s: Option %s requires an argument",
                          mcsGetProcName(), argv[optInd]);
                return FAILURE;
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
                    return FAILURE;
                }
                logSetStdoutLogLevel((logLEVEL)level);
            }
            else
            {
                logError ("%s: Option %s requires an argument",
                          mcsGetProcName(), argv[optInd]);
                return FAILURE;
            }
        }
        // Else if action level specified
        else if (strcmp(argv[optInd], "-a") == 0)
        {
            // Set new action log level
            if ((optInd + 1) < argc)
            {
                optInd += 1;
                optarg = argv[optInd];
                if ( sscanf (optarg, "%d", &level) != 1)
                {
                    logError ("%s: Argument to option %s is invalid: '%s'",
                              mcsGetProcName(), argv[optInd-1], optarg);
                    return FAILURE;
                }
                logSetActionLogLevel((logLEVEL)level);
            }
            else
            {
                logError ("%s: Option %s requires an argument",
                          mcsGetProcName(), argv[optInd]);
                return FAILURE;
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
            return FAILURE;
        }
    }
    return SUCCESS;
}

/**
 * Usage of the standard options/arguments.
 *
 * This method gives information about the standard options listed above.
 *
 * \return SUCCESS 
 */
mcsCOMPL_STAT msgMANAGER::Usage(void)
{
    cout << "Usage:" << mcsGetProcName() << " [OPTIONS]"<< endl;
    cout <<" Standard options: -l <level>   set file log level" << endl;
    cout <<"                   -v <level>   set stdout log level" << endl;
    cout <<"                   -a <level>   set action log level" << endl;
    cout <<"                   -h           print this help" << endl;
    cout <<"                   -version     print the version number of the ";
    cout <<"software" << endl;
    cout <<"                   -noDate      turn off the display of date";
    cout <<" in stdout log" << endl;
    cout <<"                                messages" << endl;
    cout <<"                   -noFileLine  turn off the display of file name";
    cout <<" and line number" << endl;
    cout <<"                                in stdout log messages" << endl; 

    return SUCCESS;
}

/**
 * Prints the version number of the software.
 * 
 */
const char *msgMANAGER::GetSwVersion(void)
{
    return "SW version number no set"; 
}

/**
 * Establish connection with a process.
 *
 * Verify that the new process name is unic, otherwise reject the connection
 * request.
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT msgMANAGER::SetConnection()
{
    logExtDbg("msgMANAGER::SetConnection()");

    /* Accept the new connection */
    msgPROCESS *newProcess = new msgPROCESS();
    if (_connectionSocket.Accept(*newProcess) != SUCCESS)
    {
        errCloseStack();
    }
    else
    {
        /* Receive the registering command */
        msgMESSAGE msg;
        if (newProcess->Receive(msg, 1000) == FAILURE)
        {
            /* Close the connection */
            delete(newProcess);
            errCloseStack();
        }
        else
        {
            /* If the registering command is received... */
            if (strcmp(msg.GetCommand(), msgREGISTER_CMD) == 0)
            {
                logTest("Connection demand received from %s ...",
                        msg.GetSender());

                logInfo("'%s' connection accepted", msg.GetSender());

                /* Add the new process to the process list */
                newProcess->SetName(msg.GetSender());
                if (_processList.AddAtTail(newProcess) == FAILURE)
                {
                    errCloseStack();
                }

                /* Send a registering validation messsage */
                msg.SetBody("OK", 0);
                if (SendReply(msg, mcsTRUE) == FAILURE)
                {
                    errCloseStack();
                }
            }
            else /* Wrong message received... */
            {
                /* Close the connection */
                logWarning("Received a '%s' message instead of '%s'",
                           "- '%s' process connection refused",
                           msg.GetCommand(), msgREGISTER_CMD,
                           msg.GetSender());
                delete (newProcess);
            }
        }
    }
    return SUCCESS;
}

/**
 * Forward command to its recepient process.
 *
 * \param msg the message to be forwarded
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT msgMANAGER::Forward(msgMESSAGE &msg)
{
    msgPROCESS *recipient;

    logExtDbg("msgMANAGER::Forward()");
    logInfo("Received '%s' command from '%s' for '%s'", msg.GetCommand(),
            msg.GetSender(), msg.GetRecipient());

    /* Try to find the recipient process in the process list */
    recipient = _processList.GetProcess(msg.GetRecipient());
    if (recipient == NULL)
    {
        /* Raise the error to the sender */
        errAdd(msgERR_RECIPIENT_NOT_CONNECTED, msg.GetRecipient(),
               msg.GetCommand());
        if (SendReply(msg, mcsTRUE) == FAILURE)
        {
            errCloseStack();
        }
    }
    else
    {
        /* If the command could not be delivered to the recipient process... */
        if (recipient->Send(msg) == FAILURE)
        {
            /* Try to report this to the sender */
            if (SendReply(msg, mcsTRUE) == FAILURE)
            {
                errCloseStack();
            }
            return FAILURE;
        }
    }
       
    return SUCCESS;
}

/**
 * Send a reply message.
 *
 * \param msg the message to reply
 * \param lastReply flag to specify if the current message is the last one or
 * not
 * \param sender
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT msgMANAGER::SendReply (msgMESSAGE &msg,
                                     mcsLOGICAL lastReply,
                                     msgPROCESS *sender)
{
    logExtDbg("msgMANAGER::SendReply()");

    // Build the reply message header
    msg.SetLastReplyFlag(lastReply);

    // If there is no error in the MCS error stack
    if (errStackIsEmpty() == mcsTRUE)
    {
        // Set message type to REPLY
        msg.SetType(msgTYPE_REPLY);
    }
    else
    {
        // Put the MCS error stack data in the message body
        char errStackContent[msgBODYMAXLEN];
        if (errPackStack(errStackContent, msgBODYMAXLEN) == FAILURE)
        {
            return FAILURE;
        }
        msg.SetBody(errStackContent, msgBODYMAXLEN);

        // Set message type to ERROR_REPLY
        msg.SetType(msgTYPE_ERROR_REPLY);

        // Empty MCS error stack
        errResetStack();
    }

    logTest("Sending '%s' answer : %s", msg.GetCommand(), msg.GetBodyPtr());

    // Get sender
    if (sender == NULL)
    {
        sender = _processList.GetProcess(msg.GetSender(), msg.GetSenderId());
    }
    if (sender == NULL)
    {
        /* Raise the error to the sender */
        errAdd(msgERR_SENDER_NOT_CONNECTED, msg.GetSender(),
               msg.GetCommand());
        return FAILURE;
    }
    
    return (sender->Send(msg));
}

/**
 * Try to manage msgManager own commands.
 *
 * Those commands are :
 * \li CLOSE - close the connection with msgManager (internal use)
 * \li DEBUG - log messages management
 * \li EXIT - quit msgManager
 * \li PING - test if a process is connected to msgManager
 * \li VERSION - give back the current msgManager version number
 *
 * \param msg a received command
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT msgMANAGER::HandleCmd (msgMESSAGE &msg)
{
    logExtDbg("msgMANAGER::HandleCmd()");
    logInfo("Received internal '%s' command from '%s'", msg.GetCommand(),
            msg.GetSender());
    
    /* If the received command is a PING request... */ 
    if (strcmp(msg.GetCommand(), msgPING_CMD) == 0)
    {
        /* If the command recipient is connected... */
        if ((strcmp (msg.GetRecipient(), "msgManager") == 0) ||
            (_processList.GetProcess(msg.GetRecipient()) != NULL))
        {
            /* Build an OK reply message */
            msg.SetBody("OK");
        }
        else
        {
            /* Raise the error */
            errAdd(msgERR_PING, msg.GetRecipient());
        }

        /* Try to send the built reply message */
        if (SendReply(msg, mcsTRUE) == FAILURE)
        {
            errCloseStack();
        }
    } 
    /* If the received command is a VERSION request... */ 
    else if (strcmp(msg.GetCommand(), msgVERSION_CMD) == 0)
    {
        /* Try to reply the msgManager CVS verson number */
        mcsSTRING256 buffer;
        memset(buffer, '\0', sizeof(buffer));
        sprintf(buffer, rcsId);
        msg.SetBody(buffer);
        if (SendReply(msg, mcsTRUE) == FAILURE)
        {
            errCloseStack();
        }
    }  
    /* If the received command is a DEBUG request... */ 
    else if (strcmp(msg.GetCommand(), msgDEBUG_CMD) == 0)
    {
#if 0
        mcsINT32   index;
        mcsLOGICAL log, verbose, printDate, printFileLine;
        mcsINT32   logLevel, verboseLevel;
        
        /* Try to analyze the received command parameter */
        cmdPARAM_LIST *paramList;
        paramList = cmdParseParam(msgDEBUG_CMD, msgGetBodyPtr(msg));
        if (paramList == NULL)
        {
            msgSendReplyTo(process->sd, msg, mcsTRUE);
            return FAILURE;
        } 
        
        /* If the 'log' parameter is specified... */
        index = cmdGetParamIndex(paramList, "log");
        if (index != -1)
        {
            /* Try to toogle file log message recording */
            if (cmdGetLogicalParamByIndex(paramList, index, &log) == FAILURE)
            {
                msgSendReplyTo(process->sd, msg, mcsTRUE);
                return FAILURE;
            }
            else
            {
                if (log == mcsTRUE)
                {
                    logEnableFileLog();
                }
                else
                {
                    logDisableFileLog();
                }
            }
        }

        /* If the 'logLevel' parameter is specified... */
        index = cmdGetParamIndex(paramList, "logLevel");
        if (index != -1)
        {
            /* Try to change the file log level accordinaly */
            if (cmdGetIntegerParamByIndex(paramList, index, &logLevel)
                == FAILURE)
            {
                msgSendReplyTo(process->sd, msg, mcsTRUE);
                return FAILURE;
            }
            else
            {
                logSetFileLogLevel(logLevel);
            }
        }

        /* If the 'verbose' parameter is specified... */
        index = cmdGetParamIndex(paramList, "verbose");
        if (index != -1)
        {  
            /* Try to toogle stdout log message printing */
            if (cmdGetLogicalParamByIndex(paramList, index, &verbose)
                != FAILURE)
            {
                msgSendReplyTo(process->sd, msg, mcsTRUE);
                return FAILURE;
            }
            else
            {
                if (verbose == mcsTRUE)
                {
                    logEnableStdoutLog();
                }
                else
                {
                    logDisableStdoutLog();
                }
            }
        }

        /* If the 'verboseLevel' parameter is specified... */
        index = cmdGetParamIndex(paramList, "verboseLevel");
        if (index != -1)
        {
            /* Try to change the stdout log level accordinaly */
            if (cmdGetIntegerParamByName(paramList, "verboseLevel", 
                                         &verboseLevel) == FAILURE)
            {
                msgSendReplyTo(process->sd, msg, mcsTRUE);
                return FAILURE;
            }
            else
            {
                logSetStdoutLogLevel(verboseLevel);
            }
        }

        /* If the 'printDate' parameter is specified... */
        index = cmdGetParamIndex(paramList, "printDate");
        if (index != -1)
        {  
            /* Try to toogle log date printing */
            if (cmdGetLogicalParamByIndex(paramList, index, &printDate)
                == FAILURE)
            {
                msgSendReplyTo(process->sd, msg, mcsTRUE);
                return FAILURE;
            }
            else
            {
                logSetPrintDate(printDate);
            }
        }
                
        /* If the 'printFileLine' parameter is specified... */
        index = cmdGetParamIndex(paramList, "printFileLine");
        if (index != -1)
        {  
            /* Try to toogle log line printing */
            if (cmdGetLogicalParamByIndex(paramList, index,  &printFileLine)
                != FAILURE)
            {
                msgSendReplyTo(process->sd, msg, mcsTRUE);
                return FAILURE;
            }
            else
            {
                logSetPrintFileLine(printFileLine);
            }
        }

        /* Verify that there is no unrecognized parameter for the command */
        if (cmdCheckUnusedParams(paramList) == FAILURE)
        {
            msgSendReplyTo(process->sd, msg, mcsTRUE);
            return FAILURE;
        }

#endif
        /* Send an hand-checking message */
        msg.SetBody("OK");
        if (SendReply(msg, mcsTRUE) == FAILURE)
        {
            errCloseStack();
        }
    }  
    /* If the received command is a CLOSE request... */
    else if (strcmp(msg.GetCommand(), msgCLOSE_CMD) == 0)
    {
        /* Try to send an hand-checking message */
        msg.SetBody("OK");
        if (SendReply(msg, mcsTRUE) == FAILURE)
        {
            errCloseStack();
        }

        /* Try to close the connection */
        logInfo("Connection with process '%s' closed", msg.GetSender());
        msgPROCESS *sender;
        sender = _processList.GetProcess(msg.GetSender(), msg.GetSenderId());
        if (_processList.Remove(sender->GetDescriptor()) == FAILURE)
        {
            errCloseStack();
        }
    }
    /* If the received command is a EXIT request... */
    else if (strcmp(msg.GetCommand(), msgEXIT_CMD) == 0)
    {
        /* Try to send an hand-checking message */
        msg.SetBody("OK");
        if (SendReply(msg, mcsTRUE) == FAILURE)
        {
            errCloseStack();
        }
        
        /* Quit msgManager process */
        logInfo("msgManager exiting...");
        exit(EXIT_SUCCESS);
    }
    /* If the received command is an unknown request... */
    else
    {
        logError("'%s' received an unknown '%s' command",
                 msg.GetSender(),
                 msg.GetCommand());

        errAdd(msgERR_CMD_NOT_SUPPORTED,  msg.GetCommand());
        if (SendReply(msg, mcsTRUE) == FAILURE)
        {
            errCloseStack();
        }

        return FAILURE;
    }

    return SUCCESS;
}

/*
 * Private methods
 */


/*___oOo___*/
