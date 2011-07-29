/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsSTAR_LIST class definition.
 */
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
#include "misc.h"

/*
 * Local Headers 
 */
#include "vobsSTAR_LIST.h"
#include "vobsCDATA.h"
#include "vobsVOTABLE.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"


/**
 * Class constructor
 */
vobsSTAR_LIST::vobsSTAR_LIST()
{
    // this list must by default free star pointers:
    _freeStarPtrs = true;
    
    _starIterator = _starList.end();
}

/**
 * Destructor
 */
vobsSTAR_LIST::~vobsSTAR_LIST()
{
    Clear();
}

/**
 * Enable the flag to indicate to free star pointers or not (shadow copy)
 */
void vobsSTAR_LIST::SetFreeStarPointers(const bool freeStarPtrs)
{
    // define the free star pointers flag:
    _freeStarPtrs = freeStarPtrs;
}

/*
 * Public methods
 */
/**
 * Copy from a list
 * i.e. Add all elements present in the given list at the end of this list
 *
 * @param list the list to copy
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsSTAR_LIST::Copy(vobsSTAR_LIST& list)
{
    logTrace("vobsSTAR_LIST::Copy(vobsSTAR_LIST& list)");

    const unsigned int nbStars = list.Size();
    for (unsigned int el = 0; el < nbStars; el++)
    {
        if (AddAtTail(*(list.GetNextStar((mcsLOGICAL)(el==0)))) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    return mcsSUCCESS;
}

/**
 * Copy only references from the given list
 * i.e. Add all pointers present in the given list at the end of this list
 * 
 * this list must free pointers (_freeStarPtrs = true)
 * the source list must NOT free pointers (list._freeStarPtrs = false)
 *
 * @param list the list to copy
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsSTAR_LIST::CopyRefs(vobsSTAR_LIST& list)
{
    logTest("vobsSTAR_LIST::CopyRefs(vobsSTAR_LIST& list)");

    const unsigned int nbStars = list.Size();
    for (unsigned int el = 0; el < nbStars; el++)
    {
        AddRefAtTail(list.GetNextStar((mcsLOGICAL)(el==0)));
    }
    // define _freeStarPtrs for both lists:
    _freeStarPtrs = true;
    list._freeStarPtrs = false;

    logTest("vobsSTAR_LIST::CopyRefs size: %d", Size());
    
    return mcsSUCCESS;
}

/**
 * Return whether the list is empty or not.  
 *
 * @return mcsTRUE if the number of elements is zero, mcsFALSE otherwise.
 */
mcsLOGICAL vobsSTAR_LIST::IsEmpty(void)
{
    if (_starList.empty() == false)
    {
        return mcsFALSE;
    }

    return mcsTRUE;
}

/**
 * Erase (i.e de-allocate) all elements from the list.
 *
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT vobsSTAR_LIST::Clear(void)
{
    logDebug("vobsSTAR_LIST::Clear: freeStarPtrs= %d", _freeStarPtrs);
    
    if (_freeStarPtrs)
    {
        // Deallocate all objects of the list 
        std::list<vobsSTAR *>::iterator iter;
        for (iter=_starList.begin(); iter != _starList.end(); iter++)
        {
            delete (*iter);
        }
    }

    // Clear list
    _starList.clear();

    // this list must now (default) free star pointers:
    _freeStarPtrs = true;

    return mcsSUCCESS;
}

/**
 * Add the given element at the end of the list.
 *
 * @param star element to be added to the list.
 *
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT vobsSTAR_LIST::AddAtTail(vobsSTAR &star)
{
    // Put the element in the list
    vobsSTAR *newStar = new vobsSTAR(star);
    _starList.push_back(newStar);

    return mcsSUCCESS;
}

/**
 * Add the given element AS one Reference (= pointer only) at the end of the list
 * i.e. it does not copy the given star
 *
 * @param star element to be added to the list.
 *
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT vobsSTAR_LIST::AddRefAtTail(vobsSTAR* star)
{
    if (star != NULL)
    {
        // Put the reference in the list
        _starList.push_back(star);
    }

    return mcsSUCCESS;
}

/**
 * Remove the given element from the list
 *
 * This method looks for the specified @em star in the list. If found, it
 * remove it. Otherwise, nothing is done.
 *
 * The method vobsSTAR::IsSame() is used to compare the list elements with
 * the specified one.
 *
 * @warning if the list contains more than one instance of the given element,
 * only first occurence isremoved.
 * 
 * @note This method does not conflict with GetNextStar(); i.e. it can be used
 * to remove the star returned by GetNextStar() method, as shown below:
 * @code
 * for (unsigned int el = 0; el < starList.Size(); el++)
 * {
 *     vobsSTAR *star;
 *     star = starList.GetNextStar((mcsLOGICAL)(el==0));
 *     if ( <condition> )
 *     {
 *          // Remove star from list 
 *          starList.Remove(*star);
 *
 *          // and decrease 'el' to take into account the new list size
 *          el--;
 *     }
 * }
 * @endcode

 * @param star to be removed from the list.
 *
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT vobsSTAR_LIST::Remove(vobsSTAR &star)
{
    logTrace("vobsSTAR_LIST::Remove()");

    // Search star in the list
    std::list<vobsSTAR *>::iterator iter;
    for (iter=_starList.begin(); iter != _starList.end(); iter++)
    {
        // If found
        if ((*iter)->IsSame(star) == mcsTRUE)
        {
            if (_freeStarPtrs)
            {
                // Delete star
                delete (*iter);
            }

            // If star to be deleted correspond to the one currently pointed
            // by GetNextStar method
            if (*_starIterator == *iter)
            {
                // If it is not the first star of the list
                if (iter != _starList.begin())
                {
                    // Then go back to the previous star
                    _starIterator--;
                }
                else
                {
                    // Else set current pointer to end() in order to restart scan
                    // from beginning of the list.
                    _starIterator = _starList.end();
                }
            }
            
            // Clear star from list
            _starList.erase(iter);

            return mcsSUCCESS;
        }
    }

    return mcsSUCCESS;
}

/**
 * Returns the number of stars currently stored in the list.
 *
 * @return The numbers of stars in the list.
 */
