#ifndef fndOBJECT_H
#define fndOBJECT_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: fndOBJECT.h,v 1.1 2004-09-22 13:39:39 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     22-Sep-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * Declaration of the fndOBJECT class.
 */


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * Class declaration
 */

/**
 * Foundation class of the inheritance scheme
 * 
 * This is the base class for all new classes. All new classes must be derived
 * from this one. This root class, from which all other classes are
 * hierarchically derived, is typical of many generic foundation libraries in
 * C++ and is the foundation of the inheritance scheme in many other OO
 * languages like SmallTalk or Java.
 *
 * \n
 * \ex
 * \n Declaration of a new class 
 * \code
 * class newCLASS : public fndOBJECT
 * {
 * public:
 * newCLASS();
 * virtual ~newCLASS();
 * ... 
 *
 * protected:
 * ... 
 *
 * private:
 * ... 
 * };
 * \endcode
 *
 */

class fndOBJECT
{
public:
    fndOBJECT();
    virtual ~fndOBJECT();

protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    fndOBJECT& operator=(const fndOBJECT&);
    fndOBJECT (const fndOBJECT&);
};


#endif /*!fndOBJECT_H*/

/*___oOo___*/
