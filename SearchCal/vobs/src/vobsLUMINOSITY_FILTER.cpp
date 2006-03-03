/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsLUMINOSITY_FILTER.cpp,v 1.6 2006-03-03 15:03:27 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.5  2005/12/14 15:07:53  scetre
 * Added log information about deleted star in filters
 *
 * Revision 1.4  2005/12/13 16:30:33  lafrasse
 * Added filter Id management through additional constructor parameter
 *
 * Revision 1.3  2005/11/29 13:49:19  gzins
 * Removed filter name setting
 *
 * Revision 1.2  2005/06/21 06:20:45  scetre
 * Changed method Disable() and Enable() to Disabled() and Enabled()
 * Changed '\' in doxygen documentatiuon to '@'
 *
 * Revision 1.1  2005/06/20 11:31:53  scetre
 * Added filter class
 *
 * Revision 1.3  2005/06/13 10:22:47  scetre
 * Updated documentation
 *
 * Revision 1.2  2005/06/07 12:36:27  scetre
 * Updated documentation
 *
 * Revision 1.1  2005/06/01 14:18:54  scetre
 * Added filters and filter list objects.
 * Changed logExtDbg to logTrace
 *
 ******************************************************************************/

/**
 * @file
 *  Definition of vobsLUMINOSITY_FILTER class.
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: vobsLUMINOSITY_FILTER.cpp,v 1.6 2006-03-03 15:03:27 scetre Exp $"; 

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
    
    if (IsEnabled() == mcsTRUE)
    {
        // For each STAR in the list
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
