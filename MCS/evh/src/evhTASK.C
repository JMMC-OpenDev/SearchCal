/*******************************************************************************
* JMMC project
*
* "@(#) $Id: evhTASK.C,v 1.2 2004-11-18 17:37:19 gzins Exp $"
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* gzins     09-Jun-2004  created
* gzins     18-Nov-2004  splitted parsing and usage methods to separate
*                        options and arguments in command-line parameters
*
*******************************************************************************/
static char *rcsId="@(#) $Id: evhTASK.C,v 1.2 2004-11-18 17:37:19 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/**
 * \file
 * class for handling of standard command-line options and of basic commands
 *
 * evhTASK is designed to handle several standard command-line options,
 * which should be common to all applications based on this class. 
 *
 * The options concern the verbosity level of the various logs, and
 * the help option that returns the usage syntax of the application:
 *     \arg \e -h                \n usage syntax
 *     \arg <em>-l \<level></em> \n file log level
 *     \arg <em>-v \<level></em> \n stdout log level
 *     \arg <em>-a \<level></em> \n action log level
 *     \arg <em>-t \<level></em> \n timer log level
 *     \arg <em>-version</em>    \n print the version number of the SW
 *     \arg <em>-noDate</em>     \n turn off the display of date in stdout log
 *                               messages
 *     \arg <em>-noFileLine</em> \n turn off the display of file name and line
 *                               number in stdout log messages
 *
 * Together with the parsing of these command-line options, the class sets
 * some flags accordingly, which allows the application to know which options
 * have been specified when it was called. This allows for example to override
 * some behavior specified elsewhere, e.g. in an application configuration
 * file.
 * 
 * This class aims to provide a general framework for the handling of standard
 * command-line options, and as such the applications developed by inheritance
 * should overload a certain number of methods, such as AppUsage() and
 * ParseAppOptions(), like explained below.
 *
 * \b Code \b Example:
 * \code
 *  // System Headers 
 *  #include <stdio.h>
 *  #include <iostream>
 *  using namespace std;
 *  
 *  // MCS Headers 
 *  #define MODULE_ID "mymod"
 *  #include "mcs.h"
 *  #include "log.h"
 *  #include "evh.h"
 *  
 *  class mymodSERVER : public evhTASK
 *  {
 *  public:
 *      mymodSERVER() {};
 *      virtual ~mymodSERVER() {};
 *      virtual mcsCOMPL_STAT AppUsage();
 *      virtual mcsCOMPL_STAT ParseAppOptions(mcsINT32 argc, mcsINT8 *argv[],
 *                                            mcsINT32 *optInd, 
 *                                            mcsLOGICAL *optUsed);
 *  private:
 *      mcsLOGICAL  noTimeout;
 *      mcsBYTES256 configFileName;
 *  };
 *  
 *  mcsCOMPL_STAT mymodSERVER::AppUsage()
 *  {
 *      logExtDbg("mymodSERVER::AppUsage()"); 
 *
 *      cout <<" Other options:    -noTimeout   disable waiting for a reply on "
 *          "a CCS message" << endl;
 *      cout <<"                   -c <file>    specify application "
 *          "configuration file" << endl;
 *      return SUCCESS;
 *  
 *  }
 *  
 *  mcsCOMPL_STAT mymodSERVER::ParseAppOptions(mcsINT32 argc, mcsINT8 *argv[],
 *                                          mcsINT32 *optInd, 
 *                                          mcsLOGICAL *optUsed)
 *  {
 *      logExtDbg("mymodSERVER::ParseAppOptions()"); 
 *
 *      // No timeout option
 *      if(strcmp(argv[*optInd], "-noTimeout") == 0)
 *      {
 *          noTimeout = mcsTRUE;
 *          return SUCCESS;
 *      }
 *      // Application configuration file option
 *      else if (strcmp(argv[*optInd], "-c") == 0)
 *      {
 *          if ((*optInd + 1) < argc)
 *          {
 *              *optInd += 1;
 *              optarg = argv[*optInd];
 *              if ( sscanf (optarg, "%s", configFileName) != 1)
 *              {
 *                  logError ("%s: Argument to option %s is invalid: '%s'",
 *                              Name(), argv[*optInd-1], optarg);
 *                  return FAILURE;
 *              }
 *              return SUCCESS;
 *          }
 *          else
 *          {
 *              logError ("%s: Option %s requires an argument",
 *                          Name(), argv[*optInd]);
 *              return FAILURE;
 *          }
 *      }
 *  
 *      // This option has not been processed. 
 *      *optUsed = mcsFALSE;
 *      return SUCCESS;
 *  }
 *  
 *  int main(int argc, char *argv[])
 *  {
 *      // Create objects living in the application
 *      mymodSERVER mymodServer;
 *      
 *      logInfo("Application starting ..");
 *      // Init application; register to MCS and parse input parameter
 *      if (mymodServer.Init(argc, argv) == FAILURE)
 *      {
 *          exit (EXIT_FAILURE);
 *      }
 *  
 *      // Specific application code
 *      ...
 *  
 *      logInfo("Application exiting ..");
 *      exit (EXIT_SUCCESS);
 *  }
 * \endcode
 */
