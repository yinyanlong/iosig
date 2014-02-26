# INSTALLATION

## Index

1. Download and unzip the iosig-1.4.tar.gz and setup environment variables.
2. Compiling the IOSIG trace library.
3. Using examples to get familiar with IOSIG.
4. Using IOSIG.

## Download IOSIG and setup the environment variables

        $ tar zxvf iosig-1.4.tar.gz
        $ export $IOSIG_HOME=./iosig-1.4
        $ mkdir ~/.iosig
        $ mkdir ~/.iosig/data
        $ export $IOSIG_DATA=${HOME}/.iosig/data

The variable `IOSIG_HOME` is used to locate the IOSIG tools, including the
library and the analyzer. The variable `IOSIG_DATA` is used to store the trace file. 

## Compiling the trace library.
  
        $ cd ${IOSIG_HOME}/src/collect
        $ vim Makefile
   
Edit the first two lines of `Makefile'. Set MPIPATH to the source code of 
MPICH2, which has been configured ($ ./configure). Set `INSTALL_DIR' to where 
you want to install the IOSIG library.
   
        $ make
        $ make install
   
## Using examples

        $ cd ${IOSIG_HOME}/src/examples

There are several seperated directories, each of which is an example of using 
IOSIG. 

- `execution_trace`
- `posix_io_c`
- `mpiio_c`
- `mpiio_fortran`
- `IOR`
   
The directory names tell what the examples are. In each example, there are two Makefiles:

- `Makefile`
- `Makefile.iosig`

The first is the default Makefile to compile the example program. The second 
also enables IOSIG during building the program. Using `Makefile.iosig` to 
build and run the program will generate the IO traces.

By comparing `Makefile` and `Makefile.iosig`, you get the idea of how to link 
IOSIG library with your program.

## Using IOSIG Tracing library

TO use IOSIG, just add the following compilers flags to your gcc or mpicc command 
line.

        IOSIG_CFLAGS   = -w -finstrument-functions
        IOSIG_LDFLAGS  = -L${IOSIG_HOME}/src/collect -liosig -Wl,-wrap,fopen,-wrap,fopen64,-wrap,fclose,-wrap,fread,-wrap,fwrite,-wrap,fseek,-wrap,open,-wrap,close,-wrap,read,-wrap,write,-wrap,lseek,-wrap,lseek64,-wrap,open64

`IOSIG_CFLAGS` is to be added in the compiling stage. `IOSIG_LDFLAGS` is to be added in the link stage. Please check the examples for how to do this.


