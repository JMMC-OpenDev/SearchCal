#ifndef vobsPARSER_H
#define vobsPARSER_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsPARSER.h,v 1.4 2004-09-30 07:40:09 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    06-Jul-2004  Created
*
*
*******************************************************************************/

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * system header files
 */
#include <vector>
#include <gdome.h>

/*
 * header files
 */
#include "vobsSTAR_LIST.h"

/*
 * Data structure containing description of the CDATA section
 * INTERNAL USED ONLY
 */
typedef struct
{
    std::vector<char*>colName; // Name of columns
    std::vector<char*>ucdName; // Name of corresponding UCD
    int nbLineToJump;          // Number of lines to be skipped in CDATA section
    char *ptr;                 // Pointer to the CDATA section
} vobsCDATA;


class vobsPARSER
{
public:
    vobsPARSER();
    virtual ~vobsPARSER();
    
    // Parse of the XML document from a URI
    mcsCOMPL_STAT Parse(char *uri, 
                        vobsSTAR_LIST &starList,
                        mcsSTRING256 fileName);
    
protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsPARSER& operator=(const vobsPARSER&);
    vobsPARSER (const vobsPARSER&);
    
    // Recurvise parsing of XML document 
    mcsCOMPL_STAT ParseXmlSubTree(GdomeNode *node,
                                  std::vector<vobsCDATA *> &listCDATA,
                                  vobsCDATA *cData);

    // Parsing of the CDATA section
    mcsCOMPL_STAT ParseCData(vobsCDATA *cData,
                             vobsSTAR_LIST &starList,
                             FILE *f);
};

#endif /*!vobsPARSER_H*/


/*___oOo___*/
