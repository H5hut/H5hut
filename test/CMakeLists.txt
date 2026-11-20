#--------------------------------------------------------
# CMake test driver for H5hut.
# The historical autotools test suite in this directory is not wired up
# here; only tests added for the CMake build live in this file.
#--------------------------------------------------------
if (NOT H5hut_WITH_MPI)
  return ()
endif ()

find_package (MPI REQUIRED)

#--------------------------------------------------------
# subfiling VFD smoke test: compiles against any parallel HDF5 >= 1.14
# and skips at runtime if HDF5 was built without subfiling support.
#--------------------------------------------------------
add_executable (h5u_subfiling h5u_subfiling.c)
target_link_libraries (h5u_subfiling H5hut)

if (MPIEXEC_EXECUTABLE)
  add_test (
    NAME h5u_subfiling
    COMMAND ${MPIEXEC_EXECUTABLE} ${MPIEXEC_NUMPROC_FLAG} 4
            $<TARGET_FILE:h5u_subfiling>)
endif ()
