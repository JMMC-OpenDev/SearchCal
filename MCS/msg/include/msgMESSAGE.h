#ifndef msgMESSAGE_H
#define msgMESSAGE_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgMESSAGE.h,v 1.1 2004-11-19 17:19:42 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    17-Nov-2004  Created
* lafrasse  19-Nov-2004  Changed all method name first letter to upper case
*
*
*******************************************************************************/

/**
 * \file
 * msgMESSAGE class declaration.
 */


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include "msg.h"

/*
 * Class declaration
 */

/**
 * msgMESSAGE class declaration.
 * 
 * msgMESSAGE class contains method which help to access to parameters of a
 * message and his body.
 * 
 *
 * \usedfiles
 * \filename mcs.h :  header file which contain the definition of the
 * msgMESSAGE_RAW and msgHEADER structure
 *
 * \sa msgMESSAGE.cpp
 * 
 */

class msgMESSAGE
{

public:
    // Brief description of the constructor
    msgMESSAGE                               (const mcsLOGICAL isInternal = mcsFALSE);

    // Brief description of the destructor
    virtual ~msgMESSAGE                      ();
    
    virtual msgMESSAGE_RAW*  GetMessageRaw   ();

    virtual char*            GetSender       ();
    virtual mcsCOMPL_STAT    SetSender       (const char     *buffer);

    virtual char*            GetSenderEnv    ();
    virtual mcsCOMPL_STAT    SetSenderEnv    (const char     *senderEnv);

    virtual char*            GetRecipient    ();
    virtual mcsCOMPL_STAT    SetRecipient    (const char     *recipient);

    virtual char*            GetRecipientEnv ();
    virtual mcsCOMPL_STAT    SetRecipientEnv (const char     *recipientEnv);

    virtual int              GetType         ();
    virtual mcsCOMPL_STAT    SetType         (const mcsUINT8  type);

    virtual char*            GetIdentifier   ();
    virtual mcsCOMPL_STAT    SetIdentifier   (const char     *identificator);

    virtual char*            GetCommand      ();
    virtual mcsCOMPL_STAT    SetCommand      (const char     *command);

    virtual char*            GetHeaderPtr    ();
    virtual mcsINT32         GetBodySize     ();
    virtual char*            GetBodyPtr      ();
    virtual mcsCOMPL_STAT    SetBody         (const char     *buffer,
                                              const mcsINT32  bufLen=0);

    virtual mcsLOGICAL       IsLastReply     ();
    virtual mcsLOGICAL       IsInternal();

protected:

    
private:
     msgMESSAGE_RAW _message;

    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
     msgMESSAGE(const msgMESSAGE&);
     msgMESSAGE& operator=(const msgMESSAGE&);
};




#endif /*!msgMESSAGE_H*/

/*___oOo___*/
