#ifndef vobsCDATA_H
#define vobsCDATA_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCDATA.h,v 1.3 2005-01-26 08:10:32 scetre Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
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

    virtual mcsCOMPL_STAT addColName(char *colName);
    virtual mcsCOMPL_STAT addUcdName(char *ucdName);
    virtual mcsUINT32     getNbColumns(void);
    virtual mcsCOMPL_STAT getNextColDesc(char **colName, char **ucdName,
                                         mcsLOGICAL init);
    virtual mcsCOMPL_STAT setNbLinesToSkip(mcsINT32 nbLines);
    virtual mcsUINT32     getNbLinesToSkip(void);
    virtual mcsCOMPL_STAT appendLines(char *buffer);
    virtual mcsUINT32     getNbLines(void);
    virtual char         *getNextLine(char *linePtr);

    virtual mcsCOMPL_STAT save(char *fileName);

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
};

#endif /*!vobsCDATA_H*/

/*___oOo___*/
