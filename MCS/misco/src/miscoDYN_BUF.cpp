/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: miscoDYN_BUF.cpp,v 1.6 2005-02-22 15:10:53 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.5  2005/02/16 14:57:23  gzins
 * Updated prototype to GetNextLine
 *
 * Revision 1.4  2005/02/14 08:09:04  gzins
 * Implemented assignment operator and copy constructor
 *
 * Revision 1.3  2005/02/12 20:04:44  gzins
 * Go back to version 1.1
 *
 * Revision 1.1  2005/02/11 09:39:41  gzins
 * Created
 *
 ******************************************************************************/

/**
 * \file
 * Interface class providing all the necessary API to manage auto-expanding,
 * byte-based buffers.
 */

static char *rcsId="@(#) $Id: miscoDYN_BUF.cpp,v 1.6 2005-02-22 15:10:53 lafrasse Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers 
 */
#include <iostream>
using namespace std;

/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"

/*
 * Local Headers 
 */
#include "miscoDYN_BUF.h"
#include "miscoPrivate.h"

/**
 * Class constructor
 */
miscoDYN_BUF::miscoDYN_BUF()
{
    miscDynBufInit(&_dynBuf);
}

/**
 * Copy constructor
 */
miscoDYN_BUF::miscoDYN_BUF(const miscoDYN_BUF& dynBuf)
{
    *this = dynBuf;
}
/**
 * Assignment operator
 */
miscoDYN_BUF& miscoDYN_BUF::operator=(const miscoDYN_BUF &dynBuf)
{
    // Copy buffer content from the given one
    mcsUINT32 bufferSize;
    dynBuf.GetNbStoredBytes(&bufferSize);
    AppendBytes(dynBuf.GetBuffer(), bufferSize); 

    return *this;
}

/**
 * Class destructor
 */
miscoDYN_BUF::~miscoDYN_BUF()
{
    miscDynBufDestroy(&_dynBuf);
}

/*
 * Public methods
 */

/**
 * \sa miscDynBufAlloc() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::Alloc               (const mcsINT32    length)
{
    logExtDbg("miscoDYN_BUF::Alloc()");

    return miscDynBufAlloc(&_dynBuf, length);
}

/**
 * \sa miscDynBufStrip() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::Strip               ()
{
    logExtDbg("miscoDYN_BUF::Strip()");

    return miscDynBufStrip(&_dynBuf);
}

/**
 * \sa miscDynBufReset() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::Reset               ()
{
    logExtDbg("miscoDYN_BUF::Reset()");

    return miscDynBufReset(&_dynBuf);
}

/**
 * \sa miscDynBufGetNbStoredBytes() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::GetNbStoredBytes    (mcsUINT32   *storedBytes) const
{
    logExtDbg("miscoDYN_BUF::GetNbStoredBytes()");

    return miscDynBufGetNbStoredBytes(&_dynBuf, storedBytes);
}

/**
 * \sa miscDynBufGetNbAllocatedBytes() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::GetNbAllocatedBytes (mcsUINT32   *allocatedBytes) const
{
    logExtDbg("miscoDYN_BUF::GetNbAllocatedBytes()");

    return miscDynBufGetNbAllocatedBytes(&_dynBuf, allocatedBytes);
}

/**
 * \sa miscDynBufGetBuffer() documentation in the 'misc' module
 */
char*         miscoDYN_BUF::GetBuffer           () const
{
    logExtDbg("miscoDYN_BUF::GetBuffer()");

    return miscDynBufGetBuffer(&_dynBuf);
}

/**
 * \sa miscDynBufGetCommentPattern() documentation in the 'misc' module
 */
const char*   miscoDYN_BUF::GetCommentPattern   () const
{
    logExtDbg("miscoDYN_BUF::GetCommentPattern()");

    return miscDynBufGetCommentPattern(&_dynBuf);
}

/**
 * \sa miscDynBufGetNextLine() documentation in the 'misc' module
 */
const char*   miscoDYN_BUF::GetNextLine   (const char        *currentPos,
                                                 char        *nextLine,
                                           const mcsUINT32   maxLineLength,
                                           const mcsLOGICAL  skipCommentFlag)
{
    logExtDbg("miscoDYN_BUF::GetNextLine()");

    return miscDynBufGetNextLine(&_dynBuf, currentPos, nextLine, maxLineLength,
                                 skipCommentFlag);
}

