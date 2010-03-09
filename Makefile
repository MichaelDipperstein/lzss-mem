############################################################################
# Makefile for arraystream library and sample
#
#   $Id: Makefile,v 1.1.1.1 2004/12/06 13:41:26 michael Exp $
#   $Log: Makefile,v $
#   Revision 1.1.1.1  2004/12/06 13:41:26  michael
#   initial release
#
#
############################################################################

CC = gcc
LD = gcc
CFLAGS = -O2 -Wall -ansi -c
LDFLAGS = -O2 -o

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
