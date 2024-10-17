/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Interface class providing all the necessary API to manage auto-expanding,
 * byte-based buffers.
 */

/*
 * System Headers
 */
#include <iostream>
using namespace std;

/*
 * MCS Headers
 */
#include "mcs.h"
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
    // Uses the operator=() method to copy
    *this = dynBuf;
}

/**
 * Assignment operator
 */
miscoDYN_BUF& miscoDYN_BUF::operator=(const miscoDYN_BUF &dynBuf)
{
    if (this != &dynBuf)
    {
        // Copy buffer content from the given one
        miscDynSIZE bufferSize;
        dynBuf.GetNbStoredBytes(&bufferSize);
        AppendBytes(dynBuf.GetBuffer(), bufferSize);
    }
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
    return &_dynBuf;
}

mcsCOMPL_STAT miscoDYN_BUF::Reserve(const miscDynSIZE length)
{
    return miscDynBufReserve(&_dynBuf, length);
}

/**
 * @sa miscDynBufAlloc() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::Alloc(const miscDynSIZE length)
{
    return miscDynBufAlloc(&_dynBuf, length);
}

/**
 * @sa miscDynBufStrip() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::Strip(void)
{
    return miscDynBufStrip(&_dynBuf);
}

/**
 * @sa miscDynBufReset() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::Reset(void)
{
    return miscDynBufReset(&_dynBuf);
}

/**
 * @sa miscDynBufGetNbStoredBytes() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::GetNbStoredBytes(miscDynSIZE *storedBytes) const
{
    return miscDynBufGetNbStoredBytes(&_dynBuf, storedBytes);
}

/**
 * @sa miscDynBufGetNbAllocatedBytes() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::GetNbAllocatedBytes(miscDynSIZE *allocatedBytes) const
{
    return miscDynBufGetNbAllocatedBytes(&_dynBuf, allocatedBytes);
}

/**
 * @sa miscDynBufGetBuffer() documentation in the 'misc' module
 */
char* miscoDYN_BUF::GetBuffer(void) const
{
    return miscDynBufGetBuffer(&_dynBuf);
}

/**
 * @sa miscDynBufGetCommentPattern() documentation in the 'misc' module
 */
const char* miscoDYN_BUF::GetCommentPattern(void) const
{
    return miscDynBufGetCommentPattern(&_dynBuf);
}

/**
 * @sa miscDynBufGetNextLine() documentation in the 'misc' module
 */
const char* miscoDYN_BUF::GetNextLine(const char      *currentPos,
                                      char            *nextLine,
                                      const mcsUINT32  maxLineLength,
                                      const mcsLOGICAL skipCommentFlag)
{
    return miscDynBufGetNextLine(&_dynBuf, currentPos, nextLine, maxLineLength,
                                 skipCommentFlag);
}

/**
 * @sa miscDynBufGetNextCommentLine() documentation in the 'misc' module
 */
const char* miscoDYN_BUF::GetNextCommentLine(const char     *currentPos,
                                             char           *nextLine,
                                             const mcsUINT32 maxLineLength)
{
    return miscDynBufGetNextCommentLine(&_dynBuf, currentPos, nextLine,
                                        maxLineLength);
}

/**
 * @sa miscDynBufGetByteAt() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::GetByteAt(char             *byte,
                                      const miscDynSIZE position)
{
    return miscDynBufGetByteAt(&_dynBuf, byte, position);
}

/**
 * @sa miscDynBufGetBytesFromTo() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::GetBytesFromTo(char             *bytes,
                                           const miscDynSIZE from,
                                           const miscDynSIZE to)
{
    return miscDynBufGetBytesFromTo(&_dynBuf, bytes, from, to);
}

/**
 * @sa miscDynBufGetStringFromTo() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::GetStringFromTo(char             *str,
                                            const miscDynSIZE from,
                                            const miscDynSIZE to)
{
    return miscDynBufGetStringFromTo(&_dynBuf, str, from, to);
}

/**
 * @sa miscDynBufSetCommentPattern() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::SetCommentPattern(const char *commentPattern)
{
    return miscDynBufSetCommentPattern(&_dynBuf, commentPattern);
}

/**
 * @sa miscDynBufExecuteCommand() documentation in the 'misc' module
 */
mcsINT8 miscoDYN_BUF::ExecuteCommand(const char *command)
{
    return miscDynBufExecuteCommand(&_dynBuf, command);
}

