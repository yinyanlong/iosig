IOSIG_LIB_PATH = ../../collect2
IOSIG_CFLAGS += -w -finstrument-functions -D_FILE_OFFSET_BITS=64
IOSIG_LDFLAGS += -Wl,-wrap,fopen,-wrap,fopen64,-wrap,fclose,-wrap,fread,-wrap,fwrite,-wrap,fseek,-wrap,open,-wrap,close,-wrap,read,-wrap,write,-wrap,lseek,-wrap,lseek64,-wrap,open64

CFLAGS    += ${IOSIG_CFLAGS}
LDFLAGS   += -L${IOSIG_LIB_PATH} -liosig ${IOSIG_LDFLAGS}

mpiioExample:	mpiioExample.o
	mpif77 mpiioExample.o -o mpiioExample $(LDFLAGS)
	
mpiioExample.o:
	mpif77 -c ${CFLAGS} ./mpiioExample.f -o ./mpiioExample.o

clean:
	rm -f *.o trace*.out mpiioExample exe*.out

