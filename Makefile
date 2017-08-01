############################################################################
# Makefile for array based lzss encode/decode library and sample programs
#
############################################################################
CC = gcc
LD = gcc
CFLAGS = -I. -O3 -Wall -Wextra -ansi -pedantic -c
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
	DEL = rm -f
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

liblzss.a:	$(LZOBJS) arraystream/arraystream.o
		ar crv liblzss.a $(LZOBJS) arraystream/arraystream.o
		ranlib liblzss.a

lzencode.o:	lzencode.c lzlocal.h arraystream/arraystream.h
		$(CC) $(CFLAGS) $<

lzdecode.o:	lzdecode.c lzlocal.h arraystream/arraystream.h
		$(CC) $(CFLAGS) $<

brute.o:	brute.c lzlocal.h
		$(CC) $(CFLAGS) $<

list.o:	        list.c lzlocal.h
		$(CC) $(CFLAGS) $<

hash.o:	        hash.c lzlocal.h
		$(CC) $(CFLAGS) $<

lzvars.o:	lzvars.c lzlocal.h
		$(CC) $(CFLAGS) $<

arraystream/arraystream.o:
		cd arraystream && $(MAKE) arraystream.o

comp$(EXE):	comp.o $(FMOBJ) lzencode.o lzvars.o arraystream/arraystream.o
		$(LD) $^ $(LDFLAGS) $@

comp.o:		comp.c lzss.h
		$(CC) $(CFLAGS) $<

decomp$(EXE):	decomp.o lzdecode.o lzvars.o arraystream/arraystream.o 
		$(LD) $^ $(LDFLAGS) $@

decomp.o:	decomp.c lzss.h
		$(CC) $(CFLAGS) $<

clean:
		$(DEL) *.o
		$(DEL) *.a
		$(DEL) comp$(EXE)
		$(DEL) decomp$(EXE)
		$(DEL) arraystream/*.o
