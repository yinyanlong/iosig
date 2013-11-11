MPI_HOME     = /export/home/yyin/install
CC           = mpicc

IOSIG_CFLAGS    = -w -finstrument-functions -D_FILE_OFFSET_BITS=64
#IOSIG_LDFLAGS   = -L../../collect2 -liosig
IOSIG_LDFLAGS   = -L../../collect2 -liosig
#IOSIG_LDFLAGS   = -L../../../old/trace_collect/ -liosig
IOSIG_LDFLAGS  += -Wl,-wrap,fopen,-wrap,fopen64,-wrap,fclose,-wrap,fread,-wrap,fwrite,-wrap,fseek,-wrap,open,-wrap,close,-wrap,read,-wrap,write,-wrap,lseek,-wrap,lseek64,-wrap,open64

CFLAGS      += ${IOSIG_CFLAGS}
CFLAGS      += -I$(MPI_HOME)/include 
CFLAGS      += -rdynamic
LDFLAGS     := ${IOSIG_LDFLAGS}
LDFLAGS     := ${LDFLAGS} -lm
LDFLAGS     := ${LDFLAGS} -rdynamic
LDFLAGS     := ${LDFLAGS} -lpthread

EXECS        = example

all: $(EXECS)

example: example.o
	$(CC) -o $@ $^ $(LDFLAGS) 

clean:
	rm -f *.o

.c.o:
	$(CC) $(CFLAGS) -c $<