/* 
 * System Headers 
 */
#include <stdio.h>
#include <iostream>
using namespace std;

/*
 * MCS Headers 
 */
#include "log.h"

/*
 * Local Headers 
 */
#include "evhPrivate.h"
#include "evhTASK.h"

// Class constructor 
evhTASK::evhTASK()
{
    _fileLogOption   = mcsFALSE;
    _stdoutLogOption = mcsFALSE;
    _actionLogOption = mcsFALSE;
    _timerLogOption  = mcsFALSE;
}

// Class destructor 
evhTASK::~evhTASK()
{
}
 
/**
 * Initialization of the application.
 *
 * This method just registers application to MCS services and parses the
 * command-line parameters.
 *
 * \return SUCCESS, or FAILURE if an error occurrs.
 */
mcsCOMPL_STAT evhTASK::Init(mcsINT32 argc, char *argv[])
{
    logExtDbg("evhTASK::Method()");

    // Initialize MCS services
    if (mcsInit(argv[0]) == FAILURE)
    {
        return (FAILURE);
    }

    // Parse input parameter
    if (ParseOptions(argc, argv) == FAILURE)
    {
        return (FAILURE);
    }

    // Perform specific application snitialization
    return (AppInit());
}

/**
 * Application initialization.
 *
 * This method should be overloaded by the application classes which inherit
 * from evhTASK, so as to perform initialization of the application. By
 * default, this method does nothing.
 * This method is called by Init() method.
 *
 * \return SUCCESS 
 */
mcsCOMPL_STAT evhTASK::AppInit()
{
    logExtDbg("evhSERVER::AppInit()");

    return SUCCESS;
}

 
/**
 * Returns the name of application.
 * \return name of application.
 */
const char *evhTASK::Name()
{
    logExtDbg("evhTASK::Name");

    return ((const char *)mcsGetProcName());
}
 
/**
 * Usage of the application.
 * It returns usage syntax for the application. It gives information about the
 * standard options listed above, and then internally calls AppUsage(), which
 * gives information about the options specific to the application.
 *
 * \return SUCCESS 
 */
mcsCOMPL_STAT evhTASK::Usage()
{
    logExtDbg("evhTASK::Usage()");

    PrintSynopsis();
    PrintStdOptions();
    PrintAppOptions();
    PrintArguments();
    return SUCCESS;
}

/**
 * Synopsys of the program.
 *
 * This method gives information about the synopsis of the program.
 *
 * \return SUCCESS 
 */
mcsCOMPL_STAT evhTASK::PrintSynopsis()
{
    std::cout << "Usage:" << Name() << " [OPTIONS]"<< endl;
    return SUCCESS;
}

/**
 * Usage of the standard options/arguments.
 *
 * This method gives information about the standard options listed above.
 *
 * \return SUCCESS 
 */
