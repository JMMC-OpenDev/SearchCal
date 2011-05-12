/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Interface class providing all the necessary API to manage auto-expanding,
 * byte-based buffers.
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: miscoDYN_BUF.cpp,v 1.11 2010-01-15 17:27:51 lafrasse Exp $";
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
 * @return A pointer on the internal miscDYN_BUF.
 */
miscDYN_BUF* miscoDYN_BUF::GetInternalMiscDYN_BUF()
{
    logExtDbg("miscoDYN_BUF::GetInternalMiscDYN_BUF()");

    return &_dynBuf;
}

/**
 * @sa miscDynBufAlloc() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::Alloc(const mcsINT32 length)
{
    logExtDbg("miscoDYN_BUF::Alloc()");

    return miscDynBufAlloc(&_dynBuf, length);
}

/**
 * @sa miscDynBufStrip() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::Strip(void)
{
    logExtDbg("miscoDYN_BUF::Strip()");

    return miscDynBufStrip(&_dynBuf);
}

/**
 * @sa miscDynBufReset() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::Reset(void)
{
    logExtDbg("miscoDYN_BUF::Reset()");

    return miscDynBufReset(&_dynBuf);
}

/**
 * @sa miscDynBufGetNbStoredBytes() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::GetNbStoredBytes(mcsUINT32 *storedBytes) const
{
    logExtDbg("miscoDYN_BUF::GetNbStoredBytes()");

    return miscDynBufGetNbStoredBytes(&_dynBuf, storedBytes);
}

/**
 * @sa miscDynBufGetNbAllocatedBytes() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::GetNbAllocatedBytes(mcsUINT32 *allocatedBytes) const
{
    logExtDbg("miscoDYN_BUF::GetNbAllocatedBytes()");

    return miscDynBufGetNbAllocatedBytes(&_dynBuf, allocatedBytes);
}

/**
 * @sa miscDynBufGetBuffer() documentation in the 'misc' module
 */
char* miscoDYN_BUF::GetBuffer(void) const
{
    logExtDbg("miscoDYN_BUF::GetBuffer()");

    return miscDynBufGetBuffer(&_dynBuf);
}

/**
 * @sa miscDynBufGetCommentPattern() documentation in the 'misc' module
 */
const char* miscoDYN_BUF::GetCommentPattern(void) const
{
    logExtDbg("miscoDYN_BUF::GetCommentPattern()");

    return miscDynBufGetCommentPattern(&_dynBuf);
}

/**
 * @sa miscDynBufGetNextLine() documentation in the 'misc' module
 */
const char* miscoDYN_BUF::GetNextLine(const char        *currentPos,
                                            char        *nextLine,
                                      const mcsUINT32   maxLineLength,
                                      const mcsLOGICAL  skipCommentFlag)
{
    logExtDbg("miscoDYN_BUF::GetNextLine()");

    return miscDynBufGetNextLine(&_dynBuf, currentPos, nextLine, maxLineLength,
                                 skipCommentFlag);
}

