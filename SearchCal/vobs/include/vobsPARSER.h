#ifndef vobsPARSER_H
#define vobsPARSER_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Declaration of vobsPARSER class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * system header files
 */
#include <vector>
#include <libgdome/gdome.h>

/** Time out (in seconds) to get the CDS XML file */
#define vobsTIME_OUT 600

/*
 * header files
 */
#include "vobsSTAR_LIST.h"
#include "vobsSCENARIO_RUNTIME.h"
#include "vobsCDATA.h"

/*
 * Class declaration
 */

/**
 * vobsPARSER allow to get a xml file from an URL in the CDS and to parse it
 * in order to extract the data present in it.
 */
class vobsPARSER
{
public:
    vobsPARSER();
    virtual ~vobsPARSER();

    // Parse of the XML document from a URI
    mcsCOMPL_STAT Parse(vobsSCENARIO_RUNTIME &ctx,
                        const char *uri,
                        const char *data,
                        vobsORIGIN_INDEX catalogId,
                        const vobsCATALOG_META* catalogMeta,
                        vobsSTAR_LIST &starList,
                        vobsCATALOG_STAR_PROPERTY_CATALOG_MAPPING* propertyCatalogMap,
                        const char *logFileName = NULL);

protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsPARSER& operator=(const vobsPARSER&) ;
    vobsPARSER(const vobsPARSER&);

    // Recurvise parsing of XML document
    mcsCOMPL_STAT ParseXmlSubTree(GdomeNode* node, vobsCDATA* cData, miscoDYN_BUF* dataBuf);
} ;

#endif /*!vobsPARSER_H*/


/*___oOo___*/
