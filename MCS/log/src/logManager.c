/*******************************************************************************
 * JMMC project
 * 
 * "@(#) $Id: logManager.c,v 1.7 2005-02-15 08:25:13 gzins Exp $"
 *
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.6  2005/02/15 08:18:43  gzins
 * Changed SUCCESS/FAILURE to mcsSUCCESS/mcsFAILURE
 *
 * Revision 1.5  2005/01/26 17:28:13  lafrasse
 * Added automatic CVS history, refined user documentation, removed all
 * ActionLog-related code, and changed mcsSUCCESS in mcsSUCCESS and mcsFAILURE
 * in mcsFAILURE
 *
 * gzins     10-Nov-2004  Replaced logDisplayError by logPrintErrMessage
 *                        Changed access mode of log file to rw-rw-rw-
 *
 * gzins     10-Nov-2004  Changed default log file to $MCSDATA/log/logfile
 *
 * lafrasse  23-Aug-2004  Changed var. name logManagerPortNumber to portNumber
 *
 * lafrasse  30-Jun-2004  Forked and ported (Windows to Linux) from CILAS Soft
 *
 ******************************************************************************/

/**
 * \file
 * \e \<logManager\> - \em file logging daemon, receiving log messages from
 * extern programs and libraries through a network connection.
 *
 * \b Synopsis:\n
 * \e \<logManager\> [\e \<-f\> path/to/logfile]
 *                   [\e \<-s\> maximum log file size] 
 *                   [\e \<-p\> listened port number] 
 *
 * \b Details:\n
 * \e \<logManager\> writes log messages in \e \<$MCSDATA/log/logfile\> (or at a
 * user-specified path), until the log file size reaches 1MBytes (or a
 * user-specified value). Then, the log file is suffixed with '.old',
 * overwritting any previous file named like this, and a new empty log file is
 * then created.\n
 * \e \<logManager\> listen on the network port number \e 8791 (or on a
 * user-specified one).
 * 
 * \b Files:\n
 * \li \e \<$MCSDATA/log/logfile\>     : default log file;
 * \li \e \<$MCSDATA/log/logfile.old\> : default old log file.
 * \n\n
 *
 * \warning Old log files are overwritten each time the current log file size
 * reaches 1Mbytes (or a user-specified maximum size).
 * \n\n
 *
 */

/* 
 * System Headers 
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

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

/**
 * logManager default 1MBytes maximum log file size (expressed in bytes).
 *
 * This value can be overwritten by a user-specified one using the '-s' option
 * when launching the logManger on a command-line interface (e.g console).
 */
#define logMANAGER_DEFAULT_MAX_FILE_SIZE 1024*1024 /* 1 Mbytes */

/*
 * Main
 */

