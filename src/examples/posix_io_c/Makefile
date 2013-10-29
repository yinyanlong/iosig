IOSIG_LIB_PATH = ../../collect
CFLAGS    += -finstrument-functions
LDFLAGS   += -Wl,-wrap,fopen,-wrap,fclose,-wrap,fread,-wrap,fwrite,-wrap,fseek,-wrap,open,-wrap,close,-wrap,read,-wrap,write,-wrap,lseek
LDFLAGS   += -L${IOSIG_LIB_PATH} -liosig

simple_posix: simple_posix.o 
	gcc simple_posix.o -o simple_posix ${LDFLAGS}


simple_posix.o: simple_posix.c
	gcc ${CFLAGS} -g -c -o simple_posix.o simple_posix.c

clean::
	rm *.o simple_posix myfile exe*.out trace*.out exe*.out.new posix*.out

