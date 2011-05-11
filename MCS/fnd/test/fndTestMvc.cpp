/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * brief description of the program, which ends at this dot.
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: fndTestMvc.cpp,v 1.2 2006-05-11 13:04:55 mella Exp $";

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
#include "fnd.h"
#include "fndPrivate.h"

typedef std::list<int> intList;

/*
 * Model class
 */
class myMODEL : public fndMVC_MODEL
{
public:
    virtual mcsCOMPL_STAT AddInteger(mcsINT32 entier)
    {
        _intList.push_back(entier);

        NotifyViews();

        return mcsSUCCESS;
    }

    virtual intList GetList()
    {
        return _intList;
    }
private:
    intList _intList;
};

/*
 * First view class which displays integer values as column
 */
class myVIEW1 : public fndMVC_VIEW
{  
public:
    myVIEW1(myMODEL *model)
    {
        _model = model;
    }

    virtual mcsCOMPL_STAT Update()
    {
        // View the list of integer as a colum
        intList intList = _model->GetList();
        cout << "VIEW AS COLUM : "<<endl;
        intList::iterator intListIterator;
        intListIterator = intList.begin();

        while (intListIterator!= intList.end())
        {
            cout << "          \t"<<(*intListIterator) << endl;
            intListIterator++;
        }
        cout << endl;

        return mcsSUCCESS;
    }
private:
    myMODEL *_model;
};

/*
 * Second view class which displays all integer values on the same line
 */
class myVIEW2 : public fndMVC_VIEW
{   
public:
    myVIEW2(myMODEL *model)
    {
        _model = model;
    }

    virtual mcsCOMPL_STAT Update()
    {
        // View the list of integer as a colum
        intList intList = _model->GetList();
        cout << "VIEW AS LINE : "<<endl;
        intList::iterator intListIterator;
        intListIterator = intList.begin();

        while (intListIterator!= intList.end())
        {
            cout << "          \t"<<(*intListIterator) ;
            intListIterator++;
        }
        cout << endl;
        return mcsSUCCESS;
    }
private:
    myMODEL *_model;
};


/* 
 * Main
 */

int main(int argc, char *argv[])
{
    // Initialize MCS services
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        // Error handling if necessary

        // Exit from the application with FAILURE
        exit (EXIT_FAILURE);
    }

    logSetStdoutLogLevel(logQUIET);

    // Create a list model
    myMODEL model;
    // Create a view of this model list
    myVIEW1 view1(&model);
    // Create another view of this model list
    myVIEW2 view2(&model);

    // Add the two view in the model list
    model.AddView(&view1);
    model.AddView(&view2);

    cout << "The model is a list of integer" << endl;

    cout << "nb of View attached to the list model : " << model.GetNbViews();
    cout << endl;

    // First changed of the model, added 1 in the list
    cout << "First change : added 1 in the model" << endl;
    model.AddInteger(1);
    // The views will be updated
    // Second changed of the model, added 2 in the list
    cout << "Second change : added 2 in the model" << endl;    
    model.AddInteger(2);
    // The views will be updated

    // Close MCS services
    mcsExit();

    // Exit from the application with SUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
