############################################################################
# Makefile for arraystream library and sample
#
#   $Id: Makefile,v 1.3 2009/01/04 06:16:52 michael Exp $
#   $Log: Makefile,v $
#   Revision 1.3  2009/01/04 06:16:52  michael
#   -O3 put back.  Segfault fixed in 4.3.2.
#
#   Revision 1.2  2007/08/27 13:01:00  michael
#   Lower optimization level.
#   -O2 in GCC 4.1.2 results in a seg fault.
#
#   Revision 1.1.1.1  2004/12/06 13:41:26  michael
#   initial release
#
#
############################################################################

CC = gcc
LD = gcc
CFLAGS = -g -O3 -Wall -Wextra -pedantic -ansi -c
LDFLAGS = -g -O3 -o

# Treat NT and non-NT windows the same
ifeq ($(OS),Windows_NT)
	OS = Windows
endif

ifeq ($(OS),Windows)
	EXE = .exe
	DEL = del
else	#assume Linux/Unix
	EXE =
	DEL = rm
endif

all:		sample$(EXE)

sample$(EXE):	sample.o arraystream.o
		$(LD) $^ $(LDFLAGS) $@

sample.o:	sample.c arraystream.h
		$(CC) $(CFLAGS) $<

arraystream.o:	arraystream.c arraystream.h
		$(CC) $(CFLAGS) $<

clean:
		$(DEL) *.o
		$(DEL) sample$(EXE)
