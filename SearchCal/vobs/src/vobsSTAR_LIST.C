/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsSTAR_LIST.C,v 1.5 2004-08-24 14:45:52 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    06-Jul-2004  Created
*
*******************************************************************************/
static char *rcsId="@(#) $Id: vobsSTAR_LIST.C,v 1.5 2004-08-24 14:45:52 scetre Exp $"; 
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
#include "err.h"

/*
 * Local Headers 
 */
#include"vobsSTAR_LIST.h"
#include"vobsPrivate.h"

/* Local variables */
static char *nameList[] =
{
   "hd",
   "hip",
   "ra",
   "dec",
   "pmdec",
   "pmra",
   "plx",
   "tsp",
   "varflag",
   "multflag",
   "glat",
   "glon",
   "radvel",
   "diam",
   "meth",
   "wlen",
   "photflux",
   "units",
   "U",
   "B",
   "V",
   "R",
   "I",
   "J",
   "H",
   "K",
   "L",
   "M",
   "N",
   "velocrotat",
   "color",
   NULL
};

//Class conctructor
vobsSTAR_LIST::vobsSTAR_LIST()
{
    _starIterator = _starList.begin();
}

//Copy constructor
mcsCOMPL_STAT vobsSTAR_LIST::Copy(vobsSTAR_LIST& list)
{
    for (unsigned int el = 0; el < list.Size(); el++)
    {
        AddAtTail(*(list.GetNextStar((el==0))));
    }
    return SUCCESS;
}

//Class destructor
vobsSTAR_LIST::~vobsSTAR_LIST()
{
    _starList.clear();
}

/**
 * Check whether the list is empty or not.  
 *
 * \return
 * True value (i.e. mcsTRUE) if the number of elements is zero, false (i.e.
 * mcsFLASE) otherwise.
 */
mcsLOGICAL vobsSTAR_LIST::IsEmpty(void)
{
    //logExtDbg("vobsSTAR_LIST::IsEmpty()");
    if (_starList.empty() == true)
    {
        return mcsTRUE;
    }
    else
    {
        return mcsFALSE;
    }
}

/**
 * Erase all elements from the list.
 *
 * \return
 * Always SUCCESS.
 */
mcsCOMPL_STAT vobsSTAR_LIST::Clear(void)
{
    _starList.clear();
    return SUCCESS;
}

/**
 * Adds the element at the end of the list
 *
 * \param star element to be added to the list.
 * \return
 * Always SUCCESS.
 */
mcsCOMPL_STAT vobsSTAR_LIST::AddAtTail(vobsSTAR &star)
{
    //logExtDbg("vobsSTAR_LIST::AddAtTail()");
    // Put element in the list
    _starList.push_back(star);

    return SUCCESS;
}

/**
 * Remove the element from the list
 *
 * This method looks for the specified \em star in the list. If found, it
 * remove it, and do nothing otherwise.
 *
 * The method vobsSTAR::IsSame() is used to compare element of the list with
 * the specified one.
 *
 * \warning if list contains more than one instance, only first occurence is
 * removed. 
 *
 * \param star element to be removed from the list.
 *
 * \return Always SUCCESS.
 */
mcsCOMPL_STAT vobsSTAR_LIST::Remove(vobsSTAR &star)
{
    logExtDbg("vobsSTAR_LIST::Remove()");

    // Search star in the list
    std::list<vobsSTAR>::iterator iter;
    for (iter=_starList.begin(); iter != _starList.end(); iter++)
    {
        if ((*iter).IsSame(star) == mcsTRUE)
        {
            //star.Display();
            //(*iter).Display();
            _starList.erase(iter);
            return SUCCESS;
        }
    }

    return SUCCESS;
}

/**
 * Returns the number of elements (stars) currently stored in the list.
 * \return 
 * The numbers of stars in the list.
 */
mcsUINT32 vobsSTAR_LIST::Size(void) 
{
    return _starList.size();
}

