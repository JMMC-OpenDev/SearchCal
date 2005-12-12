#ifndef sclsvrSERVER_H
#define sclsvrSERVER_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrSERVER.h,v 1.12 2005-12-12 14:10:00 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.11  2005/11/24 13:21:26  scetre
 * Added an element scenario faint K
 *
 * Revision 1.10  2005/11/21 13:51:47  scetre
 * Changed bad scenario name
 * Added scenario for single star research -> updated getStar
 *
 * Revision 1.9  2005/11/15 15:01:19  scetre
 * Updated with new scenario structure
 *
 * Revision 1.8  2005/10/26 11:27:24  lafrasse
 * Code review
 *
 * Revision 1.7  2005/03/06 20:29:40  gzins
 * Added GetSwVersion method
 *
 * Revision 1.6  2005/02/07 15:00:23  gzins
 * Added CVS log as modification history
 *
 * scetre    04-Oct-2004  Created
 *
 ******************************************************************************/

/**
 * @file
 * sclsvrSERVER class declaration.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * MCS Headers 
 */
#include "evh.h"


/*
 * SCALIB Headers 
 */
#include "vobs.h"
#include "sclsvrSCENARIO_BRIGHT_K.h"
#include "sclsvrSCENARIO_BRIGHT_K_OLD.h"
#include "sclsvrSCENARIO_BRIGHT_V.h"
#include "sclsvrSCENARIO_BRIGHT_N.h"
#include "sclsvrSCENARIO_FAINT_K.h"
#include "sclsvrSCENARIO_SINGLE_STAR.h"

/*
 * Class declaration
 */
/**
 * Brief description of the class, which ends at this dot.
 * 
 * OPTIONAL detailed description of the class follows here.
 *
 * @usedfiles
 * OPTIONAL. If files are used, for each one, name, and usage description.
 * @filename fileName1 :  usage description of fileName1
 * @filename fileName2 :  usage description of fileName2
 *
 * @n
 * @env
 * OPTIONAL. If needed, environmental variables accessed by the class. For
 * each variable, name, and usage description, as below.
 * @envvar envVar1 :  usage description of envVar1
 * @envvar envVar2 :  usage description of envVar2
 * 
 * @n
 * @warning OPTIONAL. Warning if any (software requirements, ...)
 *
 * @n
 * @ex
 * OPTIONAL. Code example if needed
 * @n Brief example description.
 * @code
 * Insert your code example here
 * @endcode
 *
 * @sa OPTIONAL. See also section, in which you can refer other documented
 * entities. Doxygen will create the link automatically.
 * @sa modcppMain.C
 * 
 * @bug OPTIONAL. Bugs list if it exists.
 * @bug For example, description of the first bug
 * @bug For example, description of the second bug
 * 
 * @todo OPTIONAL. Things to forsee list, if needed. For example, 
 * @todo add other methods, dealing with operations.
 * 
 */
class sclsvrSERVER : public evhSERVER 
{

public:
    // Constructor
    sclsvrSERVER();

    // Destructor
    virtual ~sclsvrSERVER();
    
    // Application initialization 
    virtual mcsCOMPL_STAT AppInit();
    
    // Software version 
    virtual const char *GetSwVersion();

    // Command callbacks
    virtual evhCB_COMPL_STAT GetCalCB(msgMESSAGE &msg, void*);
    virtual evhCB_COMPL_STAT GetStarCB(msgMESSAGE &msg, void*);

protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    sclsvrSERVER(const sclsvrSERVER&);
    sclsvrSERVER& operator=(const sclsvrSERVER&); 

    // Virtual observatory
    vobsVIRTUAL_OBSERVATORY _virtualObservatory;
    sclsvrSCENARIO_BRIGHT_K _scenarioBrightK;
    sclsvrSCENARIO_BRIGHT_K_OLD _scenarioBrightKOld;
    sclsvrSCENARIO_BRIGHT_V _scenarioBrightV;
    sclsvrSCENARIO_BRIGHT_N _scenarioBrightN;
    sclsvrSCENARIO_FAINT_K _scenarioFaintK;
    sclsvrSCENARIO_SINGLE_STAR _scenarioSingleStar;
};

#endif /*!sclsvrSERVER_H*/

/*___oOo___*/
