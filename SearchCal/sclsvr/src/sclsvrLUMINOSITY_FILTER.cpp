/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrLUMINOSITY_FILTER.cpp,v 1.2 2005-06-07 12:36:27 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/06/01 14:18:54  scetre
 * Added filters and filter list objects.
 * Changed logExtDbg to logTrace
 *
 ******************************************************************************/

/**
 * @file
 *  Definition of sclsvrLUMINOSITY_FILTER class.
 */

static char *rcsId="@(#) $Id: sclsvrLUMINOSITY_FILTER.cpp,v 1.2 2005-06-07 12:36:27 scetre Exp $"; 
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
#include "sclsvrLUMINOSITY_FILTER.h"
#include "sclsvrPrivate.h"

/**
 * Class constructor
 */
sclsvrLUMINOSITY_FILTER::sclsvrLUMINOSITY_FILTER()
{
    strcpy(_name, sclsvrLUMINOSITY_FILTER_NAME);            
}

/**
 * Class destructor
 */
sclsvrLUMINOSITY_FILTER::~sclsvrLUMINOSITY_FILTER()
{
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
mcsCOMPL_STAT sclsvrLUMINOSITY_FILTER::
SetLuminosity(std::list<char *> lumClassList)
{
    logTrace("sclsvrLUMINOSITY_FILTER::SetLuminosity()");
   
    _luminosityClassList = lumClassList;

    return mcsSUCCESS;    
}

/**
 * Get value to the filter
 *
 * @param lumClassList the temperature list
 *
 * @return always mcsSUCCESS 
 */
std::list<char *> * sclsvrLUMINOSITY_FILTER::GetLuminosity()
{
    logTrace("sclsvrLUMINOSITY_FILTER::GetLuminosity()");

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
mcsCOMPL_STAT sclsvrLUMINOSITY_FILTER::Apply(sclsvrCALIBRATOR_LIST *list)
{
    logTrace("sclsvrLUMINOSITY_FILTER::Apply()");

    char *lumClassList[_luminosityClassList.size()];
    std::list<char *>::iterator luminosityClassListIterator;
    luminosityClassListIterator = _luminosityClassList.begin();
    int cmp = 0;
    while(luminosityClassListIterator != _luminosityClassList.end())
    {
        lumClassList[cmp] = 
            (char *)malloc(strlen(*luminosityClassListIterator)*sizeof(char));
        strcpy(lumClassList[cmp], *luminosityClassListIterator);
        cmp++;
        luminosityClassListIterator++;
    }
    lumClassList[_luminosityClassList.size()] = NULL;
    
    if (IsEnable() == mcsTRUE)
    {
        // For each calibrator in the list
        sclsvrCALIBRATOR *calibrator;
        for (unsigned int el = 0; el < list->Size(); el++)
        {
            calibrator = 
                (sclsvrCALIBRATOR *)list->GetNextStar((mcsLOGICAL)(el==0));

            // If spectral type is unknown
            if (calibrator->IsPropertySet(vobsSTAR_SPECT_TYPE_MK) == mcsFALSE)
            {
                // Remove it
                if (list->Remove(*calibrator) == mcsFAILURE)
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
                spType = calibrator-> GetPropertyValue(vobsSTAR_SPECT_TYPE_MK);
                
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
                    // Remove it
                    if (list->Remove(*calibrator) == mcsFAILURE)
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
        free(lumClassList[i]);
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