mcsCOMPL_STAT evhTASK::PrintStdOptions()
{
    cout <<" Standard options: -l <level>   set file log level" << endl;
    cout <<"                   -v <level>   set stdout log level" << endl;
    cout <<"                   -a <level>   set action log level" << endl;
    cout <<"                   -t <level>   set timer log level" << endl;
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
 * Usage of the specific options.
 * This method should be overloaded by the application classes which inherit
 * from evhTASK, so as to provide the user with information about the
 * command-line options which are specific to the application. By default,
 * this method does nothing.
 *
 * \return SUCCESS 
 */
mcsCOMPL_STAT evhTASK::PrintAppOptions()
{
    logExtDbg("evhTASK::PrintAppOptions()");

    return SUCCESS;
}

/**
 * Usage of the specific arguments.
 * This method should be overloaded by the application classes which inherit
 * from evhTASK, so as to provide the user with information about the
 * command-line options which are specific to the application. By default,
 * this method does nothing.
 *
 * \return SUCCESS 
 */
mcsCOMPL_STAT evhTASK::PrintArguments()
{
    logExtDbg("evhTASK::PrintArguments()");

    return SUCCESS;
}

/**
 * Parses the options of the application.
 * The ParseOptions() method parses the command line arguments. Its arguments
 * argc and argv are the argument count and array as passed to the * main()
 * function  on  program  invocation.
 * For each options, it first calls ParseStdOptions() method to parse the
 * standard options, and then ParseAppOptions() to deal with peculiar options
 * of the application.
 * \param argc count of the arguments supplied to the method
 * \param argv array of pointers to the strings which are those arguments
 * \return On success, SUCCESS is returned. On error, FAILURE is returned, and
 * error message is printed out accordingly.
 */
mcsCOMPL_STAT evhTASK::ParseOptions(mcsINT32 argc, mcsINT8 *argv[])
{
    logExtDbg ("evhTASK::ParseOptions ()");

    mcsINT32   optInd;
    mcsLOGICAL optUsed;

    // For each command option
    for (optInd =  1; optInd < argc; optInd++)
    {
        // Parses standard options
        optUsed = mcsTRUE;
        if (ParseStdOptions(argc, argv, &optInd, &optUsed) == FAILURE)
        {
            return FAILURE;
        }
        else if (optUsed == mcsFALSE)
        {
            optUsed = mcsTRUE;
            if (ParseAppOptions(argc, argv, &optInd, &optUsed) != SUCCESS)
            {
                return FAILURE;
            }
            else if (optUsed == mcsFALSE)
            {
                optUsed = mcsTRUE;
                if (ParseArguments(argc, argv, &optInd, &optUsed) != SUCCESS)
                {
                    return FAILURE;
                }
                else if (optUsed == mcsFALSE)
                {  
                    if (argv[optInd][0] == '-')
                    {
                        logError ("%s: Invalid option %s", 
                                  Name(), argv[optInd] );
                    }
                    else
                    {
                        logError ("%s: Invalid argument %s", 
                                  Name(), argv[optInd] );
                    }
                    return FAILURE;
                }
            }
        }
        // End if
    }
    // End for

    return SUCCESS;
}

/**
 * Parses the peculiar options of the application.
 * It parses the standard command-line options.
 * \param argc count of the arguments supplied to the method
 * \param argv array of pointers to the strings which are those arguments
 * \param optInd index of the arguments currently parsed 
 * \return On success, SUCCESS is returned. On error, FAILURE is returned, and
 * error message is printed out accordingly.
 */
mcsCOMPL_STAT evhTASK::ParseStdOptions(mcsINT32 argc, mcsINT8 *argv[],
                                       mcsINT32 *optInd, mcsLOGICAL *optUsed)
{
    logLEVEL  level;

    logExtDbg ("evhTASK::ParseStdOptions ()");
    // If help option specified
    if (strcmp(argv[*optInd], "-h") == 0)
    {
        // Print usage
        Usage();
        exit (EXIT_SUCCESS);
    }
    // Else if '-version' option specified
    else if (strcmp(argv[*optInd], "-version") == 0)
    {
        // Prints the version number of the SW
        printf ("%s\n", GetSwVersion());
        exit (EXIT_SUCCESS);
    }
    // Else if logging level specified
    else if (strcmp(argv[*optInd], "-l") == 0)
    { 
        // Set new logging level
        if ((*optInd + 1) < argc)
        {
            *optInd += 1;
            optarg = argv[*optInd];
            if ( sscanf (optarg, "%d", &level) != 1)
            {
                logError ("%s: Argument to option %s is invalid: '%s'",
                          Name(), argv[*optInd-1], optarg);
                return FAILURE;
            }
            logSetFileLogLevel(level);
            _fileLogOption = mcsTRUE;
        }
        else
        {
            logError ("%s: Option %s requires an argument",
                      Name(), argv[*optInd]);
            return FAILURE;
        }
    }
    // Else if stdout level specified
    else if (strcmp(argv[*optInd], "-v") == 0)
    {
        // Set new stdout log level
        if ((*optInd + 1) < argc)
        {
            *optInd += 1;
            optarg = argv[*optInd];
            if ( sscanf (optarg, "%d", &level) != 1)
            {
                logError ("%s: Argument to option %s is invalid: '%s'",
                          Name(), argv[*optInd-1], optarg);
                return FAILURE;
            }
            logSetStdoutLogLevel(level);
            _stdoutLogOption = mcsTRUE;
        }
        else
        {
            logError ("%s: Option %s requires an argument",
                      Name(), argv[*optInd]);
            return FAILURE;
        }
    }
    // Else if action level specified
    else if (strcmp(argv[*optInd], "-a") == 0)
    {
        // Set new action log level
        if ((*optInd + 1) < argc)
        {
            *optInd += 1;
            optarg = argv[*optInd];
            if ( sscanf (optarg, "%d", &level) != 1)
            {
                logError ("%s: Argument to option %s is invalid: '%s'",
                          Name(), argv[*optInd-1], optarg);
                return FAILURE;
            }
            logSetActionLogLevel(level);
            _actionLogOption = mcsTRUE;
        }
        else
        {
            logError ("%s: Option %s requires an argument",
                      Name(), argv[*optInd]);
            return FAILURE;
        }
    }
    // Else if timer level specified
    else if (strcmp(argv[*optInd], "-t") == 0)
    {
        // Set new timer log level
        if ((*optInd + 1) < argc)
        {
            *optInd += 1;
            optarg = argv[*optInd];
            if ( sscanf (optarg, "%d", &level) != 1)
            {
                logError ("%s: Argument to option %s is invalid: '%s'",
                          Name(), argv[*optInd-1], optarg);
                return FAILURE;
            }
            //ixacTIMER_LOGS::SetLevel(level);
            _timerLogOption = mcsTRUE;
        }
        else
        {
            logError ("%s: Option %s requires an argument",
                      Name(), argv[*optInd]);
            return FAILURE;
        }
    }
    // Else if '-noDate' option specified
    else if (strcmp(argv[*optInd], "-noDate") == 0)
    {
        // Turns off the display of date
        logSetPrintDate(mcsFALSE);
    }
    // Else if '-noFileLine' option specified
    else if (strcmp(argv[*optInd], "-noFileLine") == 0)
    {
        // Turns off the display of file/line
        logSetPrintFileLine(mcsFALSE);
    }
    // Else option has not been used
    else
    {
        
        *optUsed = mcsFALSE;
    }
    return SUCCESS;
}

/**
 * Parses the peculiar options of the application.
 * It parses the command-line options which are peculiar to the calling
 * application. By default, this method does nothing, and should be overloaded
 * by the classes which inherit from this one.
 * \param argc count of the arguments supplied to the method
 * \param argv array of pointers to the strings which are those arguments
 * \param optInd index of the arguments currently parsed 
 * \param optUsed flag informing whether the current option has been
 * processed or not.
 * \return On success, SUCCESS is returned. On error, FAILURE is returned, and
 * error message is printed out accordingly.
 */
mcsCOMPL_STAT evhTASK::ParseAppOptions(mcsINT32 argc, mcsINT8 *argv[],
                                       mcsINT32 *optInd, mcsLOGICAL *optUsed)
{
    logExtDbg ("evhTASK::ParseAppOptions ()");

    // Option has not been processed
    *optUsed = mcsFALSE;

    return SUCCESS;
}

/**
 * Parses the arguments of application
 * It parses the command-line arguments; i.e. which has not been handled
 * neither by ParseStdOptions() nor ParseAppOptions() method. By default,
 * this method does nothing, and should be overloaded by the classes which
 * inherit from this one.
 * \param argc count of the arguments supplied to the method
 * \param argv array of pointers to the strings which are those arguments
 * \param optInd index of the arguments currently parsed 
 * \param optUsed flag informing whether the current option has been
 * processed or not.
 * \return On success, SUCCESS is returned. On error, FAILURE is returned, and
 * error message is printed out accordingly.
 */
mcsCOMPL_STAT evhTASK::ParseArguments(mcsINT32 argc, mcsINT8 *argv[],
                                      mcsINT32 *optInd, mcsLOGICAL *optUsed)
{
    logExtDbg ("evhTASK::ParseArguments ()");

    // Option has not been processed
    *optUsed = mcsFALSE;

    return SUCCESS;
}

/**
 * Returns a flag indicating whether the log file level has been specified on
 * the command line
 * \return log file level option flag
 */
mcsLOGICAL evhTASK::IsFileLogOption()
{
    logExtDbg ("evhTASK::IsFileLogOption ()");

    return _fileLogOption;
}

/**
 * Returns a flag indicating whether the stdout log level has been specified
 * on the command line
 * \return stdout log level option flag
 */
mcsLOGICAL evhTASK::IsStdoutLogOption()
{
    logExtDbg ("evhTASK::IsStdoutLogOption ()");

    return _stdoutLogOption;
}

/**
 * Returns a flag indicating whether the action log level has been specified
 * on the command line
 * \return action log level option flag
 */
mcsLOGICAL evhTASK::IsActionLogOption()
{
    logExtDbg ("evhTASK::IsActionLogOption ()");

    return _actionLogOption;
}

/**
 * Returns a flag indicating whether the timer log level has been specified on
 * the command line
 * \return timer log level option flag
 */
mcsLOGICAL evhTASK::IsTimerLogOption()
{
    logExtDbg ("evhTASK::IsTimerLogOption ()");

    return _timerLogOption;
}

/**
 * Prints the version number of the software.
 * 
 */
const char *evhTASK::GetSwVersion()
{
    return "SW version number no set"; 
}

/*___oOo___*/
