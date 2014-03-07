IOSIG software is a toolkit developed by [Scalable Computing Software Lab](http://www.cs.iit.edu/~scs) at
Illinois Institute of Technology. In our research work, we designed and
developed several performance optimization techniques for parallel I/O systems.
Many of the techniques is application-aware that can utilize the I/O
characteristics information of the applications to improve the efficiency of
I/O system . We use IOSIG to collect and analyze applications I/O
characteristics, which works as follows: 

1. IOSIG trace collector records the trace of all the I/O operations of the
application.  
2. The IOSIG trace analyzer analyzes the trace files and recognizes the data
access patterns.
3. The analysis results can be displayed using the browser-based dashboard.

See [INSTALL.md](https://github.com/yinyanlong/iosig/blob/master/INSTALL.md) for how to install and use the software. For more examples on how to use IOSIG to profile parallel I/O benchmarks and applications, check the GitHub project: https://github.com/yinyanlong/profiling_parallel_io.

Contact:

- Yanlong Yin (yinyanlong@gmail.com)
- Xian-He Sun (sun@iit.edu)

