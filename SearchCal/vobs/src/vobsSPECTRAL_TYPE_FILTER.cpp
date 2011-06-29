/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Definition of vobsSPECTRAL_TYPE_FILTER class.
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

/*
 * Local Headers 
 */
#include "vobsSPECTRAL_TYPE_FILTER.h"
#include "vobsPrivate.h"

/**
 * Class constructor
 */
vobsSPECTRAL_TYPE_FILTER::vobsSPECTRAL_TYPE_FILTER(const char* filterId):vobsFILTER(filterId)
{
}

/**
 * Class destructor
 */
vobsSPECTRAL_TYPE_FILTER::~vobsSPECTRAL_TYPE_FILTER()
{
}

/*
 * Public methods
 */
/**
 * Set value to the filter
 *
 * @param tempClassList the temperature list
 *
 * @return always mcsSUCCESS 
 */
mcsCOMPL_STAT 
vobsSPECTRAL_TYPE_FILTER::SetSpectralType(std::list<char *> tempClassList)
{
    logTrace("vobsSPECTRAL_TYPE_FILTER::SetSpectralType()");

    _tempClassList = tempClassList;
    
    return mcsSUCCESS;    
}

/**
 * Get value of the filter
 *
 * @return value of the filter
 */
std::list<char *> * vobsSPECTRAL_TYPE_FILTER::GetSpectralType(void)
{
    logTrace("vobsSPECTRAL_TYPE_FILTER::GetSpectralType()");

    return &_tempClassList;
}

/**
 * Apply spectral filter on a list
 *
 * @param list the list on which the filter is applied
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned. 
 */
mcsCOMPL_STAT vobsSPECTRAL_TYPE_FILTER::Apply(vobsSTAR_LIST *list)
{
    logTrace("vobsSPECTRAL_TYPE_FILTER::Apply()");

    char * tempClassList[_tempClassList.size()];
    std::list<char *>::iterator tempClassListIterator;
    tempClassListIterator = _tempClassList.begin();
    int cmp = 0;
    while (tempClassListIterator != _tempClassList.end())
    {
        tempClassList[cmp] =
            (char *) malloc(strlen(*tempClassListIterator)*sizeof(char));
        strcpy(tempClassList[cmp], *tempClassListIterator);
        cmp++;
        tempClassListIterator++;
    }
    tempClassList[_tempClassList.size()] = NULL;
    
    if (IsEnabled() == mcsTRUE)
    {
        // For each star in the list
        vobsSTAR *star;
        for (unsigned int el = 0; el < list->Size(); el++)
        {
            star = 
                (vobsSTAR *)list->GetNextStar((mcsLOGICAL)(el==0));
            mcsSTRING32 starId;
            // Get Star ID
            if (star->GetId(starId, sizeof(starId)) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            // If spectral type is unknown
            if (star->IsPropertySet(vobsSTAR_SPECT_TYPE_MK) == mcsFALSE)
            {
                logInfo("star %s has been removed by the filter '%s' : property %s is not set", starId, GetId(), vobsSTAR_SPECT_TYPE_MK);                
                // Remove it
                if (list->Remove(*star) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
                el = el - 1;
            }
            // Else
            else
            {
                // Get the star spectral type 
                const char *spType;
                spType = star-> GetPropertyValue(vobsSTAR_SPECT_TYPE_MK);
                // Look for temperature class(es). Stop when end of string 
                // reached or first character of luminosity class found 
                mcsINT32 spIdx = 0; // Index on spectral type string
                mcsLOGICAL tempClassMatch = mcsFALSE;
                while ((spType[spIdx] != '\0') && 
                       (spType[spIdx] != 'I') && (spType[spIdx] != 'V'))
                {
                    // If the list of temperature classes has been given
                    if (tempClassList != NULL)
                    {
                        // For ecah given temperature class
                        mcsINT32 classIdx = 0;
                        while (tempClassList[classIdx] != NULL)
                        {
                            // Check if 
                            if (spType[spIdx] == tempClassList[classIdx][0])
                            {
                                tempClassMatch = mcsTRUE;
                            }
                            classIdx++;
                        }
                    }
                    // Else
                    else
                    {
                        // Do not filter
                        tempClassMatch = mcsTRUE;
                    }
                    spIdx++;
                }
                // If the spectral of the list is not one of the list
                if (tempClassMatch == mcsFALSE)
                {
                    logInfo("star %s has been removed by the filter '%s'", starId, GetId());
                    // Remove it
                    if (list->Remove(*star) == mcsFAILURE)
                    {
                        return mcsFAILURE;
                    }
                    el = el-1;            
                }
            }
        }
    }

     // free memory
    for (int i=0; i<cmp; i++)
    {
        free(tempClassList[i]);
    }

    return mcsSUCCESS;
}
/*
 * Protected methods
 */


/*
 * Private methods
 */


/*___oOo___*/
