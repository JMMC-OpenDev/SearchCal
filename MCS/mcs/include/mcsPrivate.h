#ifndef mcsPrivate_H
#define mcsPrivate_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/* The following piece of code alternates the linkage type to C for all 
functions declared within the braces, which is necessary to use the 
functions in C++-code.
*/

#ifdef __cplusplus
extern C {
#endif

/* Module name */
#define MODULE_ID   "mcs"

/** thread informations (identifier and name) */
typedef struct
{
    mcsUINT32 id;     /* thread identifier */
    mcsSTRING16 name; /* thread name */
} mcsTHREAD_INFO;    
    
#ifdef __cplusplus
}
#endif

#endif /*!mcsPrivate_H*/


/*___oOo___*/
