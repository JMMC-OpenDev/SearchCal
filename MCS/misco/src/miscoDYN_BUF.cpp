/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: miscoDYN_BUF.cpp,v 1.1 2005-02-11 09:39:41 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * \file
 * Interface class providing all the necessary API to manage auto-expanding,
 * byte-based buffers.
 */

static char *rcsId="@(#) $Id: miscoDYN_BUF.cpp,v 1.1 2005-02-11 09:39:41 gzins Exp $"; 
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
 * Allocate and add a number of bytes to a Dynamic Buffer already allocated
 * bytes.
 *
 * If the Dynamic Buffer already has some allocated bytes, its length is
 * expanded by the desired one. If the Dynamic Buffer is enlarged, the previous
 * content will remain untouched after the reallocation. New allocated bytes
 * will all contain '0'.
 *
 * \remark The call to this function is optional, as a Dynamic Buffer will
 * expand itself on demand when invoquing other miscoDYN_BUF methods as
 * AppendBytes(), InsertBytesAt(), etc... So, this method is only usefull when
 * you know by advance the maximum bytes length the Dynamic Buffer can reach
 * accross its entire life, and thus want to minimize the CPU time spent to
 * expand the Dynamic Buffer allocated memory on demand.\n\n
 *  
 * \param length the number of bytes by which the Dynamic Buffer should be
 * \em expanded (if length value is less than or equal to 0, nothing is done).
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT miscoDYN_BUF::Alloc               (const mcsINT32    length)
{
    logExtDbg("miscoDYN_BUF::Alloc()");

    return miscDynBufAlloc(&_dynBuf, length);
}

/**
 * Dealloc the unused allocated memory of a Dynamic Buffer.
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT miscoDYN_BUF::Strip               ()
{
    logExtDbg("miscoDYN_BUF::Strip()");

    return miscDynBufStrip(&_dynBuf);
}

/**
 * Reset a Dynamic Buffer.
 *
 * Possibly allocated memory remains untouched, until the reseted Dynamic Buffer
 * is reused to store other bytes.
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT miscoDYN_BUF::Reset               ()
{
    logExtDbg("miscoDYN_BUF::Reset()");

    return miscDynBufReset(&_dynBuf);
}

/**
 * Give back the number of bytes stored in a Dynamic Buffer.
 *
 * \param storedBytes the address of to the extern mcsUINT32 that will hold
 * the Dynamic Buffer number of stored bytes
 *
 * \return the stored length of a Dynamic Buffer, or 0 if an error occured
 */
mcsCOMPL_STAT miscoDYN_BUF::GetNbStoredBytes    (mcsUINT32   *storedBytes) const
{
    logExtDbg("miscoDYN_BUF::GetNbStoredBytes()");

    return miscDynBufGetNbStoredBytes(&_dynBuf, storedBytes);
}

/**
 * Give back the number of bytes allocated in a Dynamic Buffer.
 *
 * \param allocatedBytes the address of to the extern mcsUINT32 that will hold
 * the Dynamic Buffer number of allocated bytes
 *
 * \return the allocated length of a Dynamic Buffer, or 0 if an error occured
 */
mcsCOMPL_STAT miscoDYN_BUF::GetNbAllocatedBytes (mcsUINT32   *allocatedBytes) const
{
    logExtDbg("miscoDYN_BUF::GetNbAllocatedBytes()");

    return miscDynBufGetNbAllocatedBytes(&_dynBuf, allocatedBytes);
}

/**
 * Return a pointer to a Dynamic Buffer internal bytes buffer.
 *
 * \return a pointer to a Dynamic Buffer buffer, or NULL if an error occured
 */
char*         miscoDYN_BUF::GetBuffer           () const
{
    logExtDbg("miscoDYN_BUF::GetBuffer()");

    return miscDynBufGetBuffer(&_dynBuf);
}

/**
 * Return a pointer to a Dynamic Buffer internal comment pattern string.
 *
 * \return a pointer to a Dynamic Buffer comment pattern, or NULL if an error
 * occured
 */
const char*   miscoDYN_BUF::GetCommentPattern   () const
{
    logExtDbg("miscoDYN_BUF::GetCommentPattern()");

    return miscDynBufGetCommentPattern(&_dynBuf);
}

/**
 * Return a pointer to the next line of a Dynamic Buffer, skipping lines
 * beginning with the defined comment pattern if specified.
 *
 * \param currentLinePtr the address of the line from which we need to find the
 * next, or NULL to begin on the first line of the file
 * \param skipCommentFlag the boolean specifying weither the line beginnig by
 * the Dynamic Buffer comment pattern should be skipped or not
 *
 * \sa SetCommentPattern()
 *
 * \return a pointer to the next line of a Dynamic Buffer buffer, or NULL if an
 * error occured
 */
