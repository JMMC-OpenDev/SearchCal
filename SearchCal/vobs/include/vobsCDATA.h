#ifndef vobsCDATA_H
#define vobsCDATA_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCDATA.h,v 1.5 2005-02-08 20:32:27 gzins Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.4  2005/02/07 09:47:08  gzins
* Renamed vobsCDATA method to be compliant with programming standards; method name starts with capital
*
* Revision 1.3  2005/01/26 08:10:32  scetre
* change history
*
* scetre    15-Nov-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * vobsCDATA class declaration.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * system header files
 */
#include <vector>

/*
 * MCS header files
 */
#include "mcs.h"
#include "misc.h"

/*
 * Class declaration
 */
/**
 * vobsCDATA represent the CDATA part of the resulting xml files coming from
 * the CDS.
 *
 * It is build with several part :
 * \li a list of UCD name.
 * \li a list of colum name.
 * \li a number of line to skip in order to get the information data in the
 * CDATA block
 * \li the body of the CDATA
 * \li the number of line stored in the CDATA
 **/
class vobsCDATA
{

public:
    // Constructor
    vobsCDATA();

    // Destructor
    virtual ~vobsCDATA();

    virtual mcsCOMPL_STAT SetCatalogName(const char *name);
    virtual const char   *GetCatalogName(void);

    virtual mcsCOMPL_STAT AddColName(char *colName);
    virtual mcsCOMPL_STAT AddUcdName(char *ucdName);
    virtual mcsUINT32     GetNbColumns(void);
    virtual mcsCOMPL_STAT GetNextColDesc(char **colName, char **ucdName,
                                         mcsLOGICAL init);
    virtual mcsCOMPL_STAT SetNbLinesToSkip(mcsINT32 nbLines);
    virtual mcsUINT32     GetNbLinesToSkip(void);
    virtual mcsCOMPL_STAT AppendLines(char *buffer);
    virtual mcsUINT32     GetNbLines(void);
    virtual char         *GetNextLine(char *linePtr);

    virtual mcsCOMPL_STAT Save(const char *fileName);

protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsCDATA(const vobsCDATA&);
    vobsCDATA& operator=(const vobsCDATA&);

    std::vector<char*> _colName; // Name of columns
    std::vector<char*> _ucdName; // Name of corresponding UCD
    std::vector<char *>::iterator _colNameIterator;
    std::vector<char *>::iterator _ucdNameIterator;

    int _nbLinesToSkip;          // Number of lines to be skipped in CDATA
                                 // section
    miscDYN_BUF _buffer;         // Dynamic buffer containg CDATA 
    int _nbLines;                // Number of lines stored in buffer

    string _catalogName;         // Catalog name from where CDATA comming from 
};

#endif /*!vobsCDATA_H*/

/*___oOo___*/
