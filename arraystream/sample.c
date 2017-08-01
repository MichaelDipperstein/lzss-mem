/***************************************************************************
*                     Array Stream Library Usage Sample
*
*   File    : sample.c
*   Purpose : Demonstrates usage of array stream library.
*   Author  : Michael Dipperstein
*   Date    : November 28, 2004
*
****************************************************************************
*   HISTORY
*
*   $Id: sample.c,v 1.2 2007/08/27 13:01:21 michael Exp $
*   $Log: sample.c,v $
*   Revision 1.2  2007/08/27 13:01:21  michael
*   Changes required for LGPL v3.
*
*   Revision 1.1.1.1  2004/12/06 13:41:26  michael
*   initial release
*
*
****************************************************************************
*
* Sample: An array stream library sample usage program
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
#include <stdio.h>
#include <stdlib.h>
#include "arraystream.h"

/***************************************************************************
*                                CONSTANTS
***************************************************************************/
#define NUM_CALLS       5

/***************************************************************************
*                                FUNCTIONS
***************************************************************************/

/***************************************************************************
*   Function   : main
*   Description: This function demonstrates the usage of each of the bit
*                bit file functions.
*   Parameters : None
*   Effects    : Writes array stream , reads back results, printing them
*                to stdout.
*   Returned   : EXIT_SUCCESS
***************************************************************************/
int main()
{
    unsigned char array[(5 * NUM_CALLS) + (NUM_CALLS / 8) + 1];
    array_stream_t *as;
    int i, value;

    /* test writes */
    as = MakeArrayStream(array, AS_WRITE);

    /* write chars */
    value = (int)'A';
    for (i = 0; i < NUM_CALLS; i++)
    {
        printf("writing char %c\n", value);
        if(ArrayStreamPutChar(value, as) == EOF)
        {
            perror("writing char");
            ReleaseArrayStream(as);
            return (EXIT_FAILURE);
        }

        value++;
    }

    printf("wrote %ld bits\n", ArrayStreamGetBitCount(as));

    /* write single bits */
    value = 0;
    for (i = 0; i < NUM_CALLS; i++)
    {
        printf("writing bit %d\n", value);
        if(ArrayStreamPutBit(value, as) == EOF)
        {
            perror("writing bit");
            ReleaseArrayStream(as);
            return (EXIT_FAILURE);
        }

        value = !value;
    }

    printf("wrote %ld bits\n", ArrayStreamGetBitCount(as));

    /* write ints as bits */
    value = 0x11111111;
    for (i = 0; i < NUM_CALLS; i++)
    {
        printf("writing bits %0X\n", value);
        if(ArrayStreamPutBits(as, &value, (8 * sizeof(int))) == EOF)
        {
            perror("writing bits");
            ReleaseArrayStream(as);
            return (EXIT_FAILURE);
        }

        value += 0x11111111;
    }

    printf("wrote %ld bits\n", ArrayStreamGetBitCount(as));

    ReleaseArrayStream(as);

    /* test reads */
    as = MakeArrayStream(array, AS_READ);

    /* read chars */
    for (i = 0; i < NUM_CALLS; i++)
    {
        value = ArrayStreamGetChar(as);
        if(value == EOF)
        {
            perror("reading char");
            ReleaseArrayStream(as);
            return (EXIT_FAILURE);
        }
        else
        {
            printf("read char %c\n", value);
        }
    }

    printf("read %ld bits\n", ArrayStreamGetBitCount(as));

    /* read single bits */
    for (i = 0; i < NUM_CALLS; i++)
    {
        value = ArrayStreamGetBit(as);
        if(value == EOF)
        {
            perror("reading bit");
            ReleaseArrayStream(as);
            return (EXIT_FAILURE);
        }
        else
        {
            printf("read bit %d\n", value);
        }
    }

    printf("read %ld bits\n", ArrayStreamGetBitCount(as));

    /* read ints as bits */
    for (i = 0; i < NUM_CALLS; i++)
    {
        if(ArrayStreamGetBits(as, &value, (8 * sizeof(int))) == EOF)
        {
            perror("reading bits");
            ReleaseArrayStream(as);
            return (EXIT_FAILURE);
        }
        else
        {
            printf("read bits %0X\n", value);
        }
    }

    printf("read %ld bits\n", ArrayStreamGetBitCount(as));
    ReleaseArrayStream(as);

    return(EXIT_SUCCESS);
}
