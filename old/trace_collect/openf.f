      subroutine MPI_FILE_OPEN(comm, str, mode, info, fh, ierr)
        integer :: comm, mode, info, fh, ierr
        character :: str*(*)
        integer :: length
        length = len(str)
        call MPI_FILE_OPENL(comm, str, mode, info, fh, ierr, length)
        return
      end
