/*******************************************************************************
* JMMC project
* 
* "@(#) $Id: logManager.c,v 1.3 2004-11-10 10:04:11 gzins Exp $"
*
*
* who       when                 what
* --------  -----------  -------------------------------------------------------
* lafrasse  30-Jun-2004  Forked and ported (Windows to Linux) from CILAS Soft
* lafrasse  23-Aug-2004  Changed var. name logManagerPortNumber to portNumber
* gzins     10-Nov-2004  Changed default log file to $MCSDATA/log/logfile
*
*
*******************************************************************************/

/**
 * \file
 * \e \<logManager\> - logging daemon receiving log messages from extern
 * programs and libraries through a network connection.
 *
 * \b Synopsis:\n
 * \e \<logManager\> [\e \<-f\> path/to/logfile]
 *                   [\e \<-s\> maximum log file size] 
 *                   [\e \<-p\> listened port number] 
 *
 * \b Details:\n
 * \e \<logManager\> writes log messages in \e '$MCSDATA/log/logfile' (or at a
 * user-specified path), until the log file size reaches 1MBytes (or a
 * user-specified value). Then, the log file is suffixed with '.OLD',
 * overwritting any previous file named like this, and new empty log file is
 * then created. \e \<logManager\> listen on the network port number 8791 (or on
 * a user-specified one).
 * 
 * \b Files:\n
 * \li \e \<$MCSDATA/log/logfile\> : default log file
 * \li \e \<$MCSDATA/log/logfile.OLD\> : default old log file
 *
 * \warning Old log files are overwritten each time the current log file size
 * reaches 1Mbytes (or user-specified maximum size).
 */


/* 
 * System Headers 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>

#include <errno.h>
#include <getopt.h>
#include <ctype.h>
#include <sys/stat.h>


/*
 * MCS Headers 
 */

#include "mcs.h"


/*
 * Local Headers 
 */

#include "log.h"
#include "logPrivate.h"


/*
 * Constants
 */

/**
 * logManager default 1MBytes maximum log file size (expressed in bytes).
 *
 * This value can be overwritten by a user-specified one using the '-s' option
 * when launching the logManger on CLI.
 */
#define logMANAGER_DEFAULT_MAX_FILE_SIZE 1048579

/**
 * logManager default old log file suffix.
 */
#define logMANAGER_DEFAULT_OLD_LOG_FILE_PATH_SUFFIX ".OLD"


/*
 * Main
 */

