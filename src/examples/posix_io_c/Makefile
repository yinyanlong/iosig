IOSIG_LIB_PATH = ../../collect
IOSIG_LDFLAGS = -Wl,-wrap,fopen,-wrap,fopen64,-wrap,fclose,-wrap,fread,-wrap,fwrite,-wrap,fseek,-wrap,open,-wrap,close,-wrap,read,-wrap,write,-wrap,lseek,-wrap,lseek64,-wrap,open64

CFLAGS    += -w -finstrument-functions -D_FILE_OFFSET_BITS=64
LDFLAGS   += $(IOSIG_LDFLAGS)
LDFLAGS   += -L${IOSIG_LIB_PATH} -liosig  #-D_FILE_OFFSET_BITS=64

simple_posix: simple_posix.o 
	gcc simple_posix.o -o simple_posix ${LDFLAGS}

simple_posix.o: simple_posix.c
	gcc ${CFLAGS} -g -c -o simple_posix.o simple_posix.c

clean::
	rm *.o simple_posix myfile exe*.out trace*.out exe*.out.new posix*.out


# CP_WRAPPERS="-Wl,-u,MPI_Init,-u,MPI_Wtime,-wrap,write,-wrap,open,-wrap,creat,-wrap,creat64,-wrap,open64,-wrap,close,-wrap,read,-wrap,lseek,-wrap,lseek64,-wrap,pread,-wrap,pwrite,-wrap,readv,-wrap,writev,-wrap,__xstat,-wrap,__lxstat,-wrap,__fxstat,-wrap,__xstat64,-wrap,__lxstat64,-wrap,__fxstat64,-wrap,mmap,-wrap,mmap64,-wrap,fopen,-wrap,fclose,-wrap,fread,-wrap,fwrite,-wrap,fseek,-wrap,fopen64,-wrap,pread64,-wrap,pwrite64,-wrap,fsync,-wrap,fdatasync,-wrap,ncmpi_create,-wrap,ncmpi_open,-wrap,ncmpi_close,-wrap,H5Fcreate,-wrap,H5Fopen,-wrap,H5Fclose"