/**
 * @sa miscDynBufGetNextCommentLine() documentation in the 'misc' module
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
 * @sa miscDynBufGetByteAt() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::GetByteAt(char              *byte,
                                      const mcsUINT32   position)
{
    logExtDbg("miscoDYN_BUF::GetByteAt()");

    return miscDynBufGetByteAt(&_dynBuf, byte, position);
}

/**
 * @sa miscDynBufGetBytesFromTo() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::GetBytesFromTo(char              *bytes,
                                           const mcsUINT32   from,
                                           const mcsUINT32   to)
{
    logExtDbg("miscoDYN_BUF::GetBytesFromTo()");

    return miscDynBufGetBytesFromTo(&_dynBuf, bytes, from, to);
}

/**
 * @sa miscDynBufGetStringFromTo() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::GetStringFromTo(char              *str,
                                            const mcsUINT32   from,
                                            const mcsUINT32   to)
{
    logExtDbg("miscoDYN_BUF::GetStringFromTo()");

    return miscDynBufGetStringFromTo(&_dynBuf, str, from, to);
}

/**
 * @sa miscDynBufSetCommentPattern() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::SetCommentPattern(const char *commentPattern)
{
    logExtDbg("miscoDYN_BUF::SetCommentPattern()");

    return miscDynBufSetCommentPattern(&_dynBuf, commentPattern);
}

/**
 * @sa miscDynBufExecuteCommand() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::ExecuteCommand(const char *command)
{
    logExtDbg("miscoDYN_BUF::ExecuteCommand()");

    return miscDynBufExecuteCommand(&_dynBuf, command);
}

/**
 * @sa miscDynBufLoadFile() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::LoadFile(const char  *fileName,
                                     const char  *commentPattern)
{
    logExtDbg("miscoDYN_BUF::LoadFile()");

    return miscDynBufLoadFile(&_dynBuf, fileName, commentPattern);
}

/**
 * @sa miscDynBufSavePartInFile() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::SavePartInFile(const mcsUINT32   length,
                                           const char       *fileName)
{
    logExtDbg("miscoDYN_BUF::SavePartInFile()");

    return miscDynBufSavePartInFile(&_dynBuf, length, fileName);
}

/**
 * @sa miscDynBufSaveInFile() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::SaveInFile(const char *fileName)
{
    logExtDbg("miscoDYN_BUF::SaveInFile()");

    return miscDynBufSaveInFile(&_dynBuf, fileName);
}

/**
 * @sa miscDynBufSaveInASCIIFile() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::SaveInASCIIFile(const char *fileName)
{
    logExtDbg("miscoDYN_BUF::SaveInASCIIFile()");

    return miscDynBufSaveInASCIIFile(&_dynBuf, fileName);
}

/**
 * @sa miscDynBufReplaceByteAt() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::ReplaceByteAt(const char        byte,
                                          const mcsUINT32   position)
{
    logExtDbg("miscoDYN_BUF::ReplaceByteAt()");

    return miscDynBufReplaceByteAt(&_dynBuf, byte, position);
}

/**
 * @sa miscDynBufReplaceBytesFromTo() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::ReplaceBytesFromTo(const char       *bytes,
                                               const mcsUINT32   length,
                                               const mcsUINT32   from,
                                               const mcsUINT32   to)
{
    logExtDbg("miscoDYN_BUF::ReplaceBytesFromTo()");

    return miscDynBufReplaceBytesFromTo(&_dynBuf, bytes, length, from, to);
}

/**
 * @sa miscDynBufReplaceStringFromTo() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::ReplaceStringFromTo(const char       *str,
                                                const mcsUINT32   from,
                                                const mcsUINT32   to)
{
    logExtDbg("miscoDYN_BUF::ReplaceStringFromTo()");

    return miscDynBufReplaceStringFromTo(&_dynBuf, str, from, to);
}

/**
 * @sa miscDynBufAppendBytes() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::AppendBytes(const char        *bytes,
                                        const mcsUINT32   length)
{
    logExtDbg("miscoDYN_BUF::AppendBytes()");

    return miscDynBufAppendBytes(&_dynBuf, bytes, length);
}

/**
 * @sa miscDynBufAppendString() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::AppendString(const char *str)
{
    logExtDbg("miscoDYN_BUF::AppendString()");

    return miscDynBufAppendString(&_dynBuf, str);
}

/**
 * @sa miscDynBufAppendLine() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::AppendLine(const char *line)
{
    logExtDbg("miscoDYN_BUF::AppendLine()");

    return miscDynBufAppendLine(&_dynBuf, line);
}

/**
 * @sa miscDynBufAppendCommentLine() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::AppendCommentLine(const char *line)
{
    logExtDbg("miscoDYN_BUF::AppendCommentLine()");

    return miscDynBufAppendCommentLine(&_dynBuf, line);
}

/**
 * @sa miscDynBufInsertBytesAt() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::InsertBytesAt(const char       *bytes,
                                          const mcsUINT32   length,
                                          const mcsUINT32   position)
{
    logExtDbg("miscoDYN_BUF::InsertBytesAt()");

    return miscDynBufInsertBytesAt(&_dynBuf, bytes, length, position);
}

/**
 * @sa miscDynBufInsertStringAt() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::InsertStringAt(const char       *str,
                                           const mcsUINT32   position)
{
    logExtDbg("miscoDYN_BUF::InsertStringAt()");

    return miscDynBufInsertStringAt(&_dynBuf, str, position);
}

/**
 * @sa miscDynBufDeleteBytesFromTo() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::DeleteBytesFromTo(const mcsUINT32  from,
                                              const mcsUINT32  to)
{
    logExtDbg("miscoDYN_BUF::DeleteBytesFromTo()");

    return miscDynBufDeleteBytesFromTo(&_dynBuf, from, to);
}


/**
 * Serialize the miscoDYN_BUF content in an output stream (as @em cout).
 *
 * @code
 *  #include <iostream>
 * #include "mcs.h"
 *
 * int main(int argc, char *argv[])
 * {
 *     // Initialize MCS services
 *     if (mcsInit(argv[0]) == mcsFAILURE)
 *     {
 *         // Exit from the application with mcsFAILURE
 *         exit (EXIT_FAILURE);
 *     }
 * 
 *     miscoDYN_BUF buffer;
 * 
 *     bytes = "hello buffer" ;
 *     bytesNumber = strlen(bytes);
 *     buffer.AppendBytes(bytes, bytesNumber);
 *     cout << buffer << endl;
 * 
 *     // Close MCS services
 *     mcsExit();
 *     
 *     // Exit from the application with SUCCESS
 *     exit (EXIT_SUCCESS);
 * }
 *
 * ...
 *
 * > miscoDYN_BUF =
 *   {
 *     storedBytes    = '13'
 *     allocatedBytes = '13'
 *     commentPattern = ''
 *     dynBuf         = 'hello dynStr'
 *   }
 * @endcode
 */
std::ostream& operator<< (std::ostream&       stream,
                          const miscoDYN_BUF& buffer)
{
    stream << "miscoDYN_BUF ="                                << endl
           << "{"                                             << endl;

    mcsUINT32 storedBytesNb = 0;
    if (buffer.GetNbStoredBytes(&storedBytesNb) == mcsFAILURE)
    {
        return stream << "  Invalid object" << endl
                      << "}";
    }

    mcsUINT32 allocatedBytesNb = 0;
    if (buffer.GetNbAllocatedBytes(&allocatedBytesNb) == mcsFAILURE)
    {
        return stream << "  Invalid object" << endl
                      << "}";
    }

    stream
    << "  storedBytes    = '" << storedBytesNb              << "'" << endl
    << "  allocatedBytes = '" << allocatedBytesNb           << "'" << endl
    << "  commentPattern = '" << buffer.GetCommentPattern() << "'" << endl;

    if (storedBytesNb > 0)
    {
        stream << "  dynBuf         = '"<< buffer.GetBuffer() << "'" << endl;
    }
    else
    {
        stream << "  dynBuf         = '(null)'"                      << endl;
    }

    return stream << "}";
}


/*
 * Protected methods
 */


/*
 * Private methods
 */


/*___oOo___*/
