/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsFILTER.cpp,v 1.6 2005-11-23 10:22:20 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.5  2005/11/16 10:47:55  scetre
 * Updated documentation
 *
 * Revision 1.4  2005/11/16 10:47:54  scetre
 * Updated documentation
 *
 * Revision 1.3  2005/11/05 15:52:36  gzins
 * Change Enabled and Disabled to Enable and Disable
 *
 * Revision 1.2  2005/06/21 06:20:45  scetre
 * Changed method Disable() and Enable() to Disabled() and Enabled()
 * Changed '\' in doxygen documentatiuon to '@'
 *
 * Revision 1.1  2005/06/20 11:31:53  scetre
 * Added filter class
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
 *  Definition of vobsFILTER class.
 */

static char *rcsId="@(#) $Id: vobsFILTER.cpp,v 1.6 2005-11-23 10:22:20 scetre Exp $"; 
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
#include "vobsFILTER.h"
#include "vobsPrivate.h"

/**
 * Class constructor
 */
vobsFILTER::vobsFILTER()
{
    // by default, the filter is disabled
    Disable();
}

/**
 * Class destructor
 */
vobsFILTER::~vobsFILTER()
{
}

/*
 * Public methods
 */
/**
 * Get the filter name
 *
 * @return filter name
 */
char * vobsFILTER::GetName(void)
{
    logTrace("vobsFILTER::GetName()");

    return _name;
}

/**
 * Say if the filter is enable or not
 *
 * @return mcsTRUE is filter is enable, otherwise mcsFALSE is returned
 */
mcsLOGICAL vobsFILTER::IsEnabled(void)
{
    logTrace("vobsFILTER::IsEnabled()");

    return _isEnable;
}

/**
 * Enable the filter
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsFILTER::Enable(void)
{
    logTrace("vobsFILTER::Enable()");
    
    _isEnable = mcsTRUE;
    
    return mcsSUCCESS;
}

/**
 * Disable the filter
 *
 * @return always mcsSUCCESS 
 */
mcsCOMPL_STAT vobsFILTER::Disable(void)
{
    logTrace("vobsFILTER::Disable()");

    _isEnable = mcsFALSE;
    
    return mcsSUCCESS;
}

/**
 * Set value on a property
 *
 * @param ucd the property ucd on which the filter is applied
 * @param value the value used to filter
 * 
 * @return always mcsSUCCESS 
 */
mcsCOMPL_STAT vobsFILTER::SetValueOnProperty(mcsSTRING32 ucd,
                                             mcsSTRING32 value)
{
    logTrace("vobsFILTER::SetValueOnProperty()");

    // copy ucd and value
    strcpy(_ucd, ucd);
    strcpy(_value, value);
    
    return mcsSUCCESS;
}

/**
 * Apply filter on a list
 *
 * @param list the list on which the filter is applied
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT vobsFILTER::Apply(vobsSTAR_LIST *list)
{
    logTrace("vobsFILTER::Apply()");

    if (IsEnabled() == mcsTRUE)
    {
        vobsSTAR *star;
        for (unsigned int el = 0; el < list->Size(); el++)
        {
            star=
                (vobsSTAR *)list->GetNextStar((mcsLOGICAL)(el==0));
            // if it is not possible to get the visibility, remove the star
            if ((star->IsPropertySet(_ucd) != mcsTRUE) ||
                (strcmp(star->GetPropertyValue(_ucd), _value) !=0))
            {
                // Remove it
                logTest("star %d had property %s = %s (!= %s)\n",
                        el+1, _ucd, star->GetPropertyValue(_ucd), _value);
                if (list->Remove(*star) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
                el = el-1;            
            }
        }
    }
    
    return mcsSUCCESS;
}


/*___oOo___*/
