cmake_minimum_required(VERSION 3.23)

#--------------------------------------------------------
# Project
#--------------------------------------------------------
project(
  H5hut 
  VERSION 2.0.0
  LANGUAGES C
)

cmake_policy(SET CMP0074 NEW)

#--------------------------------------------------------
# options
#--------------------------------------------------------
option(H5hut_WITH_MPI "Enable Parallel IO support" ON)
option(H5hut_WITH_FORTRAN "Enable fortran support, currently unimplemented" OFF)

#--------------------------------------------------------
# dependencies
#--------------------------------------------------------
if (NOT HDF5_FOUND)
  find_package(HDF5 CONFIG REQUIRED)
endif()

if (H5hut_WITH_MPI)
  find_package(MPI REQUIRED)
endif()

#--------------------------------------------------------
# Source files
#--------------------------------------------------------
set(H5hut_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/h5t_model.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/h5_attachments.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/h5_syscall.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/h5_err.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/h5t_store.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/private/h5t_adjacencies_tetm.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/private/h5t_core.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/private/h5t_model_tetm.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/private/h5t_retrieve_tetm.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/private/h5_hdf5.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/private/h5_qsort.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/private/h5_lustre.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/private/h5_io.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/private/h5_fcmp.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/private/h5t_access_trim.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/private/h5t_store_trim.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/private/h5_qsort_r.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/private/h5t_core_tetm.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/private/h5t_ref_elements.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/private/h5t_adjacencies_trim.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/private/h5t_retrieve_trim.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/private/h5t_store_tetm.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/private/h5t_access_tetm.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/private/h5_hsearch.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/private/h5_maps.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/private/h5t_io_trim.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/private/h5t_model_trim.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/private/h5t_io_tetm.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/private/h5t_core_trim.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/h5b_attribs.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/h5b_model.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/h5_log.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/h5_init.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/h5t_map.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/h5t_retrieve.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/h5t_octree.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/h5b_io.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/h5_model.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/h5u_io.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/h5u_model.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/h5t_io.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/h5t_adjacencies.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/h5_file.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/h5t_tags.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/h5core/h5_attribs.c
)

set(PUBLIC_INCLUDE  src/include)
set(PRIVATE_INCLUDE src/h5core)

file(GLOB_RECURSE PUBLIC_HDRS  ${PUBLIC_INCLUDE}/*.h)
file(GLOB_RECURSE PRIVATE_HDRS ${PRIVATE_INCLUDE}/*.h)

#--------------------------------------------------------
# define library
#--------------------------------------------------------
add_library(H5hut ${H5hut_SRCS})

#--------------------------------------------------------
# immediate dependencies
#--------------------------------------------------------
target_link_libraries(H5hut hdf5::hdf5)

#--------------------------------------------------------
# Parallel compilation settings
#--------------------------------------------------------
if(HDF5_IS_PARALLEL)
  if (NOT H5hut_WITH_MPI)
    find_package(MPI REQUIRED)
    message("Warning, MPI was not enabled, but hdf5 uses MPI already, parallel support enabled")
  endif()
  target_compile_definitions(H5hut PUBLIC PARALLEL_IO)
  target_link_libraries(H5hut MPI::MPI_C)
else()
  message("HDF5_IS_PARALLEL not found, Parallel IO will not be enabled")
endif()

#--------------------------------------------------------
# API compatibility setting, support 1.12 onwards
#--------------------------------------------------------
if ("${HDF5_VERSION}" VERSION_GREATER_EQUAL 1.12)
  target_compile_definitions(H5hut PUBLIC H5_USE_120_API)
endif()

#--------------------------------------------------------
# Includes, configuration headers
#--------------------------------------------------------
target_include_directories(H5hut
  PRIVATE ${PRIVATE_INCLUDE} ${CMAKE_CURRENT_BINARY_DIR}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${PUBLIC_INCLUDE}>
    $<INSTALL_INTERFACE:include>  # <prefix>/include
)

target_sources(H5hut
  PUBLIC
      FILE_SET public_headers
      TYPE HEADERS
      BASE_DIRS ${PUBLIC_INCLUDE}
      FILES ${PUBLIC_HDRS})

#--------------------------------------------------------
# Make sure static lib can be linked into dynamic one
#--------------------------------------------------------
set_property(TARGET H5hut PROPERTY POSITION_INDEPENDENT_CODE ON)

#--------------------------------------------------------
# Generate config.h
#--------------------------------------------------------
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.h.in.cmake 
  ${CMAKE_CURRENT_BINARY_DIR}/config.h)

#--------------------------------------------------------
# Install rules
#--------------------------------------------------------
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

install(TARGETS H5hut
        EXPORT  h5hut-targets
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include
        FILE_SET public_headers)

write_basic_package_version_file(
    "H5hutConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)


install(EXPORT h5hut-targets
    FILE h5hut-targets.cmake
    NAMESPACE H5hut::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake)

configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/h5hut_config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/H5hutConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/H5hutConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/H5hutConfigVersion.cmake
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake)

install(
    FILES "${PROJECT_SOURCE_DIR}/license.txt"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/h5hut
    COMPONENT license
)

export(EXPORT h5hut-targets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/h5hut-targets.cmake")