int main(int argc, char** argv)
{
    /* Character buffers */
    mcsBYTES256        hostName, logFilePath, oldLogFilePath, logMsg;

    /* Files stuff */
    char*              mcsDataPath = NULL;
    FILE               *logFile = NULL;
    mcsUINT32          logMaxFileSize = logMANAGER_DEFAULT_MAX_FILE_SIZE;
    mcsINT32           newLogMaxFileSize;

    /* Network stuff */
    mcsUINT32          portNumber = logMANAGER_DEFAULT_PORT_NUMBER;
    mcsINT32           newPortNumber;
    int                sock = 0;
    struct hostent     *hp = NULL;
    struct sockaddr_in from;
    int                fromLen = 0;
    unsigned int       loggedMsgNumber = 0;

    /* 'getopt()' stuff */
    int index;
    int optionChr;
    opterr = 0;

    /* Init MCS services */
    mcsInit(argv[0]);

    /* Initialize the logFilePath variable */
    memset(&logFilePath, '\0', sizeof(logFilePath));

    /* Get the $MCSDATA Environment Variable value */
    mcsDataPath = getenv("MCSDATA");
    if (mcsDataPath == NULL)
    {
        logPrintErrMessage("could not resolve $MCSDATA - %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    /* Copy $MCSDATA at the beginning of the log file path */
    strcpy(logFilePath, mcsDataPath);
    
    /* Append the rest of the path */
    strcat(logFilePath, "/log/logfile");

    /* Copy the log file path in the old log file path */
    strcpy(oldLogFilePath, logFilePath);

    /* Append the default old log file path suffix */
    strcat(oldLogFilePath, ".old");

    /* Analyzing CLI received parameters */
    while ((optionChr = getopt(argc, argv, "f:s:p:")) != -1)
    {
        switch (optionChr)
        {
            /* Copy the received log file path in place of the default value */
            case 'f':
                /* Copy user-specified file path in log file path */
                strcpy(logFilePath, optarg);
                break;

            /* Copy the received max file size in place of the default value */
            case 's':
                /* Check format */
                if (sscanf(optarg, "%d", &newLogMaxFileSize) != 1)
                {
                    logPrintErrMessage("invalid -s parameter '%s' - must be a positive integer", optarg);

                    exit(EXIT_FAILURE);
                }
                /* Check value */
                if (newLogMaxFileSize <= 0)
                {
                    logPrintErrMessage("invalid -s parameter value '%d' - must be a positive integer", newLogMaxFileSize);
                    exit(EXIT_FAILURE);
                }

                /* Apply new value */
                logMaxFileSize = newLogMaxFileSize;
                break;

            /* Copy the received port number in place of the default value */
            case 'p':
                /* Check format */
                if (sscanf(optarg, "%d", &newPortNumber) != 1)
                {
                    logPrintErrMessage("invalid -p parameter '%s' - must be a positive integer", optarg);

                    exit(EXIT_FAILURE);
                }
                /* Check value */
                if (newPortNumber < 0 || newPortNumber > 65535)
                {
                    logPrintErrMessage("invalid -p parameter value '%d' - must be a positive integer", newPortNumber);
                    exit(EXIT_FAILURE);
                }
                /* Apply new value */
                portNumber = newPortNumber;
                break;

            /* Treat all the other options as errors */
            case '?':
                if (isprint(optopt))
                {
                    logPrintErrMessage("received unsupported option");
                    exit(EXIT_FAILURE);
                }
                else
                {
                    logPrintErrMessage("received an unknown option character `\\x%x' as parameter", optopt);
                    exit(EXIT_FAILURE);
                }

            default:
                logPrintErrMessage("CLI option parsing assertion failed - %s",
                                   strerror(errno));
                exit(EXIT_FAILURE);
        }
    }
    
    /* Treat all the other parameters as errors */
    for (index = optind; index < argc; index++)
    {
        logPrintErrMessage("received more parameters than needed - will ignore them");
        exit(EXIT_FAILURE);
    }

    /* Try to change access mode to log file */
    chmod(logFilePath, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);

    /* Open the log file in appending mode */
    logFile = fopen(logFilePath, "a");
    if (logFile == NULL)
    {
        logPrintErrMessage("fopen() failed - could not open/create log file '%s' - %s",
                        logFilePath, strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Try to change access mode to log file (in case file did not exist) */
    fflush(logFile);
    chmod(logFilePath, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);

    /* Open a DATAGRAM socket */
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
    {
        logPrintErrMessage("socket() failed - %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Get local host name */
    if (logGetHostName((char *)hostName, (sizeof(hostName) -1)) == mcsFAILURE)
    {
        exit(EXIT_FAILURE);
    }

    /* Get the corresponding 'hostent' structure */
    hp = gethostbyname((char *)hostName);
    if (hp == NULL ) 
    {
        logPrintErrMessage("gethostbyname() failed - %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Initialize the sockaddr_in structure */
    memset(&from, '\0', sizeof(from));

    /* Copy host information inside the sockaddr_in structure */
    memcpy(&(from.sin_addr), hp->h_addr, hp->h_length);
    
    from.sin_family = hp->h_addrtype;

    /* Converts the port number from host byte order to network byte order */
    from.sin_port = htons(portNumber); 

    /* Associate the local address with the socket */
    if (bind(sock, (struct sockaddr *)&from, sizeof(from)) == -1)
    {
        logPrintErrMessage("bind() failed - %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Display logManager start time and parameters */
    logPrintErrMessage
        ("started listening on port '%d', will write up to '%d' bytes in file '%s'"
         , portNumber, logMaxFileSize, logFilePath);

    /* Loop forever */
    fromLen = sizeof(from);
    while (1)
    {
        /* Get the file size */
        struct stat logFileStats;
        if (stat(logFilePath, &logFileStats) == -1)
        {
            logPrintErrMessage("stat() failed - %s", strerror(errno));
        }
        
        /* If the log file size is greater than the default or specified one */
        if (logFileStats.st_size > logMaxFileSize)
        {
            /* Close log File */
            if (fclose(logFile) == -1)
            {
                /* Close the socket */
                close(sock);
            
                logPrintErrMessage("fclose() failed - could not close log file '%s'", logFilePath);
                exit(EXIT_FAILURE);
            }

            /* Rename it with '.old' extension */
            if (rename(logFilePath, oldLogFilePath) == -1)
            {
                logPrintErrMessage("rename() failed - %s", strerror(errno));
                exit(EXIT_FAILURE);
            }

            /* Open the log file in overwrite mode */
            logFile = fopen(logFilePath, "w");
            if (logFile == NULL)
            {
                /* Close the socket */
                close(sock);
            
                logPrintErrMessage("fopen() failed - could not re-open/re-create log file '%s'", logFilePath);
                exit(EXIT_FAILURE);
            }
            
            /* Try to change access mode to log file */
            fflush(logFile);
            chmod(logFilePath, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
        }
        
        /* Re-initialize temporary log message buffer */
        memset(&logMsg,'\0',sizeof(logMsg));
        
        /* Wait for received data, then put in temporary log message buffer */
        if (recvfrom(sock, (char *)logMsg, sizeof(logMsg), 0,
                    (struct sockaddr *)&from, &fromLen) == -1)
        {
            logPrintErrMessage("recvfrom() failed - %s", strerror(errno));
        }

        /* Store new message into the log file */
        if (fprintf(logFile,"%s\n", logMsg) < 0)
        {
            logPrintErrMessage("fprintf(\"%s\") failed", logMsg);
        }

        /* Empty temporary fprintf() system buffer in our log file */
        fflush(logFile);

        /* Inform that 10 new messages have been logged */
        loggedMsgNumber++;
        if ((loggedMsgNumber % 10) == 0)
        {
            logPrintErrMessage("logged %d messages", loggedMsgNumber);
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
