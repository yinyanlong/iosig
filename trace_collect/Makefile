MPIPATH		= /your/path/to/mpich2-configured-source-code/ 
INSTALL_DIR	= /your/path/to/mpich2-install/ 

CXX		= gcc
F77     = gfortran

TRC_SRCS	= $(wildcard *.c)
TRC_OBJS	= $(TRC_SRCS:.c=.o)

F77_SRCS    = $(wildcard *.f)
F77_OBJS    = $(F77_SRCS:.f=.o)

DFLAGS		:= $(DFLAGS) -DFORTRANDOUBLEUNDERSCORE
DFLAGS		:= $(DFLAGS) -D_LARGEFILE64_SOURCE
DFLAGS		:= $(DFLAGS) -D_FILE_OFFSET_BITS=64
DFLAGS		:= $(DFLAGS) -DHAVE_ROMIOCONF_H

CXXFLAGS	:= $(CXXFLAGS) -I${INSTALL_DIR}/include
CXXFLAGS	:= $(CXXFLAGS) -I${MPIPATH}/src/binding/f77
CXXFLAGS	:= $(CXXFLAGS) -I${MPIPATH}/src/mpid/ch3/include
CXXFLAGS	:= $(CXXFLAGS) -I${MPIPATH}/src/mpid/common/datatype
CXXFLAGS	:= $(CXXFLAGS) -I${MPIPATH}/src/mpid/ch3/channels/sock/include
CXXFLAGS	:= $(CXXFLAGS) -I${MPIPATH}/src/mpid/common/sock
CXXFLAGS	:= $(CXXFLAGS) -I${MPIPATH}/src/mpid/common/sock/poll
CXXFLAGS	:= $(CXXFLAGS) -O -I. -I${MPIPATH}/src/mpi/romio/mpi-io
CXXFLAGS	:= $(CXXFLAGS) -I${MPIPATH}/src/mpi/romio/adio/include
CXXFLAGS	:= $(CXXFLAGS) -I${MPIPATH}/src/include
CXXFLAGS	:= $(CXXFLAGS) -I${MPIPATH}/src/mpl/include
CXXFLAGS	:= $(CXXFLAGS) -I${MPIPATH}/src/util/wrappers

F77FLAGS	:=

.c.o:
	$(CXX) -c $(CXXFLAGS) $(DFLAGS) $<
	
.f.o:
	$(F77) -c $(F77FLAGS) $(DFLAGS) $<

all:	trace

trace: $(TRC_OBJS) $(F77_OBJS)
	ar rlc libiosig.a $(TRC_OBJS) $(F77_OBJS)
	@echo "... compile comleted."

install: trace
	cp libiosig.a ${INSTALL_DIR}/lib

clean::
	rm -f *.o
	rm -rf libiosig.a

