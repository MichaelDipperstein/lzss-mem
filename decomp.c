/***************************************************************************
*                  Sample Program Using LZSS Array Library
*
*   Purpose : Demonstrate usage of LZSS library to perform in-memory
*             decompression of a file.
*   Author  : Michael Dipperstein
*   Date    : November 28, 2004
*
****************************************************************************
*   UPDATES
*
*   $Id: decomp.c,v 1.1.1.1 2004/12/06 14:26:18 michael Exp $
*   $Log: decomp.c,v $
*   Revision 1.1.1.1  2004/12/06 14:26:18  michael
*   initial release
*
*
****************************************************************************
*
* DECOMP: Super simple sample demonstraiting use of LZSS decompression
*         routine.
* Copyright (C) 2004 by Michael Dipperstein (mdipper@cs.ucsb.edu)
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
#include "lzss.h"

/***************************************************************************
*                                CONSTANTS
***************************************************************************/
#define DATA_MAX    0xFFFF      /* maximum size of encoded/decode data */

/***************************************************************************
*                            GLOBAL VARIABLES
***************************************************************************/
unsigned char encoded[DATA_MAX];
unsigned char decoded[DATA_MAX];

/***************************************************************************
*                                FUNCTIONS
***************************************************************************/

/****************************************************************************
*   Function   : main
*   Description: This is the main function for this program it calls the
*                LZSS decoding routine using the command line arguments as
*                the file to decode and the decoded target.
*   Parameters : argc - number of parameters
*                argv - parameter list
*   Effects    : Encodes specified file
*   Returned   : 0
****************************************************************************/
int main(int argc, char *argv[])
{
    FILE *fp;
    int c;
    unsigned int dataLen, i;

    if (argc != 3)
    {
        printf("Invalid arguments\n");
        printf("Correct format:\n");
        printf("%s <compressed file> <decompressed file>\n", argv[0]);
    }

    /* open encoded file */
    if ((fp = fopen(argv[1], "rb")) == NULL)
    {
        perror(argv[1]);
        return (EXIT_FAILURE);
    }

    /* copy encoded file to array */
    dataLen = 0;
    while ((c = fgetc(fp)) != EOF)
    {
        encoded[dataLen++] = (unsigned char)c;
    }

    fclose(fp);

    /* decode array */
    dataLen = DecodeLZSSFromArray(encoded, decoded, dataLen);

    if ((fp = fopen(argv[2], "wb")) == NULL)
    {
        perror(argv[2]);
        return (EXIT_FAILURE);
    }

    /* copy decoded array to file */
    i = 0;
    while (i < dataLen)
    {
        fputc(decoded[i++], fp);
    }
    fclose(fp);

    return (EXIT_SUCCESS);
}
