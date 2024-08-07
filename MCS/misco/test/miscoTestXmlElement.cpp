/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * miscoXML_ELEMENT test program.
 *
 * @synopsis
 * @<miscoTestXmlElement@>
 */


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
    e2.AddAttribute("att2", 10.10);
    e2.AddAttribute("att3", mcsTRUE);
    root.AddAttribute("att2", "val2");

    // Add content to the root and e1 elements
    root.AddContent("test line");
    e1.AddContent("Two lines \n with carriage return");

    // Add typed elements to e2
    miscoXML_ELEMENT e20("double");
    miscoXML_ELEMENT e21("logical");
    miscoXML_ELEMENT e22("logical");
    e2.AddElement(&e20);
    e2.AddElement(&e21);
    e2.AddElement(&e22);
    e20.AddContent(10.1);
    e21.AddContent(mcsFALSE); 
    e22.AddContent(mcsTRUE); 
    
    // Print xml root desc
    cout << root.ToXml() << endl;

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