/**
 * \sa miscDynBufGetNextCommentLine() documentation in the 'misc' module
 */
const char* miscoDYN_BUF::GetNextCommentLine(const char        *currentPos,
                                                   char        *nextLine,
                                             const mcsUINT32   maxLineLength)
{
    logExtDbg("miscoDYN_BUF::GetNextCommentLine()");

    return miscDynBufGetNextCommentLine(&_dynBuf, currentPos, nextLine,
                                        maxLineLength);
}

/**
 * \sa miscDynBufGetByteAt() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::GetByteAt           (char              *byte,
                                                 const mcsUINT32   position)
{
    logExtDbg("miscoDYN_BUF::GetByteAt()");

    return miscDynBufGetByteAt(&_dynBuf, byte, position);
}

/**
 * \sa miscDynBufGetBytesFromTo() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::GetBytesFromTo      (char              *bytes,
                                                 const mcsUINT32   from,
                                                 const mcsUINT32   to)
{
    logExtDbg("miscoDYN_BUF::GetBytesFromTo()");

    return miscDynBufGetBytesFromTo(&_dynBuf, bytes, from, to);
}

/**
 * \sa miscDynBufGetStringFromTo() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::GetStringFromTo     (char              *str,
                                                 const mcsUINT32   from,
                                                 const mcsUINT32   to)
{
    logExtDbg("miscoDYN_BUF::GetStringFromTo()");

    return miscDynBufGetStringFromTo(&_dynBuf, str, from, to);
}

/**
 * \sa miscDynBufSetCommentPattern() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::SetCommentPattern   (const char     *commentPattern)
{
    logExtDbg("miscoDYN_BUF::SetCommentPattern()");

    return miscDynBufSetCommentPattern(&_dynBuf, commentPattern);
}

/**
 * \sa miscDynBufLoadFile() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::LoadFile            (const char     *fileName,
                                                 const char     *commentPattern)
{
    logExtDbg("miscoDYN_BUF::LoadFile()");

    return miscDynBufLoadFile(&_dynBuf, fileName, commentPattern);
}

/**
 * \sa miscDynBufSaveInFile() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::SaveInFile          (const char        *fileName)
{
    logExtDbg("miscoDYN_BUF::SaveInFile()");

    return miscDynBufSaveInFile(&_dynBuf, fileName);
}

/**
 * \sa miscDynBufReplaceByteAt() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::ReplaceByteAt       (char              byte,
                                                 const mcsUINT32   position)
{
    logExtDbg("miscoDYN_BUF::ReplaceByteAt()");

    return miscDynBufReplaceByteAt(&_dynBuf, byte, position);
}

/**
 * \sa miscDynBufReplaceBytesFromTo() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::ReplaceBytesFromTo  (char              *bytes,
                                                 const mcsUINT32   length,
                                                 const mcsUINT32   from,
                                                 const mcsUINT32   to)
{
    logExtDbg("miscoDYN_BUF::ReplaceBytesFromTo()");

    return miscDynBufReplaceBytesFromTo(&_dynBuf, bytes, length, from, to);
}

/**
 * \sa miscDynBufReplaceStringFromTo() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::ReplaceStringFromTo (char              *str,
                                                 const mcsUINT32   from,
                                                 const mcsUINT32   to)
{
    logExtDbg("miscoDYN_BUF::ReplaceStringFromTo()");

    return miscDynBufReplaceStringFromTo(&_dynBuf, str, from, to);
}

/**
 * \sa miscDynBufAppendBytes() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::AppendBytes         (const char        *bytes,
                                                 const mcsUINT32   length)
{
    logExtDbg("miscoDYN_BUF::AppendBytes()");

    return miscDynBufAppendBytes(&_dynBuf, bytes, length);
}

/**
 * \sa miscDynBufAppendString() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::AppendString        (const char        *str)
{
    logExtDbg("miscoDYN_BUF::AppendString()");

    return miscDynBufAppendString(&_dynBuf, str);
}

/**
 * \sa miscDynBufAppendLine() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::AppendLine          (const char        *line)
{
    logExtDbg("miscoDYN_BUF::AppendLine()");

    return miscDynBufAppendLine(&_dynBuf, line);
}

/**
 * \sa miscDynBufAppendCommentLine() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::AppendCommentLine   (const char        *line)
{
    logExtDbg("miscoDYN_BUF::AppendCommentLine()");

    return miscDynBufAppendCommentLine(&_dynBuf, line);
}

/**
 * \sa miscDynBufInsertBytesAt() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::InsertBytesAt       (char              *bytes,
                                                 const mcsUINT32   length,
                                                 const mcsUINT32   position)
{
    logExtDbg("miscoDYN_BUF::InsertBytesAt()");

    return miscDynBufInsertBytesAt(&_dynBuf, bytes, length, position);
}

/**
 * \sa miscDynBufInsertStringAt() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::InsertStringAt      (char              *str,
                                                 const mcsUINT32   position)
{
    logExtDbg("miscoDYN_BUF::InsertStringAt()");

    return miscDynBufInsertStringAt(&_dynBuf, str, position);
}

/**
 * \sa miscDynBufDeleteBytesFromTo() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::DeleteBytesFromTo   (const mcsUINT32   from,
                                                 const mcsUINT32   to)
{
    logExtDbg("miscoDYN_BUF::DeleteBytesFromTo()");

    return miscDynBufDeleteBytesFromTo(&_dynBuf, from, to);
}


/**
 * Show the msgMESSAGE content on the standard output.
 */