mcsUINT32 vobsSTAR_LIST::Size(void) 
{
    return _starList.size();
}

/**
 * Return the next star in the list.
 *
 * This method returns the pointer to the next star of the list. If @em
 * init is mcsTRUE, it returns the first star of the list.
 * 
 * This method can be used to move forward in the list, as shown below:
 * @code
 * for (unsigned int el = 0; el < starList.Size(); el++)
 * {
 *     starList.GetNextStar((mcsLOGICAL)(el==0))->View();
 * }
 * @endcode
 *
 * @return pointer to the next element of the list or NULL if the end of the
 * list is reached.
 */
vobsSTAR *vobsSTAR_LIST::GetNextStar(mcsLOGICAL init) 
{
    if ((init == mcsTRUE) || _starIterator == _starList.end())
    {
        _starIterator = _starList.begin();
    }
    else
    {
        _starIterator++;
    }

    if (_starIterator == _starList.end())
    {
        return NULL;
    }

    return (*_starIterator);
}

/**
 * Return the star of the list corresponding to the given star.
 *
 * This method looks for the specified @em star in the list. If found, it
 * returns the pointer to this element. Otherwise, NULL is returned.
 *
 * The method vobsSTAR::IsSame() is used to compare element of the list with
 * the specified one.
 * 
 * This method can be used to discover whether a star is in list or not, as
 * shown below:
 * @code
 * if (starList.GetStar(star)->View() == NULL)
 * {
 *     printf ("Star not found in list !!");
 * }
 * @endcode
 *
 * @return pointer to the found element of the list or NULL if element is not
 * found in list.
 */
vobsSTAR *vobsSTAR_LIST::GetStar(vobsSTAR &star,
                                 vobsSTAR_COMP_CRITERIA_LIST *criteriaList)
{
    // Search star in the list
    std::list<vobsSTAR *>::iterator iter;
    for (iter=_starList.begin(); iter != _starList.end(); iter++)
    {
        if ((*iter)->IsSame(star, criteriaList) == mcsTRUE)
        {
            return (*iter);
        }
    }

    // If nothing found, return NULL pointer
    return NULL;
}

/**
 * Merge the specified list.
 *
 * This method merges all stars of the given list with the current one. If a
 * star is already stored in the list, it is just updated using
 * vobsSTAR::Update method, otherwise it is added to the list.
 * 
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned if updating or adding star failed.
 */
