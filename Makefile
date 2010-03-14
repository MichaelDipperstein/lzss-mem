############################################################################
# Makefile for array based lzss encode/decode library and sample programs
#
#   $Id: Makefile,v 1.1.1.1 2004/12/06 14:26:18 michael Exp $
#   $Log: Makefile,v $
#   Revision 1.1.1.1  2004/12/06 14:26:18  michael
#   initial release
#
#
############################################################################
CC = gcc
LD = gcc
CFLAGS = -I. -O3 -Wall -ansi -c
LDFLAGS = -O3 -o

# libraries
LIBS = -L. -llzss

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

# define the method to be used for searching for matches (choose one)
# brute force
# FMOBJ = brute.o
# linked list
# FMOBJ = list.o
# hash table
FMOBJ = hash.o

LZOBJS = $(FMOBJ) lzencode.o lzdecode.o lzvars.o

all:		comp$(EXE) decomp$(EXE) liblzss.a

liblzss.a:	$(LZOBJS) arraystream.o
		ar crv liblzss.a $(LZOBJS) arraystream.o
		ranlib liblzss.a

lzencode.o:	lzencode.c lzlocal.h arraystream.h
		$(CC) $(CFLAGS) $<

lzdecode.o:	lzdecode.c lzlocal.h arraystream.h
		$(CC) $(CFLAGS) $<

brute.o:	brute.c lzlocal.h
		$(CC) $(CFLAGS) $<

list.o:	        list.c lzlocal.h
		$(CC) $(CFLAGS) $<

hash.o:	        hash.c lzlocal.h
		$(CC) $(CFLAGS) $<

lzvars.o:	lzvars.c lzlocal.h
		$(CC) $(CFLAGS) $<

arraystream.o:	arraystream.c arraystream.h
		$(CC) $(CFLAGS) $<

comp$(EXE):	comp.o $(FMOBJ) lzencode.o lzvars.o arraystream.o
		$(LD) $^ $(LDFLAGS) $@

comp.o:		comp.c lzss.h
		$(CC) $(CFLAGS) $<

decomp$(EXE):	decomp.o lzdecode.o lzvars.o arraystream.o 
		$(LD) $^ $(LDFLAGS) $@

decomp.o:	decomp.c lzss.h
		$(CC) $(CFLAGS) $<

clean:
		$(DEL) *.o
		$(DEL) *.a
		$(DEL) comp$(EXE)
		$(DEL) decomp$(EXE)
