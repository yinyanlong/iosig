MPIPATH         = /your/path/to/mpich2-configured-source-code/
INSTALL_DIR     = /your/path/to/mpich2-install/

C_COMPILER	= gcc
F_COMPILER	= gfortran

V = 0
AT_0 := @
AT_1 :=
AT = $(AT_$(V))
CC_0 = $(AT)echo "CC $<..."; $(C_COMPILER)
CC_1 = $(C_COMPILER)
CC = $(CC_$(V))
FC_0 = $(AT)echo "FC $<..."; $(F_COMPILER)
FC_1 = $(F_COMPILER)
FC = $(FC_$(V))

TRC_SRCS	= $(wildcard *.c)
TRC_OBJS	= $(TRC_SRCS:.c=.o)

FC_SRCS    = $(wildcard *.f)
FC_OBJS    = $(FC_SRCS:.f=.o)

DFLAGS	:= $(DFLAGS) -DFORTRANDOUBLEUNDERSCORE
DFLAGS	:= $(DFLAGS) -D_LARGEFILE64_SOURCE
DFLAGS	:= $(DFLAGS) -D_FILE_OFFSET_BITS=64
DFLAGS	:= $(DFLAGS) -DHAVE_ROMIOCONF_H

CFLAGS	:= $(CFLAGS) -I${INSTALL_DIR}/include
CFLAGS	:= $(CFLAGS) -I${MPIPATH}/src/mpid/ch3/include                 # mpidpost.h
CFLAGS	:= $(CFLAGS) -I${MPIPATH}/src/mpid/common/datatype             # mpid_datatype.h
CFLAGS	:= $(CFLAGS) -I${MPIPATH}/src/mpid/ch3/channels/sock/include   # mpidi_ch3_post.h
CFLAGS	:= $(CFLAGS) -I${MPIPATH}/src/mpid/common/sock                 # for older version of mpich2
CFLAGS	:= $(CFLAGS) -I${MPIPATH}/src/mpid/common/sock/poll            # for older version of mpich2
CFLAGS	:= $(CFLAGS) -I${MPIPATH}/src/mpi/romio/adio/include           # adio.h
CFLAGS	:= $(CFLAGS) -I${MPIPATH}/src/include                          # mpiimpl.h
CFLAGS	:= $(CFLAGS) -I${MPIPATH}/src/mpl/include                      # mpl.h
CFLAGS	:= $(CFLAGS) -I${MPIPATH}/src/util/wrappers                    # mpiu_process_wrappers.h
CFLAGS	:= $(CFLAGS) -I${MPIPATH}/src/mpi/romio/mpi-io                 # mpioimpl.h

FCFLAGS	:=

.c.o:
	$(CC) -c $(CFLAGS) $(DFLAGS) $<
	
.f.o:
	$(FC) -c $(FCFLAGS) $(DFLAGS) $<

all:	trace

trace: $(TRC_OBJS) $(FC_OBJS)
	@echo "Creating ligiosig.a"; ar rlc libiosig.a $(TRC_OBJS) $(FC_OBJS)
	@echo "Comleted. Next, make install."

install: trace
	cp libiosig.a ${INSTALL_DIR}/lib

clean::
	rm -f *.o
	rm -rf libiosig.a