int main(int argc, char** argv)
{
    /* Init MCS services */
    mcsInit(argv[0]);

    /* Character buffers */
    mcsBYTES256        hostName, logFilePath, oldLogFilePath, logMsg;

    /* Files stuff */
    char*              mcsDataPath = NULL;
    FILE               *logFile = NULL;
    mcsUINT32          logMaxFileSize = logMANAGER_DEFAULT_MAX_FILE_SIZE;

    /* Network stuff */
    mcsUINT32          portNumber = logMANAGER_DEFAULT_PORT_NUMBER;
    int                sock = 0;
    struct hostent     *hp = NULL;
    struct sockaddr_in from;
    int                fromLen = 0;
    unsigned int       loggedMsgNumber = 0;

    /* 'getopt()' stuff */
    int index;
    int optionChr;
    opterr = 0;
    


    /* Try to initialize the logFilePath variable */
    if (memset(&logFilePath, '\0', sizeof(logFilePath)) == NULL)
    {
        logDisplayError("could not initalize logFilePath value");
    }

    /* Try to get the $MCSDATA Environment Variable value */
    mcsDataPath = getenv("MCSDATA");
    if (mcsDataPath == NULL)
    {
        logDisplayError("could not resolve $MCSDATA Env Var");
    }
    
    /* Try to copy $MCSDATA Env Var at the beginning of the log file path */
    if (strcpy(logFilePath, mcsDataPath) == NULL)
    {
        logDisplayError("could not copy $MCSDATA Env Var value at the beginning of logFilePath");
    }
    
    /* Try to append the rest of the path */
    if (strcat(logFilePath, "/log/logfile") == NULL)
    {
        logDisplayError("could not append the default path end to logFilePath");
    }

    /* Try to copy the log file path in the old log file path */
    if (strcpy(oldLogFilePath, logFilePath) == NULL)
    {
        logDisplayError("could not copy logFilePath in oldLogFilePath");
    }

    /* Try to append the default old log file path suffix */
    if (strcat(oldLogFilePath, logMANAGER_DEFAULT_OLD_LOG_FILE_PATH_SUFFIX)
        == NULL)
    {
        logDisplayError("could not append the default oldLogFilePath suffix");
    }



    /* Analyzing CLI received parameters */
    while ((optionChr = getopt(argc, argv, "f:s:p:")) != -1)
    {
        switch (optionChr)
        {
            /* Copy the received log file path in place of the default value */
            case 'f':
                /* Try to copy user-specified file path in log file path */
                if (strcpy(logFilePath, optarg) == NULL)
                {
                    logDisplayError("could not copy user-specified file path in logFilePath");
                }
                break;

            /* Copy the received max file size in place of the default value */
            case 's':
                sscanf(optarg, "%d", &logMaxFileSize);
                if (logMaxFileSize <= 0)
                {
                    logMaxFileSize = logMANAGER_DEFAULT_MAX_FILE_SIZE;
                    logDisplayMessage("received an invalid logMaxFileSize value as parameter - will use the default '%d' value instead", logMaxFileSize);
                }
                break;

            /* Copy the received port number in place of the default value */
            case 'p':
                sscanf(optarg, "%d", &portNumber);
                if (portNumber < 0 || portNumber > 65535)
                {
                    portNumber = logMANAGER_DEFAULT_PORT_NUMBER;
                    logDisplayMessage("received an out of range port value as parameter - will use the default '%d' value instead", portNumber);
                }
                break;

            /* Treat all the other options as errors */
            case '?':
                if (isprint(optopt))
                {
                    logDisplayError("received bad option parameter(s)");
                }
                else
                {
                    logDisplayError("received an unknown option character `\\x%x' as parameter", optopt);
                }

            default:
                logDisplayError("CLI option parsing assertion failed");
        }
    }
    
    /* Treat all the other parameters as errors */
    for (index = optind; index < argc; index++)
    {
        logDisplayMessage("received more parameters than needed - will ignore them");
    }



    /* Try to open the log file in appending mode */
    logFile = fopen(logFilePath, "a");
    if (logFile == NULL)
    {
        logDisplayError("fopen() failed - could not open/create log file '%s'",
                        logFilePath);
    }



    /* Try to open a DATAGRAM socket */
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
    {
        logDisplayError("socket() failed - %s", strerror(errno));
    }

    /* Try to get local host name */
    if (logGetHostName((char *)hostName, (sizeof(hostName) -1)) == FAILURE)
    {
        logDisplayError("could not get the host name - %s", strerror(errno));
    }

    /* Try to get the corresponding 'hostent' structure */
    hp = gethostbyname((char *)hostName);
    if (hp == NULL ) 
    {
        logDisplayError("gethostbyname() failed - %s", strerror(errno));
    }


    /* Try to initialize the sockaddr_in structure */
    if (memset(&from, '\0', sizeof(from)) == NULL)
    {
        logDisplayError("could not initalize sockaddr_in structure");
    }


    /* Try to copy host information inside the sockaddr_in structure */
    if ((memcpy(&(from.sin_addr), hp->h_addr, hp->h_length)) == NULL)
    {
        logDisplayError("could not set sockaddr_in sin_addr value");
    }
    
    from.sin_family = hp->h_addrtype;

    /* Converts the port number from host byte order to network byte order */
    from.sin_port = htons(portNumber); 

    /* Try to associate the local address with the socket */
    if (bind(sock, (struct sockaddr *)&from, sizeof(from)) == -1)
    {
        logDisplayError("bind() failed - %s", strerror(errno));
    }



    /* Display logManager start time and parameters */
    logDisplayMessage(
    "started listening on port '%d', will write up to '%d' bytes in file '%s'"
    , portNumber, logMaxFileSize, logFilePath);



    /* Loop forever */
    fromLen = sizeof(from);
    while (1)
    {
        /* Try to get the file size */
        struct stat logFileStats;
        if (stat(logFilePath, &logFileStats) == -1)
        {
            logDisplayMessage("stat() failed - %s", strerror(errno));
        }
        
        /* If the log file size is greater than the default or specified one */
        if (logFileStats.st_size > logMaxFileSize)
        {
            /* Try to close log File */
            if (fclose(logFile) == -1)
            {
                /* Close the socket */
                close(sock);
            
                logDisplayError("fclose() failed - could not close log file '%s'", logFilePath);
            }

            /* Try to rename it with '.OLD' extension */
            if (rename(logFilePath, oldLogFilePath) == -1)
            {
                logDisplayError("rename() failed - %s", strerror(errno));
            }

            /* Try to open the log file in overwrite mode */
            logFile = fopen(logFilePath, "w");
            if (logFile == NULL)
            {
                /* Close the socket */
                close(sock);
            
                logDisplayError("fopen() failed - could not re-open/re-create log file '%s'", logFilePath);
            }
        }
        


        /* Try to re-initialize temporary log message buffer */
        if (memset(&logMsg,'\0',sizeof(logMsg)) == NULL)
        {
            logDisplayError("could not re-initialize temporary log message buffer");
        }
        
        /* Wait for received data, then put in temporary log message buffer */
        if (recvfrom(sock, (char *)logMsg, sizeof(logMsg), 0,
                    (struct sockaddr *)&from, &fromLen) == -1)
        {
            logDisplayMessage("recvfrom() failed - %s", strerror(errno));
        }



        /* Try to store new message into the log file */
        if (fprintf(logFile,"%s\n", logMsg) < 0)
        {
            logDisplayMessage("fprintf(\"%s\") failed", logMsg);
        }

        /* Empty temporary fprintf() system buffer in our log file */
        fflush(logFile);



        /* Inform that 10 new messages have been logged */
        loggedMsgNumber++;
        if ((loggedMsgNumber % 10) == 0)
        {
            logDisplayMessage("logged %d messages", loggedMsgNumber);
        }
    }
    /* End forever */



    /* Close the socket */
    close(sock);

    /* Close log File */
    fclose(logFile);



    mcsExit();
    exit(EXIT_SUCCESS);
}


/*___oOo___*/