mcsCOMPL_STAT miscoDYN_BUF::Display   () const
//std::ostream& operator<< (      std::ostream&  stream,
//                          const miscoDYN_BUF&  buffer)
{
    cout   << "miscoDYN_BUF ="                                << endl
           << "{"                                             << endl;

    mcsUINT32 storedBytesNb = 0;
    if (GetNbStoredBytes(&storedBytesNb) == mcsFAILURE)
    {
        cout << "\tInvalid object" << endl
             << "}" << endl;
        return mcsFAILURE;
    }

    mcsUINT32 allocatedBytesNb = 0;
    if (GetNbAllocatedBytes(&allocatedBytesNb) == mcsFAILURE)
    {
        cout << "\tInvalid object" << endl
             << "}"<< endl;;
        return mcsFAILURE;
    }

    cout
    << "\tstoredBytes   = '"  << storedBytesNb              << "'" << endl
    << "\tallocatedBytes = '" << allocatedBytesNb           << "'" << endl
    << "\tcommentPattern = '" << GetCommentPattern()        << "'" << endl;

    if (storedBytesNb > 0)
    {
        cout << "\tdynBuf = '"<< GetBuffer()                  << "'" << endl;
    }
    else
    {
        cout << "\tdynBuf = '(null)'"                         << endl;
    }

    cout << "}"<< endl;;
    return mcsSUCCESS;

/*    stream << "miscoDYN_BUF ="                                << endl
           << "{"                                             << endl;

    mcsUINT32 storedBytesNb = 0;
    if (buffer.GetNbStoredBytes(&storedBytesNb) == mcsFAILURE)
    {
        return stream << "\tInvalid object" << endl
                      << "}";
    }

    mcsUINT32 allocatedBytesNb = 0;
    if (buffer.GetNbAllocatedBytes(&allocatedBytesNb) == mcsFAILURE)
    {
        return stream << "\tInvalid object" << endl
                      << "}";
    }

    stream
    << "\t\tstoredBytes   = '"  << storedBytesNb              << "'" << endl
    << "\t\tallocatedBytes = '" << allocatedBytesNb           << "'" << endl
    << "\t\tcommentPattern = '" << buffer.GetCommentPattern() << "'" << endl;

    if (storedBytesNb > 0)
    {
        stream << "\tdynBuf = '"<< buffer.GetBuffer()         << "'" << endl;
    }
    else
    {
        stream << "\tdynBuf = '(null)'"                              << endl;
    }

    return stream << "}";*/
}


/*
 * Protected methods
 */


/*
 * Private methods
 */


/*___oOo___*/
