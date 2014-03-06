# IOSIG - INSTALLATION GUIDE

This file is written using Markdown. It can be viewed in a
Markdown viewer or converted to a single-page HTML document.

## Download IOSIG and setup the environment variables

    $ tar zxvf iosig-1.4.tar.gz
    $ export IOSIG_HOME=./iosig-1.4
    $ mkdir ~/.iosig
    $ mkdir ~/.iosig/data
    $ export IOSIG_DATA=${HOME}/.iosig/data

The variable `IOSIG_HOME` is used to locate the IOSIG tools, including the
library and the analyzer. The variable `IOSIG_DATA` is used to store the trace
file. 

## Compiling the trace library.
  
    $ cd ${IOSIG_HOME}/src/collect
    $ vim Makefile
   
Edit the first two lines of `Makefile`. Set MPIPATH to the source code of
MPICH2, which has been configured (`$ ./configure`). Set `INSTALL_DIR` to where
you want to install the IOSIG library.
   
    $ make
    $ make install
   
## Using examples

    $ cd ${IOSIG_HOME}/src/examples

There are several separated directories, each of which is an example of using 
IOSIG. 

- `execution_trace`
- `IOR`
- `posix_io_c`
- `mpiio_c`
- `mpiio_fortran`
   
The directory names tell what the examples are. In each example, there are two
Makefiles:

- `Makefile`
- `Makefile.iosig`

The first is the default Makefile to compile the example program. The second 
also enables IOSIG during building the program. Using `Makefile.iosig` to 
build and run the program will generate the IO traces.

By comparing `Makefile` and `Makefile.iosig`, you get the idea of how to link
IOSIG library with your program.

## Using IOSIG tracing library

To use IOSIG, just add the following compilers flags to your gcc or mpicc
command line. If you have difficulty on how to build your program, please check
the examples following the instructions of last section.

    IOSIG_CFLAGS   = -w -finstrument-functions
    IOSIG_LDFLAGS  = -Wl,-wrap,fopen,-wrap,fopen64,-wrap,fclose,-wrap,fread,-wrap,fwrite,-wrap,fseek,-wrap,open,-wrap,close,-wrap,read,-wrap,write,-wrap,lseek,-wrap,lseek64,-wrap,open64 -L${IOSIG_HOME}/src/collect -liosig 

`IOSIG_CFLAGS` is to be added in the compiling stage. `IOSIG_LDFLAGS` is to be
added in the link stage. Please check the examples for how to do this.

## Using IOSIG analyzer

In this current version, you need to run the analysis manually for each trace
directory in `$IOSIG_DATA`. For example:

    $ cd ${IOSIG_DATA}
    $ cd username_1386547880_userApp
    $ source ${IOSIG_HOME}/src/analysis/plot.sh

The results will be generated in a directory called `result_output` in the
directory `username_1386547880_userApp`. Do not modify or delete the content of
the directory since the web dashboard needs to read and display the results.
Next section shows how to use the web dashboard.

## Using the web dashboard

You need to download and install the Flask python package, which is available
at: <http://flask.pocoo.org/>.

After that, simply follow the following steps:

    $ cd ${IOSIG_HOME}/src/webapps
    $ python webapp.py
   
Then, open your browser and access <http://hostname:5000/>. Here, `hostname` is
the machine where you run `python webapp.py`.

NOTE: in this version, the web dashboard can only display the trace that has
been analyzed. Future version may make all analysis procedures automatic.



    


