MPIPATH		= /your/path/to/mpich2-1.4/
INSTALL_DIR	= /your/path/to/mpich2-install/lib/

CXX		= gcc

TRC_SRCS	= $(wildcard *.c)
TRC_OBJS	= $(TRC_SRCS:.c=.o)

DFLAGS		:= $(DFLAGS) -DFORTRANDOUBLEUNDERSCORE
DFLAGS		:= $(DFLAGS) -D_LARGEFILE64_SOURCE
DFLAGS		:= $(DFLAGS) -D_FILE_OFFSET_BITS=64
DFLAGS		:= $(DFLAGS) -DHAVE_ROMIOCONF_H

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

.c.o:
	$(CXX) -c $(CXXFLAGS) $(DFLAGS) $<

all:	trace

trace: $(TRC_OBJS)
	ar rlc libiosig.a $(TRC_OBJS)
	@echo "... compile comleted."

install: trace
	cp libiosig.a ${INSTALL_DIR}

clean::
	rm -f *.o
	rm -rf libpushio.a

