program H5BlockCoordsTest
  implicit none

#ifdef PARALLEL_IO
  include 'mpif.h'
#endif
  include 'H5PartF.h'

#ifdef PARALLEL_IO
  integer :: ierr, comm
#endif
  integer*8 :: file_id, status, i
  real*8, allocatable :: r8buf(:,:,:), r8buf3(:,:,:,:), coords(:)
  real*8 :: x, y, z

#ifdef PARALLEL_IO
  call MPI_INIT(ierr)
  comm = MPI_COMM_WORLD
#endif

  ! this enables level 4 ("debug") messages to be
  ! printed by the H5Part library
  ! (4_8 is the literal for an integer*8 with value 4)
  status = h5pt_set_verbosity_level (4_8)

  ! open the a file called 'test.h5' in parallel for writing
#ifdef PARALLEL_IO
  file_id = h5pt_openw_par_align ('test.h5', comm, 1048576_8, "vfd_mpiposix")
#else
  file_id = h5pt_openw_align ('test.h5', 1048576_8, "vfd_mpiposix")
#endif

  status = h5bl_define3dlayout(file_id, 1_8, 30_8, 1_8, 30_8, 1_8, 30_8)

  ! in the Fortran API, steps start at 1
  status = h5pt_setstep (file_id, 1_8)

  ! create fake data
  allocate(r8buf(30,30,30))
  allocate(r8buf3(30,30,30,3))
  allocate(coords(30))
  x = 1.0
  y = 2.0
  z = 3.0

  ! write the data
  status = h5bl_3d_write_scalar_field_r8(file_id, "x", r8buf(1,1,1))
  status = h5bl_3d_set_field_origin(file_id, "x", x, y, z)
  status = h5bl_3d_set_field_spacing(file_id, "x", x, y, z)

  status = h5bl_3d_write_3dvector_field_r8(file_id, "v", r8buf3(1,1,1,1), r8buf3(1,1,1,2), r8buf3(1,1,1,3))
  status = h5bl_3d_set_field_origin(file_id, "v", x, y, z)
  status = h5bl_3d_set_field_spacing(file_id, "v", x, y, z)

  ! create fake coordinates
  do i = 1, 30
    coords(i) = 3*i
  enddo
  status = h5bl_3d_set_field_xcoords(file_id, "v", coords, 30_8)
  do i = 1, 30
    coords(i) = 3*i + 1
  enddo
  status = h5bl_3d_set_field_ycoords(file_id, "v", coords, 30_8)
  do i = 1, 30
    coords(i) = 3*i + 2
  enddo
  status = h5bl_3d_set_field_zcoords(file_id, "v", coords, 30_8)

  ! read fake coordinates
  do i = 1, 30
    coords(i) = -1
  enddo
  status = h5bl_3d_get_field_xcoords(file_id, "v", coords, 30_8)
  write(*, *) coords
  !write(*, "I3") ( coords(i), i=1, 30 )
  do i = 1, 30
    coords(i) = -1
  enddo
  status = h5bl_3d_get_field_ycoords(file_id, "v", coords, 30_8)
  write(*, *) coords
  !write(*, "I3") ( coords(i), i=1, 30 )
  do i = 1, 30
    coords(i) = -1
  enddo
  status = h5bl_3d_get_field_zcoords(file_id, "v", coords, 30_8)
  write(*, *) coords
  !write(*, "I3") ( coords(i), i=1, 30 )

  ! close the file
  status = h5pt_close (file_id)

#ifdef PARALLEL_IO
  call MPI_FINALIZE(ierr)
#endif

end program H5BlockCoordsTest

