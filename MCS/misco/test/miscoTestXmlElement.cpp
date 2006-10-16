/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: miscoTestXmlElement.cpp,v 1.2 2006-10-16 10:29:21 mella Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2006/10/16 07:34:25  mella
 * Class miscoXML_ELEMENT created
 *
 *
 ******************************************************************************/

/**
 * @file
 * miscoXML_ELEMENT test program.
 *
 * @synopsis
 * @<miscoTestXmlElement@>
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: miscoTestXmlElement.cpp,v 1.2 2006-10-16 10:29:21 mella Exp $";

/* 
 * System Headers 
 */
#include <stdio.h>
#include <iostream>

/**
 * @namespace std
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
#include "misco.h"
#include "miscoPrivate.h"


/* 
 * Local functions  
 */


/* 
 * Main
 */

int main(int argc, char *argv[])
{
    // Initialize MCS services
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        // Exit from the application with mcsFAILURE
        exit (EXIT_FAILURE);
    }


    miscoXML_ELEMENT root("root");
    miscoXML_ELEMENT e1("e1");
    miscoXML_ELEMENT e2("e2");

    // Add Elements to the root element
    root.AddElement(& e1);
    root.AddElement(& e2);
    
    // Add attributes to the root and e2 elements
    root.AddAttribute("att1", "val1");
    e2.AddAttribute("att1", "val2");
    root.AddAttribute("att2", "val2");

    // Print xml root desc
    cout << root.ToString() << endl;
    


    cout << "---------------------------------------------------------" << endl
         << "                      THAT'S ALL FOLKS ;)                " << endl
         << "---------------------------------------------------------" << endl;



    // Close MCS services
    mcsExit();
    
    // Exit from the application with SUCCESS
    exit (EXIT_SUCCESS);
}

void displayExecStatus(mcsCOMPL_STAT executionStatusCode)
{
    if (executionStatusCode == mcsFAILURE)
    {
        cout << "FAILED";
        errCloseStack();
    }
    else
    {
        cout << "SUCCEED";
    }

    cout << endl;
    return;
}


/*___oOo___*/
