#ifndef vobsPARSER_H
#define vobsPARSER_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsPARSER.h,v 1.1 2004-07-13 13:41:09 scetre Exp $"
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
 * header files
 */
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "gdome.h"
#include <vector>
#include <list>
#include <fstream>
#include <string.h>
#include "vobsCALIBRATOR_STAR_LIST.h"

/*
 * constants
 */
/*none*/

class vobsPARSER
{
public:
    vobsPARSER();
    virtual ~vobsPARSER();
    
    int MainParser(char * request);
    int XMLParser(GdomeNodeList *childs, GdomeDocument *doc);
    int CDATAParser();
    std::list<vobsCALIBRATOR_STAR> GetList(); 

protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsPARSER& operator=(const vobsPARSER&);
    vobsPARSER (const vobsPARSER&);
    
    vobsCALIBRATOR_STAR_LIST finalList;
    std::list<vobsCALIBRATOR_STAR> listOfStar;
    std::vector<char*>tab;
    std::vector<char*>ucd;
    int nbLineToJump;
    char *CDATA;
};



#endif /*!vobsPARSER_H*/


/*___oOo___*/
