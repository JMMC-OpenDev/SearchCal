/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCALIBRATOR_STAR_LIST.C,v 1.3 2004-07-20 13:13:15 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    06-Jul-2004  Created
*
*
*******************************************************************************/

static char *rcsId="@(#) $Id: vobsCALIBRATOR_STAR_LIST.C,v 1.3 2004-07-20 13:13:15 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers 
 */
#include <iostream>
using namespace std;


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#define MODULE_ID "vobs"
#include "err.h"

/*
 * Local Headers 
 */

#include "vobs.h"
//#include"vobsCALIBRATOR_STAR_LIST.h"
//#include"vobsCALIBRATOR_STAR.h"



/*
 * Local Variables
 */


/*
 * Local Functions
 */
//Class conctructor
vobsCALIBRATOR_STAR_LIST::vobsCALIBRATOR_STAR_LIST()
{
}

//Class destructor
vobsCALIBRATOR_STAR_LIST::~vobsCALIBRATOR_STAR_LIST()
{
}

/**
 * Method to put an elemnet in a list
 */
void vobsCALIBRATOR_STAR_LIST::Put(vobsCALIBRATOR_STAR calibStar)
{
    // put an element in the list
    logExtDbg("vobsCALIBRATOR_STAR_LIST::Put()\n");

}

/**
 * Method to remove an element from a list
 */
void vobsCALIBRATOR_STAR_LIST::Remove(vobsCALIBRATOR_STAR calibStar)
{
    // remove an element from a list
    logExtDbg("vobsCALIBRATOR_STAR_LIST::Remove()\n");

}

/**
 * Method to sort a list
 */
void vobsCALIBRATOR_STAR_LIST::Sort()
{
    // Sort a list
    logExtDbg("vobsCALIBRATOR_STAR_LIST::Sort()\n");
}

/**
 * Method to fill a list
 */
void vobsCALIBRATOR_STAR_LIST::Fill()
{
    // Fill a list
    logExtDbg("vobsCALIBRATOR_STAR_LIST::Fill()\n");
}

/**
 * Method to filter the list
 */
void vobsCALIBRATOR_STAR_LIST::Filtre()
{
    // filter the list
    logExtDbg("vobsCALIBRATOR_STAR_LIST::Filtre()\n");
}

/**
 * Method to save a list
 */
void vobsCALIBRATOR_STAR_LIST::Save()
{
    // save the list
    logExtDbg("vobsCALIBRATOR_STAR_LIST::Save()\n");

}
/**
 * Method to set the list of star
 * \parameter STL list of vobsCALIBRATOR_STAR
 */
void vobsCALIBRATOR_STAR_LIST::Set(std::list<vobsCALIBRATOR_STAR>list)
{
    logExtDbg("vobsCALIBRATOR_STAR_LIST::Set()\n");    
    listOfStar=list;
}

/**
 * Method to print the list on the console
 */
void vobsCALIBRATOR_STAR_LIST::Print()
{
    logExtDbg("vobsCALIBRATOR_STAR_LIST::Print()\n");    
    std::list<vobsCALIBRATOR_STAR>::iterator iterateur=listOfStar.begin();
    while (iterateur!=listOfStar.end())
    {
        (*iterateur).View();
        ++iterateur;
    }
 
}

/**
 * Function which gave the number of star there are in the list
 * \return int size of the list
 */
int vobsCALIBRATOR_STAR_LIST::Size()
{
    return listOfStar.size();
}

/*___oOo___*/
