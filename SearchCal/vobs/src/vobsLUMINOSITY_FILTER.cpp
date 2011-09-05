/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Definition of vobsLUMINOSITY_FILTER class.
 */


/* 
 * System Headers 
 */
#include <iostream>
#include <stdlib.h>
#include <string.h>
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
#include "vobsLUMINOSITY_FILTER.h"
#include "vobsPrivate.h"

/**
 * Class constructor
 */
vobsLUMINOSITY_FILTER::vobsLUMINOSITY_FILTER(const char* filterId):vobsFILTER(filterId)
{
}

/**
 * Class destructor
 */
vobsLUMINOSITY_FILTER::~vobsLUMINOSITY_FILTER()
{
    // Free all strings containing luminosity classes
     std::list<char *>::iterator iter;
     for (iter = _luminosityClassList.begin(); 
          iter != _luminosityClassList.end(); iter++)
     {
         free(*iter);
     }
     _luminosityClassList.clear();
}

/*
 * Public methods
 */
/**
 * Set value to the filter
 *
 * @param lumClassList the temperature list
 *
 * @return always mcsSUCCESS 
 */
mcsCOMPL_STAT vobsLUMINOSITY_FILTER::
SetLuminosity(std::list<char *> lumClassList)
{
    logTrace("vobsLUMINOSITY_FILTER::SetLuminosity()");
   
    _luminosityClassList = lumClassList;

    return mcsSUCCESS;    
}

/**
 * Get value of the filter
 *
 * @return value of the filter
 */
std::list<char *> * vobsLUMINOSITY_FILTER::GetLuminosity()
{
    logTrace("vobsLUMINOSITY_FILTER::GetLuminosity()");

    return &_luminosityClassList;
}

/**
 * Apply luminosity filter on a list
 *
 * @param list the list on which the filter is applied
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned. 
 */
mcsCOMPL_STAT vobsLUMINOSITY_FILTER::Apply(vobsSTAR_LIST *list)
{
    logTrace("vobsLUMINOSITY_FILTER::Apply()");

    mcsCOMPL_STAT result = mcsSUCCESS;
    
    int lumClassLength = 0;
    char *lumClassList[_luminosityClassList.size()];
    
    std::list<char *>::iterator luminosityClassListIterator;
    luminosityClassListIterator = _luminosityClassList.begin();
    while(luminosityClassListIterator != _luminosityClassList.end())
    {
        lumClassList[lumClassLength] = (char *)malloc(strlen(*luminosityClassListIterator)*sizeof(char));
        strcpy(lumClassList[lumClassLength], *luminosityClassListIterator);
        lumClassLength++;
        luminosityClassListIterator++;
    }
    lumClassList[_luminosityClassList.size()] = NULL;
    
    if (IsEnabled() == mcsTRUE)
    {
        // For each STAR in the list
        vobsSTAR *star;
        for (unsigned int el = 0; el < list->Size(); el++)
        {
            star = (vobsSTAR *)list->GetNextStar((mcsLOGICAL)(el==0));

            mcsSTRING32 starId;
            // Get Star ID
            if (star->GetId(starId, sizeof(starId)) == mcsFAILURE)
            {
                result = mcsFAILURE;
                goto cleanup;
            }

            // If spectral type is unknown
            if (star->IsPropertySet(vobsSTAR_SPECT_TYPE_MK) == mcsFALSE)
            {
                logInfo("star %s has been removed by the filter '%s' : property %s is not set", starId, GetId(), vobsSTAR_SPECT_TYPE_MK);
                // Remove it
                if (list->Remove(*star) == mcsFAILURE)
                {
                    result = mcsFAILURE;
                    goto cleanup;
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

                while ((spType[spIdx] != '\0') && 
                       (spType[spIdx] != 'I') && (spType[spIdx] != 'V'))
                {
                    spIdx++;
                }
                
                mcsLOGICAL lumClassMatch = mcsFALSE; 
                // If the list of luminosity classes has been given
                if (lumClassList != NULL)
                {
                    // Look for luminosity class(es). Stop when end of string
                    // reached
                    mcsSTRING8 lumClass;
                    memset(lumClass, '\0', sizeof(mcsSTRING8)); 
                    mcsLOGICAL endOfString = mcsFALSE;
                    mcsINT32   lumIdx = 0;
                    while (endOfString == mcsFALSE)
                    {
                        // Get all consecutive characters constituing luminosity
                        // class
                        if ((spType[spIdx] == 'I') || (spType[spIdx] == 'V'))
                        {
                            lumClass[lumIdx] = spType[spIdx];
                            lumIdx++;
                        }
                        else
                        {
                            // If luminosity class has been found
                            if (lumIdx != 0)
                            {
                                // For each given temperature class
                                mcsINT32 classIdx = 0;
                                while (lumClassList[classIdx] != NULL)
                                {
                                    // Check if 
                                    if (strcmp(lumClass, 
                                               lumClassList[classIdx]) == 0)
                                    {
                                        lumClassMatch = mcsTRUE;
                                    }
                                    classIdx++;
                                }

                                memset(lumClass, '\0', sizeof(mcsSTRING8)); 
                                lumIdx = 0;
                            }
                        }
                        if (spType[spIdx] == '\0')
                        {
                            endOfString = mcsTRUE;
                        }
                        spIdx++;
                    }
                }
                // Else
                else
                {
                    // Do not filter
                    lumClassMatch = mcsTRUE;
                }
                // If the spectral of the list is not one of the list
                if (lumClassMatch == mcsFALSE)
                {            
                    logInfo("star %s has been removed by the filter '%s'", starId, GetId());
                    // Remove it
                    if (list->Remove(*star) == mcsFAILURE)
                    {
                        result = mcsFAILURE;
                        goto cleanup;
                    }
                    el = el-1;            
                }
            }
        }
    }

cleanup:
    // free memory
    for (int i = 0; i < lumClassLength; i++)
    {
        free(lumClassList[i]);
    }
    
    return result;
}
/*
 * Protected methods
 */


/*
 * Private methods
 */


/*___oOo___*/
