/***************************************************************************
*                           Array Stream File Header
*
*   File    : arraystream.h
*   Purpose : Provides definitions and prototypes for a simple library
*             allowing arrays of unsigned char to be treated as input/output
*             streams of bits.  This library was created with in-memory data
*             compression in mind, but may be used for a number of other
*             applications.
*
*             Array streams are FIFO (first in/first out) structures.  The
*             first bit written is the first bit returned.  This
*             implementation writes/reads starting with the MSB of index 0.
*
*   Author  : Michael Dipperstein
*   Date    : November 23, 2004
*
****************************************************************************
*   UPDATES
*
*   $Id: arraystream.h,v 1.2 2007/08/27 13:01:21 michael Exp $
*   $Log: arraystream.h,v $
*   Revision 1.2  2007/08/27 13:01:21  michael
*   Changes required for LGPL v3.
*
*   Revision 1.1.1.1  2004/12/06 13:41:26  michael
*   initial release
*
*
****************************************************************************
*
* Array Stream: Bit stream opperations on arrays
* Copyright (C) 2004, 2007 by Michael Dipperstein (mdipper@cs.ucsb.edu)
*
* This file is part of the array stream library.
*
* The array stream library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License as
* published by the Free Software Foundation; either version 3 of the
* License, or (at your option) any later version.
*
* The array stream library is distributed in the hope that it will be
* useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
* General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************/

#ifndef _ARRAYSTREAM_H_
#define _ARRAYSTREAM_H_

/***************************************************************************
*                             INCLUDED FILES
***************************************************************************/
#include <stdio.h>

/***************************************************************************
*                            TYPE DEFINITIONS
***************************************************************************/
/* incomplete type to hide implementation */
struct array_stream_t;
typedef struct array_stream_t array_stream_t;

/* read/write mode of stream */
typedef enum
{
    AS_NO_MODE,     /* uninitialized mode */
    AS_READ,        /* array stream is for reading */
    AS_WRITE        /* array steam is for writing */
} as_mode_t;

/***************************************************************************
*                               PROTOTYPES
***************************************************************************/

/* create/free array stream from existing array */
array_stream_t *MakeArrayStream(unsigned char *array, as_mode_t mode);
unsigned char *ReleaseArrayStream(array_stream_t *stream);

/* get/put character */
int ArrayStreamGetChar(array_stream_t *stream);
int ArrayStreamPutChar(const int c, array_stream_t *stream);

/* get/put single bit */
int ArrayStreamGetBit(array_stream_t *stream);
int ArrayStreamPutBit(const int c, array_stream_t *stream);

/* get/put number of bits */
int ArrayStreamGetBits(array_stream_t *stream, void *bits,
    const unsigned int count);
int ArrayStreamPutBits(array_stream_t *stream, void *bits,
    const unsigned int count);

/* number of bits read/written so far */
unsigned long ArrayStreamGetBitCount(array_stream_t *stream);

#endif /* _ARRAYSTREAM_H_ */
