#/*****************************************************************************\
#*                                                                             *
#*       Copyright (c) 2003, The Regents of the University of California       *
#*     See the file COPYRIGHT for a complete copyright notice and license.     *
#*                                                                             *
#*******************************************************************************
#*
#* CVS info:
#*   $RCSfile: Makefile,v $
#*   $Revision: 1.1.1.1 $
#*   $Date: 2007/10/15 23:36:54 $
#*   $Author: rklundt $
#*
#* Purpose:
#*       Make IOR executable.
#*
#*       gmake posix      -- IOR with only POSIX interfaces
#*       gmake mpiio      -- IOR with only POSIX and MPIIO interfaces
#*       gmake hdf5       -- IOR with POSIX, MPIIO, and HDF5 interfaces
#*       gmake ncmpi      -- IOR with POSIX, MPIIO, and NCMPI interfaces
#*       gmake all        -- IOR with POSIX, MPIIO, HDF5, and NCMPI interfaces
#*       gmake clean      -- remove executable and object files
#*
#\*****************************************************************************/

include Makefile.config

# Requires GNU Make
OS=$(shell uname)

SRCS = IOR.c utilities.c parse_options.c
OBJS = $(SRCS:.c=.o)

mpiio: $(OBJS) aiori-POSIX.o aiori-MPIIO.o aiori-noHDF5.o aiori-noNCMPI.o
	$(CC) -o IOR $(OBJS) \
		aiori-POSIX.o aiori-MPIIO.o aiori-noHDF5.o aiori-noNCMPI.o \
		$(LDFLAGS) 

clean:
	-rm -f *.o IOR exe*.out mpiio*.out posix*.out

aiori-MPIIO.o: aiori-MPIIO.c
	$(CC) -c aiori-MPIIO.c

.c.o:
	$(CC) $(CCFLAGS) -c $<