char*         miscoDYN_BUF::GetNextLine         (const char  *currentLinePtr,
                                           const mcsLOGICAL  skipCommentFlag)
{
    logExtDbg("miscoDYN_BUF::GetNextLine()");

    return miscDynBufGetNextLine(&_dynBuf, currentLinePtr, skipCommentFlag);
}

/**
 * Give back a Dynamic Buffer byte stored at a given position.
 *
 * \warning The first Dynamic Buffer byte has the position value defined by the
 * miscDYN_BUF_BEGINNING_POSITION macro.\n\n
 *
 * \param byte the address of to the extern byte that will hold the seeked
 *  Dynamic Buffer byte
 * \param position the position of the Dynamic Buffer seeked byte
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT miscoDYN_BUF::GetByteAt           (char              *byte,
                                                 const mcsUINT32   position)
{
    logExtDbg("miscoDYN_BUF::GetByteAt()");

    return miscDynBufGetByteAt(&_dynBuf, byte, position);
}

/**
 * Give back a part of a Dynamic Buffer in an already allocated extern buffer.
 *
 * \warning The first Dynamic Buffer byte has the position value defined by the
 * miscDYN_BUF_BEGINNING_POSITION macro.\n\n
 *
 * \param bytes the address of the receiving, already allocated extern buffer
 * \param from the first Dynamic Buffer byte to be copied in the extern buffer
 * \param to the last Dynamic Buffer byte to be copied in the extern buffer
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT miscoDYN_BUF::GetBytesFromTo      (char              *bytes,
                                                 const mcsUINT32   from,
                                                 const mcsUINT32   to)
{
    logExtDbg("miscoDYN_BUF::GetBytesFromTo()");

    return miscDynBufGetBytesFromTo(&_dynBuf, bytes, from, to);
}

/**
 * Give back a part of a Dynamic Buffer as a null terminated string, in an
 * already allocated extern buffer.
 *
 * \warning The first Dynamic Buffer byte has the position value defined by the
 * miscDYN_BUF_BEGINNING_POSITION macro.\n\n
 *
 * \param str the address of the receiving, already allocated extern buffer
 * \param from the first Dynamic Buffer byte to be copied in the extern string
 * \param to the last Dynamic Buffer byte to be copied in the extern string
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT miscoDYN_BUF::GetStringFromTo     (char              *str,
                                                 const mcsUINT32   from,
                                                 const mcsUINT32   to)
{
    logExtDbg("miscoDYN_BUF::GetStringFromTo()");

    return miscDynBufGetStringFromTo(&_dynBuf, str, from, to);
}

/**
 * Set the null-terminated string holding the comment pattern to be skipped when
 * running throughout the Dynamic Buffer line by line.
 *
 * If no comment pattern is specified, no line will be skipped while using
 * GetNextLine()
 *
 * \param commentPattern a null-terminated string defining the comment pattern
 * that can be skipped when using GetNextLine(), or NULL
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT miscoDYN_BUF::SetCommentPattern   (const char     *commentPattern)
{
    logExtDbg("miscoDYN_BUF::SetCommentPattern()");

    return miscDynBufSetCommentPattern(&_dynBuf, commentPattern);
}

/**
 * Overwrite a Dynamic Buffer with the content of a specified file.
 *
 * \warning The given Dynamic Buffer content (if any) will be \em destroyed by
 * this function call.\n\n
 * \warning The given file path must have been \em resolved before this function
 * call. See miscResolvePath() documentation for more information.\n\n
 * \warning The file buffer will have all its '\\n' character replaced by '\\0'.
 *
 * \param fileName the path specifying the file to be loaded in the Dynamic
 * Buffer
 * \param commentPattern (OPTIONNAL) the bytes that defines the comment pattern
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT miscoDYN_BUF::LoadFile            (const char     *fileName,
                                                 const char     *commentPattern)
{
    logExtDbg("miscoDYN_BUF::LoadFile()");

    return miscDynBufLoadFile(&_dynBuf, fileName, commentPattern);
}

/**
 * Put a Dynamic Buffer content in a specified file.
 *
 * \warning The given file will be over-written on each call.
 * \warning The given file path must have been \em resolved before this function
 * call. See miscResolvePath() documentation for more information.\n\n
 *
 * \param fileName the path specifying the file to be over-written with the
 * Dynamic Buffer content
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT miscoDYN_BUF::SaveInFile          (const char        *fileName)
{
    logExtDbg("miscoDYN_BUF::SaveInFile()");

    return miscDynBufSaveInFile(&_dynBuf, fileName);
}

/**
 * Overwrite a Dynamic Buffer byte at a given position.
 *
 * \warning The first Dynamic Buffer byte has the position value defined by the
 * miscDYN_BUF_BEGINNING_POSITION macro.\n\n
 *
 * \param byte the byte to be written in the Dynamic Buffer
 * \param position the position of the Dynamic Buffer byte to be overwritten
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT miscoDYN_BUF::ReplaceByteAt       (char              byte,
                                                 const mcsUINT32   position)
{
    logExtDbg("miscoDYN_BUF::ReplaceByteAt()");

    return miscDynBufReplaceByteAt(&_dynBuf, byte, position);
}

/**
 * Replace a given range of Dynamic Buffer bytes by extern buffer bytes.
 *
 * The Dynamic Buffer replaced bytes will be overwritten. Their range can be
 * smaller or bigger than the extern buffer bytes number.
 *
 * \warning The first Dynamic Buffer byte has the position value defined by the
 * miscDYN_BUF_BEGINNING_POSITION macro.\n\n
 *
 * \param bytes the address of the extern buffer bytes to be written in
 * \param length the number of extern buffer bytes to be written in
 * \param from the position of the first Dynamic Buffer byte to be substituted
 * \param to the position of the last Dynamic Buffer byte to be substituted
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
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
 * Replace a given range of Dynamic Buffer bytes by an extern string without its
 * ending '\\0'.
 *
 * The Dynamic Buffer replaced bytes will be overwritten. Their range can be
 * smaller or bigger than the extern string length. If the end of the Dynamic
 * Buffer is to be replaced, the string ending '\\0' is keeped.
 *
 * \warning The first Dynamic Buffer byte has the position value defined by the
 * miscDYN_BUF_BEGINNING_POSITION macro.\n\n
 *
 * \param str the address of the extern string to be written in
 * \param from the position of the first Dynamic Buffer byte to be substituted
 * \param to the position of the last Dynamic Buffer byte to be substituted
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT miscoDYN_BUF::ReplaceStringFromTo (char              *str,
                                                 const mcsUINT32   from,
                                                 const mcsUINT32   to)
{
    logExtDbg("miscoDYN_BUF::ReplaceStringFromTo()");

    return miscDynBufReplaceStringFromTo(&_dynBuf, str, from, to);
}

/**
 * Copy extern buffer bytes at the end of a Dynamic Buffer.
 *
 * \param bytes the address of the extern buffer bytes to be written in
 * \param length the number of extern buffer bytes to be written in
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT miscoDYN_BUF::AppendBytes         (const char        *bytes,
                                                 const mcsUINT32   length)
{
    logExtDbg("miscoDYN_BUF::AppendBytes()");

    return miscDynBufAppendBytes(&_dynBuf, bytes, length);
}

/**
 * Copy an extern string (a null terminated char array) at the end of a
 * Dynamic Buffer, adding an '\\0' at the end of it.
 *
 * \param str the address of the extern string to be written in
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT miscoDYN_BUF::AppendString        (const char        *str)
{
    logExtDbg("miscoDYN_BUF::AppendString()");

    return miscDynBufAppendString(&_dynBuf, str);
}

/**
 * Insert extern buffer bytes in a Dynamic Buffer at a given position.
 *
 * The Dynamic Buffer bytes are not overwritten, but shiffted to the right.
 *
 * \warning The first Dynamic Buffer byte has the position value defined by the
 * miscDYN_BUF_BEGINNING_POSITION macro.\n\n
 *
 * \param bytes a pointer to the extern buffer bytes to be inserted
 * \param length the number of extern buffer bytes to be inserted
 * \param position the position of the first Dynamic Buffer byte to write at
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT miscoDYN_BUF::InsertBytesAt       (char              *bytes,
                                                 const mcsUINT32   length,
                                                 const mcsUINT32   position)
{
    logExtDbg("miscoDYN_BUF::InsertBytesAt()");

    return miscDynBufInsertBytesAt(&_dynBuf, bytes, length, position);
}

/**
 * Insert an extern string (a null terminated char array) without its ending
 * '\\0' in a Dynamic Buffer at a given position.
 *
 * The Dynamic Buffer bytes are not overwritten, but shiffted to the right.
 *
 * \warning The first Dynamic Buffer byte has the position value defined by the
 * miscDYN_BUF_BEGINNING_POSITION macro.\n\n
 *
 * \param str a pointer to the extern string to be inserted
 * \param position the position of the first Dynamic Buffer byte to write at
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT miscoDYN_BUF::InsertStringAt      (char              *str,
                                                 const mcsUINT32   position)
{
    logExtDbg("miscoDYN_BUF::InsertStringAt()");

    return miscDynBufInsertStringAt(&_dynBuf, str, position);
}

/**
 * Delete a given range of Dynamic Buffer bytes.
 *
 * \warning The first Dynamic Buffer byte has the position value defined by the
 * miscDYN_BUF_BEGINNING_POSITION macro.\n\n
 *
 * \param from the position of the first Dynamic Buffer byte to be deleted
 * \param to the position of the last Dynamic Buffer byte to be deleted
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
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
