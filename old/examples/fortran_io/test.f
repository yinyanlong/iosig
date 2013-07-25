      program main
      include 'mpif.h'
      
      integer rank, ierr, status(MPI_STATUS_SIZE), request
      integer fh
      character buf(100)

      call MPI_INIT(ierr)
      call MPI_COMM_RANK(MPI_COMM_WORLD, rank, ierr)

      call MPI_FILE_OPEN(MPI_COMM_WORLD, './testfile',
     & MPI_MODE_RDWR, MPI_INFO_NULL, fh, ierr)
     
      call MPI_FILE_WRITE(fh, 'aaaaaaaaaaaaaaa', 10, MPI_CHARACTER,
     & status, ierr)
      call MPI_FILE_WRITE_ALL(fh, 'bbbbbbbbbbb', 7, MPI_CHARACTER,
     & status, ierr)
      call MPI_FILE_WRITE_AT(fh, 2, 'ccccccccc', 5, MPI_CHARACTER,
     & status, ierr)
      call MPI_FILE_WRITE_AT_ALL(fh, 8, 'dddddddd', 6, MPI_CHARACTER,
     & status, ierr)
c      call MPI_FILE_WRITE_ORDERED(fh, 'eeeeeeeee', 4, MPI_CHARACTER,
c     & status, ierr)
c      call MPI_FILE_WRITE_SHARED(fh, 'ffffffff', 4, MPI_CHARACTER,
c     & status, ierr)
      call MPI_FILE_IWRITE(fh, 'ggggggg', 5, MPI_CHARACTER,
     & request, ierr)
      call MPI_FILE_IWRITE_AT(fh, 6, 'hhhhhhh', 3, MPI_CHARACTER,
     & request, ierr)
c      call MPI_FILE_IWRITE_SHARED(fh, 'iiiiiii', 3, MPI_CHARACTER,
c     & request, ierr)
     
      call MPI_FILE_READ(fh, buf, 3, MPI_CHARACTER,
     & status, ierr)
      call MPI_FILE_READ_ALL(fh, buf, 3, MPI_CHARACTER,
     & status, ierr)
      call MPI_FILE_READ_AT(fh, 4, buf, 4, MPI_CHARACTER,
     & status, ierr)
      call MPI_FILE_READ_AT_ALL(fh, 1, buf, 2, MPI_CHARACTER,
     & status, ierr)
c      call MPI_FILE_READ_ORDERED(fh, buf, 2, MPI_CHARACTER,
c     & status, ierr)
c      call MPI_FILE_READ_SHARED(fh, buf, 3, MPI_CHARACTER,
c     & status, ierr)
      call MPI_FILE_IREAD(fh, buf, 2, MPI_CHARACTER,
     & status, ierr)
      call MPI_FILE_IREAD_AT(fh, 0, buf, 5, MPI_CHARACTER,
     & status, ierr)
c      call MPI_FILE_IREAD_SHARED(fh, buf, 4, MPI_CHARACTER,
c     & status, ierr)
     
      call MPI_FILE_SEEK(fh, 5, MPI_SEEK_SET, ierr)
      
      
      call MPI_FILE_CLOSE(fh, ierr)

      print*, 'node', rank, ': ', inbuf

      call MPI_FINALIZE(ierr)
      end