/**
 * @sa miscDynBufLoadFile() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::LoadFile(const char  *fileName,
                                     const char  *commentPattern)
{
    return miscDynBufLoadFile(&_dynBuf, fileName, commentPattern);
}

/**
 * @sa miscDynBufSavePartInFile() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::SavePartInFile(const miscDynSIZE length,
                                           const char       *fileName)
{
    return miscDynBufSavePartInFile(&_dynBuf, length, fileName);
}

/**
 * @sa miscDynBufSaveInFile() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::SaveInFile(const char *fileName)
{
    return miscDynBufSaveInFile(&_dynBuf, fileName);
}

/**
 * @sa miscDynBufSaveInASCIIFile() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::SaveInASCIIFile(const char *fileName)
{
    return miscDynBufSaveInASCIIFile(&_dynBuf, fileName);
}

/**
 * @sa miscDynBufReplaceByteAt() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::ReplaceByteAt(const char        byte,
                                          const miscDynSIZE position)
{
    return miscDynBufReplaceByteAt(&_dynBuf, byte, position);
}

/**
 * @sa miscDynBufReplaceBytesFromTo() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::ReplaceBytesFromTo(const char       *bytes,
                                               const miscDynSIZE length,
                                               const miscDynSIZE from,
                                               const miscDynSIZE to)
{
    return miscDynBufReplaceBytesFromTo(&_dynBuf, bytes, length, from, to);
}

/**
 * @sa miscDynBufReplaceStringFromTo() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::ReplaceStringFromTo(const char       *str,
                                                const miscDynSIZE from,
                                                const miscDynSIZE to)
{
    return miscDynBufReplaceStringFromTo(&_dynBuf, str, from, to);
}

/**
 * @sa miscDynBufAppendBytes() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::AppendBytes(const char        *bytes,
                                        const miscDynSIZE length)
{
    return miscDynBufAppendBytes(&_dynBuf, bytes, length);
}

/**
 * @sa miscDynBufAppendString() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::AppendString(const char *str)
{
    return miscDynBufAppendString(&_dynBuf, str);
}

/**
 * @sa miscDynBufAppendLine() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::AppendLine(const char *line)
{
    return miscDynBufAppendLine(&_dynBuf, line);
}

/**
 * @sa miscDynBufAppendCommentLine() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::AppendCommentLine(const char *line)
{
    return miscDynBufAppendCommentLine(&_dynBuf, line);
}

/**
 * @sa miscDynBufInsertBytesAt() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::InsertBytesAt(const char       *bytes,
                                          const miscDynSIZE length,
                                          const miscDynSIZE position)
{
    return miscDynBufInsertBytesAt(&_dynBuf, bytes, length, position);
}

/**
 * @sa miscDynBufInsertStringAt() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::InsertStringAt(const char       *str,
                                           const miscDynSIZE position)
{
    return miscDynBufInsertStringAt(&_dynBuf, str, position);
}

/**
 * @sa miscDynBufDeleteBytesFromTo() documentation in the 'misc' module
 */
mcsCOMPL_STAT miscoDYN_BUF::DeleteBytesFromTo(const miscDynSIZE from,
                                              const miscDynSIZE to)
{
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
 *     // Initializes MCS services
 *     FAIL_DO(mcsInit(argv[0]), 
 *         // Exit from the application with mcsFAILURE
 *         errCloseStack(); 
 *         exit(EXIT_FAILURE);
 *     );
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

    miscDynSIZE storedBytesNb = 0;
    if (buffer.GetNbStoredBytes(&storedBytesNb) == mcsFAILURE)
    {
        return stream << "  Invalid object" << endl << "}";
    }

    miscDynSIZE allocatedBytesNb = 0;
    if (buffer.GetNbAllocatedBytes(&allocatedBytesNb) == mcsFAILURE)
    {
        return stream << "  Invalid object" << endl << "}";
    }

    stream  << "  storedBytes    = '" << storedBytesNb              << "'" << endl
            << "  allocatedBytes = '" << allocatedBytesNb           << "'" << endl
            << "  commentPattern = '" << buffer.GetCommentPattern() << "'" << endl;

    if (storedBytesNb > 0)
    {
        stream << "  dynBuf         = '" << buffer.GetBuffer() << "'" << endl;
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
