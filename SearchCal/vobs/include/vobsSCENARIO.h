#ifndef vobsSCENARIO_H
#define vobsSCENARIO_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsSCENARIO.h,v 1.11 2005-12-22 14:14:17 lafrasse Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.10  2005/11/29 10:30:20  gzins
* Moved vobsFILTER to vobsGENERIC_FILTER
*
* Revision 1.9  2005/11/24 08:13:50  scetre
* Changed mother class of filter from vobsFILTER to vobsFILTER
*
* Revision 1.8  2005/11/15 14:57:56  scetre
* Added new scenario structure
* Added possibility to query merand and borde as primary catalog
*
* Revision 1.7  2005/02/09 06:09:57  gzins
* Changed vobsSTAR_COMP_CRITERIA_LIST& to vobsSTAR_COMP_CRITERIA_LIST* in vobsSCENARIO
*
* Revision 1.6  2005/01/27 15:55:54  scetre
* scenario became a friend class of scenario entry to be able to access private members
*
* Revision 1.5  2005/01/27 13:44:04  scetre
* remove vobsSCENARIO_ENTRY class
*
* Revision 1.4  2005/01/26 08:10:32  scetre
* change history
*
* scetre    07-Sep-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * Brief description of the header file, which ends at this dot.
 */


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * header files
 */
#include <list>

/*
 * local Header
 */
#include "vobsSTAR_LIST.h"
#include "vobsCATALOG.h"
#include "vobsSTAR_COMP_CRITERIA_LIST.h"
#include "vobsSCENARIO_ENTRY.h"
#include "vobsCATALOG_LIST.h"
#include "vobsREQUEST.h"

/*
 * Class declaration
 */

/**
 * vobsSCENARIO is a class which represent the catalog interrogation scenario 
 * of the catalogues.
 * 
 * vobsSCENARIO methods allow to
 * \li Add action into the scenario : an action is represented by the
 * catalog of destination and the specific action to apply
 * \li Execute all the action loaded
 * 
 */

class vobsSCENARIO
{
public :
    vobsSCENARIO();
    virtual ~vobsSCENARIO(); 
    
    virtual mcsCOMPL_STAT AddEntry(mcsSTRING32      catalog,
                                   vobsREQUEST      *request,
                                   vobsSTAR_LIST    *listInput,
                                   vobsSTAR_LIST    *listOutput,
                                   vobsACTION       action,
                                   vobsSTAR_COMP_CRITERIA_LIST *criteriaList=NULL,
                                   vobsFILTER       *filter=NULL);
  
    // Execute the scenario
    virtual mcsCOMPL_STAT Execute(vobsSTAR_LIST &starList);

    virtual mcsCOMPL_STAT SetCatalogList(vobsCATALOG_LIST * catalogList);

    virtual mcsCOMPL_STAT Clear(void);
    
protected :

private :
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsSCENARIO& operator=(const vobsSCENARIO&);
    vobsSCENARIO (const vobsSCENARIO&);

    // List of entries
    std::list<vobsSCENARIO_ENTRY>            _entryList;
    std::list<vobsSCENARIO_ENTRY>::iterator  _entryIterator;

    // pointer of list of catalog
    vobsCATALOG_LIST *_catalogList;
    mcsUINT32 _nbOfCatalogs;
    mcsUINT32 _catalogIndex;
};



#endif /*!vobsSCENARIO_H*/

/*___oOo___*/
