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
*   $Id: sample.c,v 1.1.1.1 2004/12/06 13:41:26 michael Exp $
*   $Log: sample.c,v $
*   Revision 1.1.1.1  2004/12/06 13:41:26  michael
*   initial release
*
*
****************************************************************************
*
* Sample: An array stream library sample usage program
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
*   Parameters : argc - the number command line arguements (not used)
*                argv - array of command line arguements (not used)
*   Effects    : Writes array stream , reads back results, printing them
*                to stdout.
*   Returned   : EXIT_SUCCESS
***************************************************************************/
int main(int argc, char *argv[])
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