mcsCOMPL_STAT vobsSTAR_LIST::Merge(vobsSTAR_LIST &list,
                                   vobsSTAR_COMP_CRITERIA_LIST *criteriaList,
                                   mcsLOGICAL updateOnly)
{
    logTrace("vobsSTAR_LIST::Merge() - list size= %d", Size());

    vobsSTAR *starPtr;
    vobsSTAR *starToUpdatePtr;
    
    // For each star of the given list
    const unsigned int nbStars = list.Size();
    const unsigned int step = nbStars / 10;
    const bool logProgress = nbStars > 1000;
    
    for (unsigned int el = 0; el < nbStars; el++)
    {
        if (logProgress && el % step == 0)
        {
            logTest("vobsSTAR_LIST::Merge() - merged stars = %d", el);
        }
        
        starPtr = list.GetNextStar((mcsLOGICAL)(el==0));
        
        // If star is in the list
        starToUpdatePtr = GetStar(*starPtr, criteriaList);
        
        if (starToUpdatePtr != NULL)
        {
            // Update the star
            if (starToUpdatePtr->Update(*starPtr)== mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
        else if (updateOnly == mcsFALSE)
        {
            // Else add it to the list
            if (AddAtTail(*starPtr) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
    }
 
    return mcsSUCCESS;
}

/**
 * Sort the list.
 *
 * This method sorts the given list according to the given property Id. 
 * 
 * @param propertyId property id 
 * @param reverseOrder indicates sorting order 
 *
 * @return mcsSUCCESS on successful completion, and mcsFAILURE otherwise. 
 */
mcsCOMPL_STAT vobsSTAR_LIST::Sort(const char *propertyId, mcsLOGICAL reverseOrder)
{
    logInfo("vobsSTAR_LIST::Sort()");
 
    // If list is empty or contains only one element, return
    if ((IsEmpty() == mcsTRUE) || (Size() == 1))
    {
        return mcsSUCCESS;
    }

    // Check that the given property Id exists
    if (GetNextStar(mcsTRUE)->IsProperty(propertyId) == mcsFALSE)
    {
        errAdd(vobsERR_INVALID_PROPERTY_ID, propertyId); 
        return mcsFAILURE;
    }

    // Get property type 
    vobsPROPERTY_TYPE propertyType;
    propertyType = GetNextStar(mcsTRUE)->GetProperty(propertyId)->GetType();
    
    // While the sorting is not achieved
    mcsLOGICAL sortingDone;
    do 
    {
        // Assumes the list is sorted
        sortingDone = mcsTRUE;

        // For all elements of the list
        std::list<vobsSTAR *>::iterator iter;
        std::list<vobsSTAR *>::iterator prevIter=_starList.begin();
        for (iter=_starList.begin(); iter != _starList.end(); iter++)
        {
            // If iterator is not on the first element
            if (iter != _starList.begin()) 
            {
                // Indicates if current elements are to swapped or not
                mcsLOGICAL swap = mcsFALSE;

                // Check properties are set
                mcsLOGICAL isValue1Set;
                mcsLOGICAL isValue2Set;
                isValue1Set = (*prevIter)->IsPropertySet(propertyId);
                isValue2Set = (*iter)->IsPropertySet(propertyId);

                // If one of the properties is not set, move it at the begining
                // or at the end, according to the sorting order
                if ((isValue1Set == mcsFALSE) || (isValue2Set == mcsFALSE))
                {
                    // If it is normal sorting order
                    if (reverseOrder == mcsFALSE) 
                    {
                        // If value of previous element is not set while next
                        // one is, swap them
                        if ((isValue1Set == mcsFALSE) && 
                            (isValue2Set == mcsTRUE))
                        {
                            swap = mcsTRUE;
                        }
                    }
                    // Else (reverse sorting order)
                    else
                    {
                        // If value of next element is not set while previous
                        // one is, swap them
                        if ((isValue2Set == mcsFALSE) && 
                            (isValue1Set == mcsTRUE))
                        {
                            swap = mcsTRUE;
                        }
                    }
                }
                // Else (properties are set)
                else
                {
                    // Compare element values according to property  or property
                    // type, and check if elements have to be swapped according
                    // to the sorting order
                    if ((strcmp (propertyId, vobsSTAR_POS_EQ_RA_MAIN)  == 0) ||
                        (strcmp (propertyId, vobsSTAR_POS_EQ_DEC_MAIN) == 0) ||
                        (propertyType == vobsFLOAT_PROPERTY))
                    {
                        mcsDOUBLE value1;
                        mcsDOUBLE value2;
                        if (strcmp (propertyId, vobsSTAR_POS_EQ_RA_MAIN)  == 0)
                        {
                            (*prevIter)->GetRa(value1);
                            (*iter)->GetRa(value2);
                        }
                        else if (strcmp (propertyId, 
                                         vobsSTAR_POS_EQ_DEC_MAIN) == 0)
                        {
                            (*prevIter)->GetDec(value1);
                            (*iter)->GetDec(value2);
                        }
                        else
                        {
                            (*prevIter)->GetPropertyValue(propertyId, &value1);
                            (*iter)->GetPropertyValue(propertyId, &value2);
                        }

                        if (reverseOrder == mcsFALSE) 
                        {
                            if (value1 > value2)
                            {
                                swap = mcsTRUE;
                            }
                        }
                        else
                        {
                            if (value1 < value2)
                            {
                                swap = mcsTRUE;
                            }
                        }
                    }
                    else
                    {
                        const char *value1;
                        const char *value2;
                        value1 = (*prevIter)->GetPropertyValue(propertyId);
                        value2 = (*iter)->GetPropertyValue(propertyId);

                        if (reverseOrder == mcsFALSE) 
                        {
                            if (strcmp (value1, value2) > 0)
                            {
                                swap = mcsTRUE;
                            }
                        }
                        else
                        {
                            if (strcmp (value1, value2) < 0)
                            {
                                swap = mcsTRUE;
                            }
                        }
                    }
                }

                // Swap elements if needed 
                if (swap == mcsTRUE)
                {
                    iter++;
                    iter = _starList.insert(iter, (*prevIter));
                    _starList.erase(prevIter);
                    sortingDone = mcsFALSE;
                }
                
                // Update previous element iterator 
                prevIter = iter;
            }
            else
            {
                prevIter = iter;
            }
        }
    } while (sortingDone == mcsFALSE);
    
    logInfo("vobsSTAR_LIST::Sort() - done.");
    
    return mcsSUCCESS;
}

/**
 * Display the list contnent on the console.
 */
void vobsSTAR_LIST::Display(void)
{
    logTrace("vobsSTAR_LIST::Display()");

    // Display all element of the list 
    std::list<vobsSTAR *>::iterator iter;
    for (iter=_starList.begin(); iter != _starList.end(); iter++)
    {
        (*iter)->Display();
    }

    printf("\n");
}

/**
 * Save each star in a VOTable v1.1.
 *
 * @param header header of the VO Table
 * @param softwareVersion software version
 * @param request user request
 * @param xmlRequest user request as XML
 * @param buffer the buffer in which the VOTable should be written
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise. 
 */
mcsCOMPL_STAT vobsSTAR_LIST::GetVOTable(const char*    header,
                                        const char*    softwareVersion,
                                        const char*    request,
                                        const char*    xmlRequest,
                                        miscoDYN_BUF*  buffer)
{
    logTrace("vobsSTAR_LIST::GetVOTable()");

    vobsVOTABLE serializer;
    return(serializer.GetVotable(*this, NULL, header, softwareVersion, request, xmlRequest, buffer));
}

/**
 * Save each star in a VOTable v1.1.
 *
 * @param filename the path to the file in which the VOTable should be saved
 * @param header header of the VO Table
 * @param softwareVersion software version
 * @param request user request
 * @param xmlRequest user request as XML
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise. 
 */
mcsCOMPL_STAT vobsSTAR_LIST::SaveToVOTable(const char *filename,
                                           const char *header,
                                           const char *softwareVersion,
                                           const char *request,
                                           const char *xmlRequest)
{
    logTrace("vobsSTAR_LIST::SaveToVOTable()");

    vobsVOTABLE serializer;
    return(serializer.Save(*this, filename, header, softwareVersion, request, xmlRequest));
}

/**
 * Save the stars of the list in a file.
 *
 * @param filename the file where to save
 * @param extendedFormat if true, each property is saved with its attributes
 * (origin and confidence index), otherwise only only property is saved.
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsSTAR_LIST::Save(const char *filename,
                                  mcsLOGICAL extendedFormat)
{
    logTrace("vobsSTAR_LIST::Save()");

    vobsSTAR_PROPERTY_ID_LIST ucdList;
    
    return Save(filename, ucdList, extendedFormat);
}


/**
 * Save the stars of the list in a file.
 *
 * @param filename the file where to save
 * @param ucdList list of ucd to save
 * @param extendedFormat if true, each property is saved with its attributes
 * (origin and confidence index), otherwise only only property is saved.
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsSTAR_LIST::Save(const char *filename,
                                  vobsSTAR_PROPERTY_ID_LIST ucdList,
                                  mcsLOGICAL extendedFormat)
{
    logTrace("vobsSTAR_LIST::Save()");

    // Store list into the begining
    vobsCDATA cData;
    vobsSTAR  star;
    if (cData.Store(star, *this, ucdList, extendedFormat) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // Save into file
    if (cData.SaveInFile(filename) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/**
 * Load stars from a file.
 *
 * @param filename name of file containing star list
 * @param extendedFormat if true, each property is has been saved with its
 * attributes (origin and confidence index), otherwise only only property has
 * been saved.
 * @param origin used if origin is not given in file (see above). If NULL, the
 * name of file is used as origin.
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsSTAR_LIST::Load(const char *filename,
                                  mcsLOGICAL extendedFormat,
                                  const char *origin)
{
    logTrace("vobsSTAR_LIST::Load()");

    // Load file
    vobsCDATA cData;
    if (cData.LoadFile(filename) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Set origin (if needed)
    if (extendedFormat == mcsFALSE)
    {
        // if origin is unknown, set catalog name as the file in which the data
        // had been got
        if (origin == NULL)
        {
            if (cData.SetCatalogName(filename) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
        // if origin is kniwn, set catalog name as the catalog in which the data
        // had been got
        else
        {
            if (cData.SetCatalogName(origin) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
    }
        
    // Extract list from the CDATA
    vobsSTAR star;
    if (cData.Extract(star, *this, extendedFormat) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    return mcsSUCCESS;
}
/*___oOo___*/
