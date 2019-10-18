/***************************************************************************
*                 Lempel, Ziv, Storer, and Szymanski Decoding
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
*   $Id: lzdecode.c,v 1.1.1.1 2004/12/06 14:26:18 michael Exp $
*   $Log: lzdecode.c,v $
*   Revision 1.1.1.1  2004/12/06 14:26:18  michael
*   initial release
*
*
****************************************************************************
*
* LZDecode: An ANSI C LZSS Decoding Routines
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
*   Function   : DecodeLZSSByFile
*   Description: This function will read an LZSS encoded input data and
*                write an output array.  This algorithm encodes strings as
*                16 bits (a 12 bit offset + a 4 bit length).
*   Parameters : inData - pointer to array to decode
*                outData - pointer to the array to write decoded output
*   Effects    : inData is decoded and written to outData.
*   Returned   : size of decoded data or 0 for failure.
****************************************************************************/
unsigned int DecodeLZSSFromArray(unsigned char *inData,
    unsigned char *outData, unsigned int encodedLen)
{
    int  i, c, nextChar;
    unsigned int decodeCount;
    encoded_string_t code;              /* offset/length code for string */
    array_stream_t *encodedData;        /* stream wrapper for inData */

    if ((inData == NULL) || (outData == NULL) || (encodedLen == 0))
    {
        return 0;
    }

    encodedData = MakeArrayStream(inData, AS_READ);

    if (encodedData == NULL)
    {
        return 0;
    }

    /************************************************************************
    * Fill the sliding window buffer with some known vales.  EncodeLZSS must
    * use the same values.  If common characters are used, there's an
    * increased chance of matching to the earlier strings.
    ************************************************************************/
    for (i = 0; i < WINDOW_SIZE; i++)
    {
        slidingWindow[i] = ' ';
    }

    nextChar = 0;
    decodeCount = 0;

    /* decode all the data until there aren't enough meaningful bits */
    while (ArrayStreamGetBitCount(encodedData) < ((encodedLen * 8) - 7))
    {
        c = ArrayStreamGetBit(encodedData);

        if (c == UNCODED)
        {
            /* uncoded character */
            c = ArrayStreamGetChar(encodedData);

            /* write out byte and put it in sliding window */
            outData[decodeCount++] = c;
            slidingWindow[nextChar] = c;
            nextChar = (nextChar + 1) % WINDOW_SIZE;
        }
        else
        {
            /* offset and length */
            code.offset = ArrayStreamGetChar(encodedData);
            code.length = ArrayStreamGetChar(encodedData);

            /* unpack offset and length */
            code.offset <<= 4;
            code.offset |= ((code.length & 0x00F0) >> 4);
            code.length = (code.length & 0x000F) + MAX_UNCODED + 1;

            /****************************************************************
            * Write out decoded string to array and lookahead.  It would be
            * nice to write to the sliding window instead of the lookahead,
            * but we could end up overwriting the matching string with the
            * new string if abs(offset - next char) < match length.
            ****************************************************************/
            for (i = 0; i < code.length; i++)
            {
                c = slidingWindow[(code.offset + i) % WINDOW_SIZE];
                outData[decodeCount++] = c;
                uncodedLookahead[i] = c;
            }

            /* write out decoded string to sliding window */
            for (i = 0; i < code.length; i++)
            {
                slidingWindow[(nextChar + i) % WINDOW_SIZE] =
                    uncodedLookahead[i];
            }

            nextChar = (nextChar + code.length) % WINDOW_SIZE;
        }
    }

    ReleaseArrayStream(encodedData);

    return decodeCount;
}
