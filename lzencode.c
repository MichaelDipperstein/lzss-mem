/***************************************************************************
*                 Lempel, Ziv, Storer, and Szymanski Encoding
*
*   File    : lzdecode.c
*   Purpose : Use lzss coding (Storer and Szymanski's modified LZ77) to
*             decompress lzss encoded data.
*   Author  : Michael Dipperstein
*   Date    : November 07, 2004
*
****************************************************************************
*   UPDATES
*
*   Date        Change
*   12/10/03    Changed handling of sliding window to better match standard
*               algorithm description.
*   12/11/03    Remebered to copy encoded characters to the sliding window
*               even when there are no more characters in the input stream.
*
*
*   Revision 1.2  2004/02/22 17:14:26  michael
*   - Separated encode/decode, match finding, and main.
*   - Use bitfiles for reading/writing files
*   - Use traditional LZSS encoding where the coded/uncoded bits
*     precede the symbol they are associated with, rather than
*     aggregating the bits.
*
*   Revision 1.1.1.1  2004/01/21 06:25:49  michael
*   Initial version
*
*   11/07/04    Separated encode and decode functions for improved
*               modularity.
*
*   Revision 1.2  2004/11/13 22:51:00  michael
*   Provide distinct names for by file and by name functions and add some
*   comments to make their usage clearer.
*
*   Revision 1.1  2004/11/08 05:54:18  michael
*   1. Split encode and decode routines for smarter linking
*   2. Renamed lzsample.c sample.c to match my other samples
*   3. Makefile now builds code as libraries for better LGPL compliance.
*
*   11/28/04    Forked from baseline to create in memory FromArray
*               functions.
*
*   $Id: lzencode.c,v 1.1.1.1 2004/12/06 14:26:18 michael Exp $
*   $Log: lzencode.c,v $
*   Revision 1.1.1.1  2004/12/06 14:26:18  michael
*   initial release
*
*
****************************************************************************
*
* LZEncode: An ANSI C LZSS Encoding Routines
* Copyright (C) 2003-2004 by Michael Dipperstein (mdipperstein@gmail.com)
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
***************************************************************************/

/***************************************************************************
*                             INCLUDED FILES
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lzlocal.h"
#include "arraystream/arraystream.h"

/***************************************************************************
*                            TYPE DEFINITIONS
***************************************************************************/

/***************************************************************************
*                                CONSTANTS
***************************************************************************/

/***************************************************************************
*                            GLOBAL VARIABLES
***************************************************************************/
/* cyclic buffer sliding window of already read characters */
extern unsigned char slidingWindow[];
extern unsigned char uncodedLookahead[];

/***************************************************************************
*                               PROTOTYPES
***************************************************************************/

/***************************************************************************
*                                FUNCTIONS
***************************************************************************/

/****************************************************************************
*   Function   : EncodeLZSSFromArray
*   Description: This function will read an array of data and write out an
*                lzss encoded array.  This algorithm encodes strings as
*                16 bits (a 12 bit offset + a 4 bit length).
*   Parameters : inData - pointer to array to encode
*                outData - pointer to the array to write encoded output
*   Effects    : inData is encoded and written to outData.
*   Returned   : size of encoded data or 0 for failure.
****************************************************************************/
unsigned int EncodeLZSSFromArray(unsigned char *inData,
    unsigned char *outData, unsigned int uncodedLen)
{
    encoded_string_t matchData;
    int i;
    int len;                        /* length of string */
    unsigned int index;             /* index of character being encoded */
    int windowHead, uncodedHead;    /* head of sliding window and lookahead */
    array_stream_t *encodedData;    /* stream wrapper for outData */

    if ((inData == NULL) || (outData == NULL) || (uncodedLen == 0))
    {
        return 0;
    }

    encodedData = MakeArrayStream(outData, AS_WRITE);

    index = 0;
    windowHead = 0;
    uncodedHead = 0;

    /************************************************************************
    * Fill the sliding window buffer with some known vales.  DecodeLZSS must
    * use the same values.  If common characters are used, there's an
    * increased chance of matching to the earlier strings.
    ************************************************************************/
    for (i = 0; i < WINDOW_SIZE; i++)
    {
        slidingWindow[i] = ' ';
    }

    /************************************************************************
    * Copy MAX_CODED bytes from the input file into the uncoded lookahead
    * buffer.
    ************************************************************************/
    for (len = 0; (len < MAX_CODED) && (index < uncodedLen); len++)
    {
        uncodedLookahead[len] = inData[index++];
    }

    if (len == 0)
    {
        return (EXIT_SUCCESS);   /* inFile was empty */
    }

    /* Look for matching string in sliding window */
    InitializeSearchStructures();
    matchData = FindMatch(windowHead, uncodedHead);

    /* now encoded the rest of the file until an EOF is read */
    while (len > 0)
    {
        if (matchData.length > len)
        {
            /* garbage beyond last data happened to extend match length */
            matchData.length = len;
        }

        if (matchData.length <= MAX_UNCODED)
        {
            /* not long enough match.  write uncoded flag and character */
            ArrayStreamPutBit(UNCODED, encodedData);
            ArrayStreamPutChar(uncodedLookahead[uncodedHead], encodedData);

            matchData.length = 1;   /* set to 1 for 1 byte uncoded */
        }
        else
        {
            /* match length > MAX_UNCODED.  Encode as offset and length. */
            ArrayStreamPutBit(ENCODED, encodedData);
            ArrayStreamPutChar(
                (unsigned char)((matchData.offset & 0x0FFF) >> 4),
                encodedData);
            ArrayStreamPutChar(
                (unsigned char)(((matchData.offset & 0x000F) << 4) |
                (matchData.length - (MAX_UNCODED + 1))), encodedData);
        }

        /********************************************************************
        * Replace the matchData.length worth of bytes we've matched in the
        * sliding window with new bytes from the input file.
        ********************************************************************/
        i = 0;
        while ((i < matchData.length) && (index < uncodedLen))
        {
            /* add old byte into sliding window and new into lookahead */
            ReplaceChar(windowHead, uncodedLookahead[uncodedHead]);
            uncodedLookahead[uncodedHead] = inData[index++];
            windowHead = (windowHead + 1) % WINDOW_SIZE;
            uncodedHead = (uncodedHead + 1) % MAX_CODED;
            i++;
        }

        /* handle case where we hit EOF before filling lookahead */
        while (i < matchData.length)
        {
            ReplaceChar(windowHead, uncodedLookahead[uncodedHead]);
            /* nothing to add to lookahead here */
            windowHead = (windowHead + 1) % WINDOW_SIZE;
            uncodedHead = (uncodedHead + 1) % MAX_CODED;
            len--;
            i++;
        }

        /* find match for the remaining characters */
        matchData = FindMatch(windowHead, uncodedHead);
    }

    /* get the number of bits we've encoded and padfil last byte */
    index = ArrayStreamGetBitCount(encodedData);

    while (index % 8)
    {
        ArrayStreamPutBit(UNCODED, encodedData);
        index++;
    }

    index = (index / 8);        /* number of bytes encoded */

    /* we've decoded everything, free array stream structure */
    ReleaseArrayStream(encodedData);

   return (index);
}