/**
 * Returns the next element (star) in the list.
 *
 * This method returns the pointer to the next element of the list. If \em
 * init is mcsTRUE, it returns the first element of the list.
 * 
 * This method can be used to move forward in the list, as shown below:
 * \code
 *     for (unsigned int el = 0; el < starList.Size(); el++)
 *     {
 *         starList.GetNextStar((el==0))->View();
 *     }
 * \endcode
 * \return pointer to the next element of the list or NULL if the end of the
 * list is reached.
 */
vobsSTAR *vobsSTAR_LIST::GetNextStar(mcsLOGICAL init) 
{
    //logExtDbg("vobsSTAR_LIST::GetNextStar()");

    if (init == mcsTRUE)
    {
        _starIterator = _starList.begin();
    }
    else
    {
        _starIterator++;
        if (_starIterator == _starList.end())
        {
            return NULL;
        }
    }
    return &(*_starIterator);
}

/**
 * Returns the element (star) of the list corresponding to the star.
 *
 * This method looks for the specified \em star in the list. If found, it
 * returns the pointer to this element, and NULL otherwise.
 * The method vobsSTAR::IsSame() is used to compare element of the list with
 * the specified one.
 * 
 * This method can be used to know whether a star is in list or not, as shown
 * below:
 * \code
 *     if (starList.GetStar(star)->View() == NULL)
 *     {
 *         printf ("Star not found in list !!");
 *     }
 * \endcode
 * \return pointer to the found element of the list or NULL if element is not
 * found in list.
 */
vobsSTAR *vobsSTAR_LIST::GetStar(vobsSTAR &star,
                                 float intervalRa, float intervalDec)
{
    //logExtDbg("vobsSTAR_LIST::GetStar()");

    // Search star in the list
    std::list<vobsSTAR>::iterator iter;
    for (iter=_starList.begin(); iter != _starList.end(); iter++)
    {
        if ((*iter).IsSameCoordinate(star, intervalRa, intervalDec) == mcsTRUE)
        {
            return &(*iter);
        }
    }

    // If not found return NULL pointer
    return NULL;
}

/**
 * Merge the specified list.
 *
 * This method merges all elements (stars) of the given list with the current
 * one. If star is already stored in the list, it is just updated using
 * vobsSTAR::Update method, otherwise it is added to the list.
 * 
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned if
 * updating or adding star fails.
 */
mcsCOMPL_STAT vobsSTAR_LIST::Merge(vobsSTAR_LIST &list,
                                   float intervalRa,
                                   float intervalDec,
                                   mcsLOGICAL updateOnly)
{
    logExtDbg("vobsSTAR_LIST::Merge()");

    // For each star of the given list
    unsigned int nbStars;
    nbStars = list.Size();
    for (unsigned int el = 0; el < nbStars; el++)
    {
        vobsSTAR *starPtr;
        starPtr = list.GetNextStar((el==0));
        // If star is in the list
        vobsSTAR *starToUpdatePtr;
        starToUpdatePtr = GetStar(*starPtr, intervalRa, intervalDec);
        if (starToUpdatePtr != NULL)
        {
            // Update the star
            if (starToUpdatePtr->Update(*starPtr)== FAILURE)
            {
                return FAILURE;
            }
        }
        else if (updateOnly == mcsFALSE)
        {
            // Else add it to the list
            if (AddAtTail(*starPtr) == FAILURE)
            {
                return FAILURE;
            }
        }
    }
 
    return SUCCESS;
}

/**
 * Method to print the list on the console
 */
/**
 * Display the elements (stars) of the list.
 *
 * This method display all elements of the list on the console, using the
 * vobsSTAR::Display method.
 */
void vobsSTAR_LIST::Display(void)
{
    logExtDbg("vobsSTAR_LIST::Display()");

    // Display all element of the list 
    std::list<vobsSTAR>::iterator iter;
    for (int i=0; i<vobsNB_STAR_PROPERTIES; i++)
    {
        printf("%12s",nameList[i]);
    }
    printf("\n");
    for (iter=_starList.begin(); iter != _starList.end(); iter++)
    {
        (*iter).Display();
    }
}
/*___oOo___*/
