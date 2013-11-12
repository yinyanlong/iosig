MPIPATH		= /export/home/yyin/common/program_files/mpich2-1.4.1p1
INSTALL_DIR	= /export/home/yyin/install

C_COMPILER	= gcc
F_COMPILER	= gfortran

V = 0
AT_0 := @
AT_1 :=
AT = $(AT_$(V))
CC_0 = $(AT)echo "CC $<"; $(C_COMPILER)
CC_1 = $(C_COMPILER)
CC = $(CC_$(V))
FC_0 = $(AT)echo "FC $<"; $(F_COMPILER)
FC_1 = $(F_COMPILER)
FC = $(FC_$(V))

TRC_SRC    = $(wildcard *.c)
TRC_OBJS   = $(TRC_SRCS:.c=.o)

FC_SRCS    = $(wildcard *.f)
FC_OBJS    = $(FC_SRCS:.f=.o)


DFLAGS	:= $(DFLAGS) -DFORTRANDOUBLEUNDERSCORE
DFLAGS	:= $(DFLAGS) -D_LARGEFILE64_SOURCE
DFLAGS	:= $(DFLAGS) -D_FILE_OFFSET_BITS=64
DFLAGS	:= $(DFLAGS) -DHAVE_ROMIOCONF_H

COMMON_CFLAGS	:= $(DFLAGS) -I${INSTALL_DIR}/include
COMMON_CFLAGS	:= $(COMMON_CFLAGS) -I${INSTALL_DIR}/include
COMMON_CFLAGS	:= $(COMMON_CFLAGS) -lpthread

MPIIO_CFLAGS	:= $(MPIIO_CFLAGS) -I${MPIPATH}/src/mpid/ch3/include                 # mpidpost.h
MPIIO_CFLAGS	:= $(MPIIO_CFLAGS) -I${MPIPATH}/src/mpid/common/datatype             # mpid_datatype.h
MPIIO_CFLAGS	:= $(MPIIO_CFLAGS) -I${MPIPATH}/src/mpid/ch3/channels/sock/include   # mpidi_ch3_post.h
MPIIO_CFLAGS	:= $(MPIIO_CFLAGS) -I${MPIPATH}/src/mpid/common/sock                 # for older version of mpich2
MPIIO_CFLAGS	:= $(MPIIO_CFLAGS) -I${MPIPATH}/src/mpid/common/sock/poll            # for older version of mpich2
MPIIO_CFLAGS	:= $(MPIIO_CFLAGS) -I${MPIPATH}/src/mpi/romio/adio/include           # adio.h
MPIIO_CFLAGS	:= $(MPIIO_CFLAGS) -I${MPIPATH}/src/include                          # mpiimpl.h
MPIIO_CFLAGS	:= $(MPIIO_CFLAGS) -I${MPIPATH}/src/mpl/include                      # mpl.h
MPIIO_CFLAGS	:= $(MPIIO_CFLAGS) -I${MPIPATH}/src/util/wrappers                    # mpiu_process_wrappers.h
MPIIO_CFLAGS	:= $(MPIIO_CFLAGS) -I${MPIPATH}/src/mpi/romio/mpi-io                 # mpioimpl.h

IOSIG_WRAP_FLAGS = -Wl,-wrap,fopen,-wrap,fopen64,-wrap,fclose,-wrap,fread,-wrap,fwrite,-wrap,fseek,-wrap,open,-wrap,close,-wrap,read,-wrap,write,-wrap,lseek,-wrap,lseek64,-wrap,open64

FCFLAGS	:= -DFORTRANDOUBLEUNDERSCORE

# put target objects to different groups so each group can have
# special compiler options
COMMON_OBJS := iosig_trace.o iosig_init.o
EXE_OBJS    := exe_trace.o
POSIX_OBJS  := posix_trace.o
MPIIO_OBJS  := mpiio_trace.o mpiio_sp_trace.o
TRC_OBJS    := $(COMMON_OBJS) $(EXE_OBJS) $(POSIX_OBJS) $(MPIIO_OBJS)

$(COMMON_OBJS): CFLAGS := $(COMMON_CFLAGS) $(MPIIO_CFLAGS)
$(MPIIO_OBJS): CFLAGS := $(COMMON_CFLAGS) $(MPIIO_CFLAGS)
$(EXE_OBJS): CFLAGS := $(COMMON_CFLAGS) $(IOSIG_WRAP_FLAGS)
$(POSIX_OBJS): CFLAGS := $(COMMON_CFLAGS) $(IOSIG_WRAP_FLAGS)

all:	iosig

$(TRC_OBJS): %.o: %.c
	$(CC) -c $(CFLAGS) -g $< -o $@

.f.o:
	$(FC) -c $(FCFLAGS) $(DFLAGS) $<

iosig: $(TRC_OBJS) $(FC_OBJS)
	@echo "Creating ligiosig.a"; ar rlc libiosig.a $(TRC_OBJS) $(FC_OBJS)
	@echo "Completed. Next, make install."

install: iosig
	cp libiosig.a ${INSTALL_DIR}/lib

clean::
	rm -f *.o
	rm -rf libiosig.a

