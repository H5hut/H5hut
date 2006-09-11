/**
	H5Part C API	
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>	/* va_arg - System dependent ?! */
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <hdf5.h>

#include "H5PartTypes.h"
#include "H5Part.h"
#include "H5PartPrivate.h"

/********* Private Variable Declarations *************/

static unsigned			_debug = 0;
static h5part_int64_t		_errno = H5PART_SUCCESS;
static h5part_error_handler	_err_handler = H5PartDefaultErrorHandler;

/********** Definitions of Functions ******/

/*========== File Opening/Closing ===============*/
/*!
  Opens file with specified filename. 

  If you open with flag \c H5PART_WRITE, it will truncate any
  file with the specified filename and start writing to it. If 
  you open with \c H5PART_APPEND, then you can append new timesteps.
  If you open with \c H5PART_READ, then it will open the file
  readonly.

  The typical extension for these files is \c .h5.
  
  H5PartFile should be treated as an essentially opaque
  datastructure.  It acts as the file handle, but internally
  it maintains several key state variables associated with 
  the file.

  \return	File handle or \c NULL
 */

H5PartFile*
H5PartOpenFileParallel (
	const char *filename,	/*!< [in] The name of the data file to open. */
	unsigned flags		/*!< [in] The access mode for the file. */
#ifdef PARALLEL_IO
	,MPI_Comm comm		/*!< [in] MPI communicator */
#endif
) {
	static char *__funcname = "H5PartOpenFileParallel";

	if ( _H5Part_init() < 0 ) {
		HANDLE_H5PART_INIT_ERR;
		return NULL;
	}
	_errno = H5PART_SUCCESS;
	H5PartFile *f = NULL;

	f = (H5PartFile*) malloc( sizeof (H5PartFile) );
	if( f == NULL ) {
		HANDLE_H5PART_NOMEM_ERR;
		goto error_cleanup;
	}
	memset (f, 0, sizeof (H5PartFile));
	f->xfer_prop = f->create_prop = f->access_prop = H5P_DEFAULT;

#ifdef PARALLEL_IO
	/* for the SP2... perhaps different for linux */
	MPI_Info info = MPI_INFO_NULL;

	if (MPI_Comm_size (comm, &f->nprocs) != MPI_SUCCESS) {
		HANDLE_MPI_COMM_SIZE_ERR;
		goto error_cleanup;
	}
	if (MPI_Comm_rank (comm, &f->myproc) != MPI_SUCCESS) {
		HANDLE_MPI_COMM_RANK_ERR;
		goto error_cleanup;
	}

	f->pnparticles = (h5part_int64_t*) malloc (f->nprocs * sizeof (h5part_int64_t));
	if (f->pnparticles == NULL) {
		HANDLE_H5PART_NOMEM_ERR;
		goto error_cleanup;
	}
		
	f->access_prop = H5Pcreate (H5P_FILE_ACCESS);
	if (f->access_prop < 0) {
		HANDLE_H5P_CREATE_ERR;
		goto error_cleanup;
	}

	if (H5Pset_fapl_mpio (f->access_prop, comm, info) < 0) {
		HANDLE_H5P_SET_FAPL_MPIO_ERR;
		goto error_cleanup;
	}
		
	/* create_prop: tunable parameters like blocksize and btree sizes */
	/* f->create_prop = H5Pcreate(H5P_FILE_CREATE); */
	f->create_prop = H5P_DEFAULT;

	/* currently create_prop is empty */
	/* xfer_prop:  also used for parallel I/O, during actual writes
	   rather than the access_prop which is for file creation. */
	f->xfer_prop = H5Pcreate (H5P_DATASET_XFER);
	if (f->xfer_prop < 0) {
		HANDLE_H5P_CREATE_ERR;
		goto error_cleanup;
	}

	if (H5Pset_dxpl_mpio (f->xfer_prop, H5FD_MPIO_COLLECTIVE) < 0) {
		HANDLE_H5P_SET_DXPL_MPIO_ERR;
		goto error_cleanup;
	}

	f->comm = comm;
#endif
	if ( flags == H5PART_READ ) {
		f->file = H5Fopen (filename, H5F_ACC_RDONLY, f->access_prop);
	}
	else if ( flags == H5PART_WRITE ){
		f->file = H5Fcreate (filename, H5F_ACC_TRUNC, f->create_prop, f->access_prop);
	}
	else if ( flags == H5PART_APPEND ) {
		int fd = open (filename, O_RDONLY, 0);
		if ( (fd == -1) && (errno == ENOENT) ) {
			f->file = H5Fcreate(filename, H5F_ACC_TRUNC,
					    f->create_prop, f->access_prop);
		}
		else if (fd != -1) {
			close (fd);
			f->file = H5Fopen (filename, H5F_ACC_RDWR, f->access_prop);
			/*
			  The following function call returns an error,
			  if f->file < 0. But we can safely ignore this.
			*/
			f->timestep = H5PartGetNumSteps (f);
		}
	}
	else {
		HANDLE_H5PART_FILE_ACCESS_TYPE_ERR ( flags );
		goto error_cleanup;
	}

	if (f->file < 0) {
		HANDLE_H5F_OPEN_ERR ( filename, flags );
		goto error_cleanup;
	}
#ifdef PARALLEL_IO
	else {
		print_info ( "H5PartOpenFileParallel: "
			       "Proc[%d]: Opened file \"%s\" val=%d",
			       f->myproc,
			       filename,
			       f->file );
	}
#endif
	f->mode = flags;
	f->timegroup = 0;
	f->shape = 0;
	f->diskshape = H5S_ALL;
	f->memshape = H5S_ALL;
	f->viewstart = -1;
	f->viewend = -1;

	return f;

 error_cleanup:
	if (f != NULL ) {
		if (f->pnparticles != NULL) {
			free (f->pnparticles);
		}
		free (f);
	}
	return NULL;
}

/*!
  Opens file with specified filename. 

  If you open with flag \c H5PART_WRITE, it will truncate any
  file with the specified filename and start writing to it. If 
  you open with \c H5PART_APPEND, then you can append new timesteps.
  If you open with \c H5PART_READ, then it will open the file
  readonly.

  The typical extension for these files is \c .h5.
  
  H5PartFile should be treated as an essentially opaque
  datastructure.  It acts as the file handle, but internally
  it maintains several key state variables associated with 
  the file.

  \return	File handle or \c NULL
 */

H5PartFile*
H5PartOpenFile (
	const char *filename,	/*!< [in] The name of the data file to open. */
	unsigned flags		/*!< [in] The access mode for the file. */
	) {

	static char *__funcname = "H5PartOpenFile";
	if ( _H5Part_init() < 0 ) {
		HANDLE_H5PART_INIT_ERR;
		return NULL;
	}

	_errno = H5PART_SUCCESS;
	H5PartFile *f = NULL;

	f = (H5PartFile*) malloc( sizeof (H5PartFile) );
	if( f == NULL ) {
		HANDLE_H5PART_NOMEM_ERR;
		goto error_cleanup;
	}
	memset (f, 0, sizeof (H5PartFile));
	f->xfer_prop = f->create_prop = f->access_prop = H5P_DEFAULT;

#ifdef PARALLEL_IO
	f->pnparticles = 0;
	f->comm = MPI_COMM_WORLD;
	f->nprocs = 1;
	f->myproc = 0;
#endif
	if (flags == H5PART_READ) {
		f->file = H5Fopen (filename, H5F_ACC_RDONLY, f->access_prop);
	}
	else if (flags == H5PART_WRITE){
		f->file = H5Fcreate (filename, H5F_ACC_TRUNC, f->create_prop, f->access_prop);
	}
	else if (flags == H5PART_APPEND) {
		int fd = open (filename, O_RDONLY, 0);
		if ( (fd == -1) && (errno == ENOENT) ) {
			f->file = H5Fcreate(filename, H5F_ACC_TRUNC,
					    f->create_prop, f->access_prop);
		}
		else if (fd != -1) {
			close (fd);
			f->file = H5Fopen (filename, H5F_ACC_RDWR, f->access_prop);
			/*
			  The following function call returns an error,
			  if f->file < 0. But we can safely ignore it
			*/
			f->timestep = H5PartGetNumSteps (f);
		}
	}
	else {
		HANDLE_H5PART_FILE_ACCESS_TYPE_ERR ( flags );
		goto error_cleanup;
	}

	if (f->file < 0) {
		HANDLE_H5F_OPEN_ERR ( filename, flags );
		goto error_cleanup;
	}

	f->mode = flags;
	f->timegroup = 0;
	f->shape = 0;
	f->diskshape = H5S_ALL;
	f->memshape = H5S_ALL;
	f->viewstart = -1;
	f->viewend = -1;

	return f;

 error_cleanup:
	if (f != NULL ) {
		if (f->pnparticles != NULL) {
			free (f->pnparticles);
		}
		free (f);
	}
	return NULL;
}

/*!
  Checks if a file was successfully opened.

  \return	\c H5PART_SUCCESS or error code
 */
h5part_int64_t
_H5PartFileIsValid (
	H5PartFile *f		/*!< filehandle  to check validity of */
	) {

	if( f == NULL )
		return H5PART_ERR_BADFD;
	else if(f->file > 0)
		return H5PART_SUCCESS;
	else
		return H5PART_ERR_BADFD;
}

/*!
  Closes an open file.

  \return	\c H5PART_SUCCESS or error code
*/
h5part_int64_t
H5PartCloseFile (
	H5PartFile *f		/*!< [in] filehandle of the file to close */
	) {

	static char *__funcname = "H5PartCloseFile";
	herr_t r = 0;
	_errno = H5PART_SUCCESS;

	CHECK_FILEHANDLE ( f );

	if( f->shape > 0 ) {
		r = H5Sclose( f->shape );
		if ( r < 0 ) HANDLE_H5S_CLOSE_ERR;
		f->shape = 0;
	}
	if( f->timegroup > 0 ) {
		r = H5Gclose( f->timegroup );
		if ( r < 0 ) HANDLE_H5G_CLOSE_ERR;
		f->timegroup = 0;
	}
	if( f->diskshape != H5S_ALL ) {
		r = H5Sclose( f->diskshape );
		if ( r < 0 ) HANDLE_H5S_CLOSE_ERR;
		f->diskshape = 0;
	}
	if( f->xfer_prop != H5P_DEFAULT ) {
		r = H5Pclose( f->xfer_prop );
		if ( r < 0 ) HANDLE_H5P_CLOSE_ERR ( "f->xfer_prop" );
		f->xfer_prop = H5P_DEFAULT;
	}
	if( f->access_prop != H5P_DEFAULT ) {
		r = H5Pclose( f->access_prop );
		if ( r < 0 ) HANDLE_H5P_CLOSE_ERR ( "f->access_prop" );
		f->access_prop = H5P_DEFAULT;
	}  
	if( f->create_prop != H5P_DEFAULT ) {
		r = H5Pclose( f->create_prop );
		if ( r < 0 ) HANDLE_H5P_CLOSE_ERR ( "f->create_prop" );
		f->create_prop = H5P_DEFAULT;
	}
	if ( f->file ) {
		r = H5Fclose( f->file );
		if ( r < 0 ) HANDLE_H5F_CLOSE_ERR;
		f->file = 0;
	}
	if( f->pnparticles ) {
		free( f->pnparticles );
	}
	free( f );

	return _errno;
}

/*============== File Writing Functions ==================== */
/*!
  Set number of particles for current time-step.

  This function's sole purpose is to prevent 
  needless creation of new HDF5 DataSpace handles if the number of 
  particles is invariant throughout the simulation. That's its only reason 
  for existence. After you call this subroutine, all subsequent 
  operations will assume this number of particles will be written.


  \return	\c H5PART_SUCCESS or error code
 */
h5part_int64_t
H5PartSetNumParticles (
	H5PartFile *f,			/*!< [in] Handle to open file */
	h5part_int64_t nparticles	/*!< [in] Number of particles */
	) {

	static char *__funcname = "H5PartSetNumParticles";
	int r;
#ifdef PARALLEL_IO
#ifdef HDF5V160
	hssize_t start[1];
#else
	hsize_t start[1];
#endif

	hsize_t stride[1];
	hsize_t count[1];
	hsize_t total;
	hsize_t dmax = H5S_UNLIMITED;
	register int i;
#endif

	CHECK_FILEHANDLE( f );

#ifndef PARALLEL_IO
	/*
	  if we are not using parallel-IO, there is enough information
	   to know that we can short circuit this routine.  However,
	   for parallel IO, this is going to cause problems because
	   we don't know if things have changed globally
	*/
	if ( f->nparticles == nparticles ) {
		return H5PART_SUCCESS;
	}
#endif
	if ( f->diskshape != H5S_ALL ) {
		r = H5Sclose( f->diskshape );
		if ( r < 0 ) return HANDLE_H5S_CLOSE_ERR;
		f->diskshape = H5S_ALL;
	}
	if(f->memshape != H5S_ALL) {
		r = H5Sclose( f->memshape );
		if ( r < 0 ) return HANDLE_H5S_CLOSE_ERR;
		f->memshape = H5S_ALL;
	}
	if( f->shape ) {
		r = H5Sclose(f->shape);
		if ( r < 0 ) return HANDLE_H5S_CLOSE_ERR;
	}
	f->nparticles =(hsize_t) nparticles;
#ifndef PARALLEL_IO
	f->shape = H5Screate_simple (1,
				     &(f->nparticles),
				     NULL);
	if ( f->shape < 0 ) HANDLE_H5S_CREATE_SIMPLE_ERR ( f->nparticles );

#else /* PARALLEL_IO */
	/*
	  The Gameplan here is to declare the overall size of the on-disk
	  data structure the same way we do for the serial case.  But
	  then we must have additional "DataSpace" structures to define
	  our in-memory layout of our domain-decomposed portion of the particle
	  list as well as a "selection" of a subset of the on-disk 
	  data layout that will be written in parallel to mutually exclusive
	  regions by all of the processors during a parallel I/O operation.
	  These are f->shape, f->memshape and f->diskshape respectively.
	*/

	/*
	  acquire the number of particles to be written from each MPI process
	*/
	r = MPI_Allgather (
		&nparticles, 1, MPI_LONG_LONG,
		f->pnparticles, 1, MPI_LONG_LONG,
		f->comm);
	if ( r != MPI_SUCCESS) {
		return HANDLE_MPI_ALLGATHER_ERR;
	}
	if ( f->myproc == 0 ) {
		print_debug ("H5PartSetNumParticels: AllGather:  Particle offsets:\n");
		for(i=0;i<f->nprocs;i++) 
			print_debug ( "\tnp=%d\n",(int) f->pnparticles[i] );
	}
	/* should I create a selection here? */

	/* compute start offsets */
	stride[0] = 1;
	start[0] = 0;
	for (i=0; i<f->myproc; i++) {
		start[0] += f->pnparticles[i];
	}
	


        /* compute total nparticles */
	total = 0;
	for (i=0; i < f->nprocs; i++) {
		total += f->pnparticles[i];
	}

	/* declare overall datasize */
	f->shape = H5Screate_simple (1, &total, &total);
	if (f->shape < 0) return HANDLE_H5S_CREATE_SIMPLE_ERR ( total );


	/* declare overall data size  but then will select a subset */
	f->diskshape = H5Screate_simple (1, &total, &total);
	if (f->diskshape < 0) return HANDLE_H5S_CREATE_SIMPLE_ERR ( total );

	/* declare local memory datasize */
	f->memshape = H5Screate_simple (1, &(f->nparticles), &dmax);
	if (f->memshape < 0) return HANDLE_H5S_CREATE_SIMPLE_ERR ( f->nparticles );

	count[0] = nparticles; /* based on local nparticles (for the selection */
	/* and then set the subset of the data you will write to */
	r = H5Sselect_hyperslab (f->diskshape, H5S_SELECT_SET, start, stride, count, NULL);
	if ( r < 0 ) return HANDLE_H5S_SELECT_HYPERSLAB_ERR;

	if ( f->timegroup < 0 ) {
		r = H5PartSetStep ( f, 0 );
		if ( r < 0 ) return HANDLE_H5PART_SETSTEP_ERR( r, 0 );
		
	}
#endif
	return H5PART_SUCCESS;
}

/*!
  Write array of 64 bit floating point data to file.

  After setting the number of particles with \c H5PartSetNumParticles() and
  the current timestep using \c H5PartSetStep(), you can start writing datasets
  into the file. Each dataset has a name associated with it (chosen by the
  user) in order to facilitate later retrieval. The name of the dataset is
  specified in the parameter \c name, which must be a null-terminated string.

  There are no restrictions on naming of datasets, but it is useful to arrive
  at some common naming convention when sharing data with other groups.

  The writing routines also implicitly store the datatype of the array so that
  the array can be reconstructed properly on other systems with incompatible type
  representations.

  All data that is written after setting the timestep is associated with that
  timestep. While the number of particles can change for each timestep, you
  cannot change the number of particles in the middle of a given timestep.

  The data is committed to disk before the routine returns.

  \return	\c H5PART_SUCCESS or error code
 */
h5part_int64_t
H5PartWriteDataFloat64 (
	H5PartFile *f,			/*!< [in] Handle to open file */
	char *name,			/*!< [in] Name to associate array/dataset with */
	h5part_float64_t *array		/*!< [in] Array to commit to disk */
	) {

	static char *__funcname = "H5PartWriteDataFloat64";
	register int r;
	hid_t dataset;

	CHECK_FILEHANDLE ( f );
	CHECK_WRITABLE_MODE( f );
	CHECK_TIMEGROUP( f );

	print_debug ( "H5PartWriteDataFloat64: "
			"Create a dataset[%s]  mounted on the timegroup %d",
			name,f->timestep );

	dataset = H5Dcreate(f->timegroup,name,H5T_NATIVE_DOUBLE,f->shape,H5P_DEFAULT);
	if ( dataset < 0 ) return HANDLE_H5D_CREATE_ERR ( name, f->timestep );

	r = H5Dwrite(dataset,H5T_NATIVE_DOUBLE,f->memshape,f->diskshape,H5P_DEFAULT,array);
	if ( r < 0 ) return HANDLE_H5D_WRITE_ERR ( name, f->timestep );

	r = H5Dclose(dataset);
	if ( r < 0 ) return HANDLE_H5D_CLOSE_ERR;

	return r;
}

/*!
  Write array of 64 bit integer data to file.

  After setting the number of particles with \c H5PartSetNumParticles() and
  the current timestep using \c H5PartSetStep(), you can start writing datasets
  into the file. Each dataset has a name associated with it (chosen by the
  user) in order to facilitate later retrieval. The name of the dataset is
  specified in the parameter \c name, which must be a null-terminated string.

  There are no restrictions on naming of datasets, but it is useful to arrive
  at some common naming convention when sharing data with other groups.

  The writing routines also implicitly store the datatype of the array so that
  the array can be reconstructed properly on other systems with incompatible type
  representations.

  All data that is written after setting the timestep is associated with that
  timestep. While the number of particles can change for each timestep, you
  cannot change the number of particles in the middle of a given timestep.

  The data is committed to disk before the routine returns.

  \return	\c H5PART_SUCCESS or error code
 */
h5part_int64_t
H5PartWriteDataInt64 (
	H5PartFile *f,			/*!< [in] Handle to open file */
	char *name,			/*!< [in] Name to associate array/dataset with */
	h5part_int64_t *array		/*!< [in] Array to commit to disk */
	) {

	static char *__funcname = "H5PartOpenWriteDataInt64";
	register int r;
	hid_t dataset;

	CHECK_FILEHANDLE ( f );
	CHECK_WRITABLE_MODE( f );
	CHECK_TIMEGROUP( f );

	print_debug ( "%s: Create a dataset[%s] mounted on the timegroup %d",
			__funcname, name, f->timestep );

	dataset = H5Dcreate(f->timegroup,name,H5T_NATIVE_INT64,f->shape,H5P_DEFAULT);
	if ( dataset < 0 ) return HANDLE_H5D_CREATE_ERR ( name, f->timestep );

	r = H5Dwrite(dataset,H5T_NATIVE_INT64,f->memshape,f->diskshape,H5P_DEFAULT,array);
	if ( r < 0 ) return HANDLE_H5D_WRITE_ERR( name, f->timestep );

	r = H5Dclose(dataset);
	if ( r < 0 ) return HANDLE_H5D_CLOSE_ERR;

	return H5PART_SUCCESS;
}

/*!
  Writes a string attribute bound to a file.

  This function creates a new attribute \c name with the string \c value as
  content. The attribute is bound to the file associated with the file handle 
  \c f.

  If the attribute already exists an error will be returned. There
  is currently no way to change the content of an existing attribute.

  \return	\c H5PART_SUCCESS or error code   
*/

h5part_int64_t
H5PartWriteFileAttribString (
	H5PartFile *f,			/*!< [in] Handle to open file */
	char *name,			/*!< [in] Name of attribute to create */
	char *value			/*!< [in] Value of attribute */ 
	) {

	static char *__funcname = "H5PartWriteFileAttribString";

	CHECK_FILEHANDLE ( f );
	CHECK_WRITABLE_MODE( f );

	return H5PartWriteFileAttrib(f,name,H5T_NATIVE_CHAR,value,strlen(value)+1);
}

/*!
  Writes a string attribute bound to the current time-step.

  This function creates a new attribute \c name with the string \c value as
  content. The attribute is bound to the current time step in the file given
  by the file handle \c f.

  If the attribute already exists an error will be returned. There
  is currently no way to change the content of an existing attribute.

  \return	\c H5PART_SUCCESS or error code   
*/

h5part_int64_t
H5PartWriteStepAttribString (
	H5PartFile *f,			/*!< [in] Handle to open file */
	char *name,			/*!< [in] Name of attribute to create */
	char *value			/*!< [in] Value of attribute */ 
	) {

	static char *__funcname = "H5PartWriteStepAttribString";

	CHECK_FILEHANDLE ( f );
	CHECK_WRITABLE_MODE( f );

	return H5PartWriteStepAttrib(f,name,H5T_NATIVE_CHAR,value,strlen(value)+1);
}

/*!
  Writes a attribute bound to the current time-step.

  This function creates a new attribute \c name with the string \c value as
  content. The attribute is bound to the current time step in the file given
  by the file handle \c f.

  The value of the attribute is given the parameter \c type, which must be one
  of \c H5T_NATIVE_DOUBLE, \c H5T_NATIVE_INT64 of \c H5T_NATIVE_CHAR, the array
  \c value and the number of elements \c nelem in the array.

  If the attribute already exists an error will be returned. There
  is currently no way to change the content of an existing attribute.

  \return	\c H5PART_SUCCESS or error code   
*/

h5part_int64_t
H5PartWriteStepAttrib (
	H5PartFile *f,			/*!< [in] Handle to open file */
	char *name,			/*!< [in] Name of attribute to create */
	hid_t type,			/*!< [in] Type of value: On of H5T_NATIVE_DOUBLE,
					  H5T_NATIVE_INT64, H5T_NATIVE_CHAR. */
	void *value,			/*!< [in] Value of attribute */ 
	int nelem			/*!< [in] Number of elements of type \c type */
	){

	static char *__funcname = "H5PartWriteStepAttrib";
	register int r;
	hid_t attrib;
	hid_t space;
	hsize_t len;

	CHECK_FILEHANDLE ( f );
	CHECK_WRITABLE_MODE( f );
	CHECK_TIMEGROUP( f );

	print_debug ( "H5PartWriteStepAttrib: "
			"Create an attribute[%s]  mounted on the timegroup %d",
			name, f->timestep );

	len = nelem;
	space = H5Screate_simple (1, &len, NULL);
	if ( space < 0 ) return HANDLE_H5S_CREATE_SIMPLE_ERR ( len );

	attrib = H5Acreate(f->timegroup,name,type,space,H5P_DEFAULT);
	if ( attrib < 0 ) return HANDLE_H5A_CREATE_ERR ( name );

	r = H5Awrite(attrib,type,value);
	if ( r < 0 ) return HANDLE_H5A_WRITE_ERR ( name );

	r = H5Aclose(attrib);
	if ( r < 0 ) return HANDLE_H5A_CLOSE_ERR;

	r = H5Sclose(space);
	if ( r < 0 ) return HANDLE_H5S_CLOSE_ERR;

	return r;
}

/*!
  Writes a attribute bound to the current time-step.

  This function creates a new attribute \c name with the string \c value as
  content. The attribute is bound to the current time step in the file given
  by the file handle \c f.

  The value of the attribute is given the parameter \c type, which must be one
  of \c H5T_NATIVE_DOUBLE, \c H5T_NATIVE_INT64 of \c H5T_NATIVE_CHAR, the array
  \c value and the number of elements \c nelem in the array.

  If the attribute already exists an error will be returned. There
  is currently no way to change the content of an existing attribute.

  \note This call has been superseeded by  \c H5PartWriteStepAttrib.

  \return	\c H5PART_SUCCESS or error code   
*/

h5part_int64_t
H5PartWriteAttrib (
	H5PartFile *f,			/*!< [in] Handle to open file */
	char *name,			/*!< [in] Name of attribute to create */
	hid_t type,			/*!< [in] Type of value: On of \c H5T_NATIVE_DOUBLE,
					  \c H5T_NATIVE_INT64, \c H5T_NATIVE_CHAR. */
	void *value,			/*!< [in] Value of attribute */ 
	int nelem			/*!< [in] Number of elements of type \c type */
	) {

	static char *__funcname = "H5PartWriteAttrib";

	CHECK_FILEHANDLE ( f );
	CHECK_WRITABLE_MODE ( f );

	return H5PartWriteStepAttrib(f,name,type,value,nelem);
}

/*!
  Writes a attribute bound to a file.

  This function creates a new attribute \c name with the string \c value as
  content. The attribute is bound to the file file given by the file handle
  \c f.

  The value of the attribute is given the parameter \c type, which must be one
  of H5T_NATIVE_DOUBLE, H5T_NATIVE_INT64 of H5T_NATIVE_CHAR, the array \c value
  and the number of elements \c nelem in the array.

  If the attribute already exists an error will be returned. There
  is currently no way to change the content of an existing attribute.

  \return	\c H5PART_SUCCESS or error code   
*/

h5part_int64_t
H5PartWriteFileAttrib (
	H5PartFile *f,			/*!< [in] Handle to open file */
	char *name,			/*!< [in] Name of attribute to create */
	hid_t type,			/*!< [in] Type of value: On of H5T_NATIVE_DOUBLE,
					  H5T_NATIVE_INT64, H5T_NATIVE_CHAR. */
	void *value,			/*!< [in] Value of attribute */ 
	int nelem			/*!< [in] Number of elements of type \c type */
	) {

	static char *__funcname = "H5PartWriteFileAttrib";
	register int r;
	hid_t attrib,rootgroup;
	hid_t space;
	hsize_t len;

	CHECK_FILEHANDLE ( f );
	CHECK_WRITABLE_MODE ( f );

	print_debug ( "%s: Create a file attribute[%s] spep=%d",
		      __funcname, name, f->timestep  );
	len = nelem;
	space = H5Screate_simple ( 1, &len, NULL )
		;			if ( space <= 0 ) return HANDLE_H5S_CREATE_SIMPLE_ERR( len );

	rootgroup = H5Gopen(f->file,"/")
		;			if ( rootgroup < 0 ) return HANDLE_H5G_OPEN_ERR( "/" );

	attrib = H5Acreate(rootgroup,name,type,space,H5P_DEFAULT)
		;			if ( attrib < 0 ) return HANDLE_H5A_CREATE_ERR( name );


	r = H5Gclose(rootgroup);	if ( r < 0 ) return HANDLE_H5G_CLOSE_ERR;
	r = H5Awrite(attrib,type,value);if ( r < 0 ) return HANDLE_H5A_WRITE_ERR ( name );
	r = H5Aclose(attrib);		if ( r < 0 ) return HANDLE_H5A_CLOSE_ERR;
	r = H5Sclose(space);		if ( r < 0 ) return HANDLE_H5S_CLOSE_ERR;
	return H5PART_SUCCESS;
}


/*!
  Gets the number of attributes bound to the current step.

  \return	Number of attributes bound to current time step or error code.
*/
h5part_int64_t
H5PartGetNumStepAttribs (
	H5PartFile *f			/*!< [in] Handle to open file */
	) {

	static char *__funcname = "H5PartGetNumStepAttribs";
	int r;

	CHECK_FILEHANDLE ( f );

	r = H5Aget_num_attrs(f->timegroup);
	if ( r < 0 ) HANDLE_H5A_GET_NUM_ATTRS_ERR;

	return r;
}

/*!
  Gets the number of attributes bound to the file.

  \return	Number of attributes bound to file \c f or error code.
*/
h5part_int64_t
H5PartGetNumFileAttribs (
	H5PartFile *f			/*!< [in] Handle to open file */
	) {

	static char *__funcname = "H5PartGetNumFileAttribs";
	int r;
	int nattribs;

	CHECK_FILEHANDLE ( f );

	/* open / for the file?
	   or is there a problem with 
	   file attributes? */
	hid_t rootgroup = H5Gopen ( f->file,"/" );
	if ( rootgroup < 0 ) HANDLE_H5G_OPEN_ERR ( "/" );

	nattribs = H5Aget_num_attrs(rootgroup);
	if ( nattribs < 0 ) HANDLE_H5A_GET_NUM_ATTRS_ERR;

	r = H5Gclose ( rootgroup );
	if ( r < 0 ) HANDLE_H5G_CLOSE_ERR;
	return nattribs;
}

/**
   H5PartNormType: Private
*/
static hid_t
_H5PartNormType (
	hid_t type
	) {
	H5T_class_t tclass = H5Tget_class(type);
	int size = H5Tget_size(type);

	switch(tclass){
	case H5T_INTEGER:
		if(size==8) {
			return H5T_NATIVE_INT64;
		}
		else if(size==1) {
			return H5T_NATIVE_CHAR;
		}
		break;
	case H5T_FLOAT:
		return H5T_NATIVE_DOUBLE;
	default:
		; /* NOP */
	}
	print_warn ( "_H5PartNormType: Unknown type %d", (int)type );

	return -1;
}

/*!
  Gets the name, type and number of elements of the step attribute
  specified by its index.

  This function can be used to retrieve all attributes bound to the
  current time-step by looping from \c 0 to the number of attribute
  minus one.  The number of attributes bound to the current
  time-step can be queried by calling the function
  \c H5PartGetNumStepAttribs().

  \return	\c H5PART_SUCCESS or error code 
*/
h5part_int64_t
H5PartGetStepAttribInfo (
	H5PartFile *f,			/*!< [in]  Handle to open file */
	int idx,			/*!< [in]  Index of attribute to get infos about */
	char *name,			/*!< [out] Name of attribute */
	size_t maxname,			/*!< [in]  length of buffer \c name */
	hid_t *type,			/*!< [out] Type of value: On of H5T_NATIVE_DOUBLE,
					           H5T_NATIVE_INT64, H5T_NATIVE_CHAR. */
	int *nelem			/*!< [out] Number of elements of type \c type */
	) {
	
	static char *__funcname = "H5PartGetStepAttribInfo";
	hid_t attrib;
	hid_t mytype;
	hid_t space;
	int r;

	CHECK_FILEHANDLE( f );

	attrib = H5Aopen_idx(f->timegroup,idx);
	if ( attrib < 0 ) return HANDLE_H5A_OPEN_IDX_ERR ( idx );

	mytype = H5Aget_type(attrib);
	if ( attrib < 0 ) return HANDLE_H5A_GET_TYPE_ERR;

	space =  H5Aget_space(attrib);
	if ( attrib < 0 ) return HANDLE_H5A_GET_SPACE_ERR;

	if ( nelem ) {
		*nelem=H5Sget_simple_extent_npoints(space);
		if ( *nelem < 0 ) return HANDLE_H5S_GET_SIMPLE_EXTENT_NPOINTS_ERR;
	}
	if ( name ) {
		r = H5Aget_name(attrib,maxname,name);
		if ( r < 0 ) return HANDLE_H5A_GET_NAME_ERR;
	}
	if ( type ) {
		*type = _H5PartNormType ( mytype );
	}
	r = H5Sclose(space); 	if ( r < 0 ) return HANDLE_H5S_CLOSE_ERR;
	r = H5Tclose(mytype);   if ( r < 0 ) return HANDLE_H5T_CLOSE_ERR;
	r = H5Aclose(attrib);   if ( r < 0 ) return HANDLE_H5A_CLOSE_ERR;

	return H5PART_SUCCESS;
}

/*!
  Gets the name, type and number of elements of the file attribute
  specified by its index.

  This function can be used to retrieve all attributes bound to the
  file \c f by looping from \c 0 to the number of attribute minus
  one.  The number of attributes bound to file \c f can be queried
  by calling the function \c H5PartGetNumFileAttribs().

  \return	\c H5PART_SUCCESS or error code 
*/

h5part_int64_t
H5PartGetFileAttribInfo (
	H5PartFile *f,			/*!< [in]  Handle to open file */
	int idx,			/*!< [in]  Index of attribute to get infos about */
	char *name,			/*!< [out] Name of attribute */
	size_t maxname,			/*!< [in]  length of buffer \c name */
	hid_t *type,			/*!< [out] Type of value: On of H5T_NATIVE_DOUBLE,
					           H5T_NATIVE_INT64, H5T_NATIVE_CHAR. */
	int *nelem			/*!< [out] Number of elements of type \c type */
	) {

	static char *__funcname = "H5PartGetFileAttribInfo";
	hid_t rootgroup;
	hid_t attrib;
	hid_t mytype;
	hid_t space;
	int r;

	CHECK_FILEHANDLE( f );

	rootgroup = H5Gopen(f->file,"/");
	if ( rootgroup < 0 ) return HANDLE_H5G_OPEN_ERR( "/" );
	attrib = H5Aopen_idx(rootgroup,idx);
	if ( attrib < 0 ) return HANDLE_H5A_OPEN_IDX_ERR( idx );
	mytype = H5Aget_type(attrib);
	if ( mytype < 0 ) return HANDLE_H5A_GET_TYPE_ERR;
	space = H5Aget_space(attrib);
	if ( space < 0 ) return HANDLE_H5A_GET_SPACE_ERR;

	if ( nelem ) {
		*nelem=H5Sget_simple_extent_npoints(space);
		if ( *nelem < 0 ) return HANDLE_H5S_GET_SIMPLE_EXTENT_NPOINTS_ERR;
	}
	if ( name ) {
		r = H5Aget_name(attrib,maxname,name);
		if ( r < 0 ) return HANDLE_H5A_GET_NAME_ERR;

	}
	if ( type ) {
		*type = _H5PartNormType ( mytype );
	}
	r = H5Sclose(space); 	if ( r < 0 ) return HANDLE_H5S_CLOSE_ERR;
	r = H5Tclose(mytype);   if ( r < 0 ) return HANDLE_H5T_CLOSE_ERR;
	r = H5Aclose(attrib);   if ( r < 0 ) return HANDLE_H5A_CLOSE_ERR;

	return H5PART_SUCCESS;
}

/*!
  Reads an attribute bound to current time-step.

  \return \c H5PART_SUCCESS or error code 
*/

h5part_int64_t
H5PartReadStepAttrib (
	H5PartFile *f,			/*!< [in]  Handle to open file */
	char *name,			/*!< [in] Name of attribute to read */
	void *value			/*!< [out] Value of attribute */
	) {

	static char *__funcname = "H5PartReadStepAttrib";

	/* use the open attribute by name mode of operation */
	hid_t attrib = H5Aopen_name(f->timegroup,name);
	hid_t mytype;
	hid_t space;
	hsize_t nelem;
	hid_t type;
	hid_t r;

	CHECK_FILEHANDLE( f );

	if ( attrib <= 0 ) return HANDLE_H5A_OPEN_NAME_ERR( name );

	mytype = H5Aget_type(attrib);
	if ( mytype < 0 ) return HANDLE_H5A_GET_TYPE_ERR;

	space = H5Aget_space(attrib);
	if ( space < 0 ) return HANDLE_H5A_GET_SPACE_ERR;

	nelem = H5Sget_simple_extent_npoints(space);
	if ( nelem < 0 ) return HANDLE_H5S_GET_SIMPLE_EXTENT_NPOINTS_ERR;

	type = _H5PartNormType ( mytype );

	r = H5Aread(attrib,type,value);	if ( r < 0 ) return HANDLE_H5A_READ_ERR;

	r = H5Sclose(space);		if ( r < 0 ) return HANDLE_H5S_CLOSE_ERR;
	r = H5Tclose(mytype);		if ( r < 0 ) return HANDLE_H5T_CLOSE_ERR;
	r = H5Aclose(attrib);		if ( r < 0 ) return HANDLE_H5A_CLOSE_ERR;

	return H5PART_SUCCESS;
}

/*!
  Reads an attribute bound to current time-step.

  \return \c H5PART_SUCCESS or error code 

  \note
  This function has been superseeded by \c H5PartReadStepAttrib().

*/
h5part_int64_t
H5PartReadAttrib (
	H5PartFile *f,			/*!< [in]  Handle to open file */
	char *name,			/*!< [in] Name of attribute to read */
	void *value			/*!< [out] Value of attribute */
	) {

	static char *__funcname = "H5PartReadAttrib";

	CHECK_FILEHANDLE( f );

	/* use the open attribute by name mode of operation */
	return H5PartReadStepAttrib(f,name,value);
}

/*!
  Reads an attribute bound to file \c f.

  \return \c H5PART_SUCCESS or error code 
*/


h5part_int64_t
H5PartReadFileAttrib ( 
	H5PartFile *f,
	char *name,
	void *value
	) {

	static char *__funcname = "H5PartReadFileAttrib";

	hid_t rootgroup;
	hid_t attrib;
	hid_t mytype;
	hid_t space;
	hsize_t nelem;
	hid_t type;
	hid_t r;

	CHECK_FILEHANDLE( f );

	rootgroup = H5Gopen(f->file,"/") 
		;			if ( rootgroup < 0 ) return HANDLE_H5G_OPEN_ERR( "/" );

	attrib = H5Aopen_name(rootgroup,name)
		;			if ( attrib <= 0 ) return HANDLE_H5A_OPEN_NAME_ERR( name );

	mytype = H5Aget_type(attrib);	if ( mytype < 0 ) return HANDLE_H5A_GET_TYPE_ERR;

	space = H5Aget_space(attrib);	if ( space < 0 ) return HANDLE_H5A_GET_SPACE_ERR;

	nelem = H5Sget_simple_extent_npoints(space)
		;			if ( nelem < 0 ) return HANDLE_H5S_GET_SIMPLE_EXTENT_NPOINTS_ERR;

	type = _H5PartNormType ( mytype );

	r = H5Aread(attrib,type,value);	if ( r < 0 ) return HANDLE_H5A_READ_ERR;

	r = H5Sclose(space);		if ( r < 0 ) return HANDLE_H5S_CLOSE_ERR;
	r = H5Tclose(mytype);		if ( r < 0 ) return HANDLE_H5T_CLOSE_ERR;
	r = H5Aclose(attrib);		if ( r < 0 ) return HANDLE_H5A_CLOSE_ERR;

	return H5PART_SUCCESS;
}


/*================== File Reading Routines =================*/
/*
  H5PartSetStep:


  So you use this to random-access the file for a particular timestep.
  Failure to explicitly set the timestep on each read will leave you
  stuck on the same timestep for *all* of your reads.  That is to say
  the writes auto-advance the file pointer, but the reads do not
  (they require explicit advancing by selecting a particular timestep).
*/

/*!
  Set the current time-step.

  When writing data to a file the current time step must be set first
  (even if there is only one). In write-mode this function creates a new
  time-step! You are not allowed to step to an already existing time-step.
  This prevents you from overwriting existing data. Another consequence is,
  that you \b must write all data before going to the next time-step.

  In read-mode you can use this function to random-access the file for a
  particular timestep.

  \return \c H5PART_SUCCESS or error code 
*/

h5part_int64_t
H5PartSetStep (
	H5PartFile *f,			/*!< [in]  Handle to open file */
	h5part_int64_t step		/*!< [in]  Time-step to set. */
	) {

	static char *__funcname = "H5PartSetStep";
	herr_t r;

	char name[128];

	CHECK_FILEHANDLE ( f );

	print_info ( "%s: Proc[%d]: Set step to #%d for file %d\n",
			__funcname, f->myproc, step, (int) f->file );

	sprintf ( name, "Particles#%ld", step );
	r = H5Gget_objinfo( f->file, name, 1, NULL );
	if ( ( (f->mode == H5PART_APPEND) || (f->mode == H5PART_WRITE) )
	     && ( r >= 0 ) ) {
		return HANDLE_H5PART_STEP_EXISTS_ERR ( step );
	}

	if(f->timegroup>0) {
		r = H5Gclose(f->timegroup); if ( r < 0 ) HANDLE_H5G_CLOSE_ERR;
	}
	f->timegroup = -1;
	f->timestep = step;

	if(f->mode==H5PART_READ) {
		f->timegroup = H5Gopen(f->file,name); 
		if ( f->timegroup < 0 ) return HANDLE_H5G_OPEN_ERR( name );
	}
	else {
		print_debug ( "%s: Proc[%d]: create step #%d", 
				__funcname, f->myproc, step );

		f->timegroup = H5Gcreate(f->file,name,0);
		if ( f->timegroup < 0 ) return HANDLE_H5G_CREATE_ERR ( name );
	}
#if 0
#ifdef PARALLEL_IO
	H5PartWriteStepAttrib(f,"pnparticles",H5T_NATIVE_INT64,f->pnparticles,f->nprocs);
#endif
#endif
	return H5PART_SUCCESS;
}

/*!
  \internal

  This is an entirely internal callback function 
  which is used in conjunction with HDF5 iterators.  The HDF5 Group
  iterator will call this repeatedly in order to count how many
  timesteps of data have been stored in a particular file.
  This is used by H5PartGetNumSteps().

  \result        Allways \c 0
*/
static herr_t
_H5PartIOcounter (
	hid_t group_id,			/*!< [in]  group id */
	const char *member_name,	/*!< [in]  group name */
	void *operator_data		/*!< [in,out] data passed to the iterator */
	) {

	int *count = (int*)operator_data;

	/* only count the particle groups... ignore all others */
	if(!strncmp(member_name,"Particles",9)) (*count)++;

	return 0;
}

/*!
  \internal

  Same as \c _H5PartIOcounter but to count the datasets in the current
  time-step 

  \result        Allways \c 0
*/
static herr_t
_H5PartDScounter (
	hid_t group_id,			/*!< [in]  group id */
	const char *member_name,	/*!< [in]  group name */
	void *operator_data		/*!< [in,out] data passed to the iterator */
	) {

	int *count = (int*)operator_data;
	H5G_stat_t objinfo;
	/* only count the datasets... ignore all others */
	if(H5Gget_objinfo(group_id, 
			  member_name,
			  1 /* follow links */, 
			  &objinfo)<0) {
		return 0; /* error (probably bad symlink) */
	}
	if(objinfo.type==H5G_DATASET)    (*count)++;

	return 0;
}

/**
	H5IO_getname_t: Private
*/
typedef struct H5IO_getname_t {
	int indx;
	int count;
	char *name;
} H5IO_getname_t;

/*!
  \internal

  Operator for \c H5Giterate(). 

  \result        1 if we reached searched dataset
                 0 if we have to iterate further
		 < 0 on errors
*/
static herr_t
_H5IOgetname (
	hid_t group_id,			/*!< [in]  group id */
	const char *member_name,	/*!< [in]  group name */
	void *operator_data		/*!< [in,out] data passed to the iterator */
	) {

	static char *__funcname = "_H5IOgetname";
	herr_t r;


	H5IO_getname_t *getn = (H5IO_getname_t*)operator_data;
	H5G_stat_t objinfo;

	/* request info about the type of objects in root group, follow links */
	r = H5Gget_objinfo ( group_id, member_name, 1, &objinfo );
	if ( r < 0 ) return HANDLE_H5G_GET_OBJINFO_ERR ( member_name );

	/* only count objects that are datasets (not subgroups) */
	if ( objinfo.type != H5G_DATASET )
		return 0; /* do not increment count if it isn't a dataset. */

	if ( getn->indx == getn->count ){
		strcpy(getn->name, member_name);
		return 1;
	}
	getn->count++;
	return 0;
}

/*!
  Get the number of time-steps that are currently stored in the file
  \c f.

  It works for both reading and writing of files, but is probably
  only typically used when you are reading.

  \return	number of time-steps or error code

  \note
  There are two bugs in this function.

  \par
  There is no code to check whether the dataset name fits into the buffer \c name or not.
  The parameter \c maxlen is not used.

  \par
  There is no error handling and the iteration will continue endless, if there one.
*/
h5part_int64_t
H5PartGetNumSteps (
	H5PartFile *f			/*!< [in]  Handle to open file */
	) {

	static char *__funcname = "H5PartGetNumSteps";
	int count=0;
	int idx=0;

	CHECK_FILEHANDLE( f );

	/* iterate to get numsteps */
	while ( H5Giterate ( f->file, "/", &idx, _H5PartIOcounter, &count) < 0) {}

	return count;
}

/*!
  Get the number of datasets that are stored at the current time-step.

  \return	number of datasets in current timestep or error code

  \note
  There are two bugs in this function.

  \par
  There is no code to check whether the dataset name fits into the buffer \c name or not.
  The parameter \c maxlen is not used.

  \par
  There is no error handling and the iteration will continue endless, if there one.
*/

h5part_int64_t
H5PartGetNumDatasets (
	H5PartFile *f			/*!< [in]  Handle to open file */
	) {

	static char *__funcname = "H5PartGetNumDatasets";
	int count=0;
	int idx=0;
	char name[128];

	CHECK_FILEHANDLE( f );

	/* we need to have scanned file to get min timestep
	   before we call this ???  */
	sprintf ( name, "Particles#%d", f->timestep );

	while ( H5Giterate(f->file, name, &idx, _H5PartDScounter, &count) < 0 ) {}

	return count;   
}

/*!
  This reads the name of a dataset specified by it's index in the current time-step.

  If the number of datasets is \c n, the range of \c _index is \c 0 to \c n-1.

  \note
  There is currently no error handling!

  \result	\c H5PART_SUCCESS

  \note
  There are two bugs in this function.

  \par
  There is no code to check whether the dataset name fits into the buffer \c name or not.
  The parameter \c maxlen is not used.

  \par
  There is no error handling and the iteration will continue endless, if there one.
*/
h5part_int64_t
H5PartGetDatasetName (
	H5PartFile *f,			/*!< [in]  Handle to open file */
	int _index,			/*!< [in]  Index of the dataset */
	char *name,			/*!< [out] Name of dataset */
	size_t maxlen			/*!< [in]  Size of buffer \c name */
	) {

	static char *__funcname = "H5PartGetDatasetName";
	H5IO_getname_t getn;
	int idx = _index;
	char gname[128];
	sprintf(gname,"Particles#%d",f->timestep);
	getn.indx = _index;
	getn.name = name;
	getn.count = _index;

	CHECK_FILEHANDLE( f );

	/* if an error occures, we are sitting in an endless loop ... */
	while( H5Giterate(
		       f->file,
		       gname,
		       &idx,
		       _H5IOgetname,
		       &getn) < 0 ){}
	return H5PART_SUCCESS;
}

/*!
  Gets the name, type and number of elements of a dataset specified by it's index 
  in the current time-step.

  Type is one of \c H5T_NATIVE_DOUBLE or \c H5T_NATIVE_INT64.

  \return	\c H5PART_SUCCESS

  \note
  There are two bugs in this function.

  \par
  There is no code to check whether the dataset name fits into the buffer \c name or not.
  The parameter \c maxlen is not used.

  \par
  There is no error handling and the iteration will continue endless, if there one.
*/
h5part_int64_t
H5PartGetDatasetInfo (
	H5PartFile *f,			/*!< [in]  Handle to open file */
	int _index,			/*!< [in]  Index of the dataset */
	char *name,			/*!< [out] Name of dataset */
	size_t maxlen,			/*!< [in]  Size of buffer \c name */
	hid_t *type,			/*!< [out] Type of data in dataset */
	h5part_int64_t *nelem		/*!< [out] Number of elements of type \c type */
	) {

	static char *__funcname = "H5PartGetDatasetInfo";
	herr_t r;

	H5IO_getname_t getn;
	int idx=_index;
	char gname[128];
	hid_t dataset_id;
	hid_t mytype;

	CHECK_FILEHANDLE( f );
	
	sprintf(gname,"Particles#%d", f->timestep);
	getn.indx = _index;
	getn.name = name;
	getn.count=_index;

	while( H5Giterate( 
		       f->file,
		       gname,
		       &idx,
		       _H5IOgetname,
		       &getn ) < 0 ){}

	*nelem = H5PartGetNumParticles(f);

	if(!f->timegroup) {
		r = H5PartSetStep(f,f->timestep); /* choose current step */
		if ( r < 0 ) return HANDLE_H5PART_SETSTEP_ERR ( r, f->timestep );
	}	
	dataset_id = H5Dopen ( f->timegroup, name );
	if ( dataset_id < 0 ) HANDLE_H5D_OPEN_ERR ( name );

	mytype = H5Dget_type (dataset_id);
	if ( mytype < 0 ) HANDLE_H5D_GET_TYPE_ERR;

	if(type)
		*type = _H5PartNormType ( mytype );

	r = H5Tclose(mytype);		if ( r < 0 ) HANDLE_H5T_CLOSE_ERR;
	r = H5Dclose(dataset_id);	if ( r < 0 ) HANDLE_H5D_CLOSE_ERR;

	return H5PART_SUCCESS;
}

/*!
  \intenal 

*/
static hid_t
_H5PartGetDiskShape (
	H5PartFile *f,
	hid_t dataset
	) {

	static char *__funcname = "_H5PartGetDiskShape";
	herr_t r;

	CHECK_FILEHANDLE( f );

	hid_t space = H5Dget_space(dataset);
	if ( space < 0 ) return HANDLE_H5D_GET_SPACE_ERR;

	if ( H5PartHasView(f) ){ 
		hsize_t stride;
		hsize_t count;
#ifdef HDF5V160
		hssize_t range[2];
#else
		hsize_t range[2];
#endif
		print_debug ( "%s: Selection is available", __funcname );

		/* so, is this selection inclusive or exclusive? */
		range[0]=f->viewstart;
		range[1]=f->viewend;
		count = range[1]-range[0]; /* to be inclusive */
		stride=1;

		/* now we select a subset */
		if(f->diskshape>0) {
			r = H5Sselect_hyperslab(f->diskshape,H5S_SELECT_SET,
						range /* only using first element */,
						&stride,&count,NULL);
			if ( r < 0 ) return HANDLE_H5S_SELECT_HYPERSLAB_ERR;
		}
		/* now we select a subset */
		r = H5Sselect_hyperslab(space,H5S_SELECT_SET,
					range,&stride,&count,NULL);
		if ( r < 0 ) return HANDLE_H5S_SELECT_HYPERSLAB_ERR;

		print_debug ( "%s: Selection: range=%d:%d, npoints=%d s=%d\n",
				__funcname,
				(int)range[0],(int)range[1],
				(int)H5Sget_simple_extent_npoints(space),
				(int)H5Sget_select_npoints(space) );
		}	      
	else {
		print_debug ( "%s: Selection", __funcname );
	}
	return space;
}

static hid_t
_H5PartGetMemShape (
	H5PartFile *f,
	hid_t dataset
	) {

	static char *__funcname = "_H5PartGetMemShape";
	hid_t r;

	CHECK_FILEHANDLE( f );
 
	print_debug ( "%s: ", __funcname );

	if(H5PartHasView(f)) {
		hsize_t dmax=H5S_UNLIMITED;
		hsize_t len = f->viewend - f->viewstart;
		r = H5Screate_simple(1,&len,&dmax);
		if ( r < 0 ) return HANDLE_H5S_CREATE_SIMPLE_ERR ( len );
		return r;
	}
	else {
		return H5S_ALL;
	}
}

/*!
  H5PartGetFirstDS: Private
*/
static herr_t
_H5PartGetFirstDS (
	hid_t group_id,
	const char *member_name,
	void *operator_data
	) {

	static char *__funcname = "_H5PartGetFirstDS";

	hid_t *dataset = (hid_t*)operator_data;
	H5G_stat_t objinfo;
	/* only count the particle groups... ignore all others */
	if(H5Gget_objinfo(group_id, 
			  member_name,
			  1 /* follow links */, 
			  &objinfo)<0) {
		return 0; /* error (probably bad symlink) */
	}
	if(objinfo.type==H5G_DATASET){
		(*dataset) = H5Dopen(group_id,member_name);
		if ( *dataset < 0 ) return HANDLE_H5D_OPEN_ERR ( member_name );
		return 1; /* all done: return success */
	}
	return 0; /* causes iterator to continue to next item */
}

/*!
  This gets the number of particles stored in the current timestep. 
  It will arbitrarily select a time-step if you haven't already set
  the timestep with \c H5PartSetStep().

  \return	number of particles in current timestep or an error
		code.
 */
h5part_int64_t
H5PartGetNumParticles (
	H5PartFile *f			/*!< [in]  Handle to open file */
	) {

	static char *__funcname = "H5PartGetNumParticles";
	hid_t r;

	hid_t space,dataset;
	hsize_t nparticles;
	int idx=0;
	char name[128];

	CHECK_FILEHANDLE( f );

	if(f->timegroup<0) {
		int step = (f->timestep<0 ? 0 : f->timestep );
		h5part_int64_t rc = H5PartSetStep ( f, step );
		if ( rc < 0 ) return HANDLE_H5PART_SETSTEP_ERR ( rc, step );
	}

	/* Get first dataset in current time-step */
	sprintf(name,"Particles#%d",f->timestep);
	while ( H5Giterate(f->file,
			   name,
			   &idx,
			   _H5PartGetFirstDS,
			   &dataset) < 0 ) {}

	/* need to use H5PartGetDiskShape for any changes to f->diskshape */
	space = _H5PartGetDiskShape(f,dataset);
	if ( space < 0 ) return HANDLE_H5PART_GET_DISKSHAPE_ERR ( space );


	if ( H5PartHasView ( f ) ) {
		print_debug ( "%s: H5Sget_select_npoints");
		nparticles = H5Sget_select_npoints(space);
		if ( nparticles < 0 ) return HANDLE_H5S_GET_SELECT_NPOINTS_ERR;
	}
	else {
		print_debug ( "%s: H5Sget_simple_extent_npoints");
		nparticles = H5Sget_simple_extent_npoints(space);
		if ( nparticles < 0 ) return HANDLE_H5S_GET_SIMPLE_EXTENT_NPOINTS_ERR;
	}
	if ( space != H5S_ALL ) {
		r = H5Sclose(space);
		if ( r < 0 ) return HANDLE_H5S_CLOSE_ERR;
	}
	r = H5Dclose(dataset);
	if ( r < 0 ) return HANDLE_H5D_CLOSE_ERR;

	return (h5part_int64_t) nparticles;
}


/*!
  For parallel I/O or for subsetting operations on the datafile, the
  \c H5PartSetView() function allows you to define a subset of the total particle
  dataset to read.  The concept of "view" works for both serial and for
  parallel I/O.  The "view" will remain in effect until a new view is set,
  or the number of particles in a dataset changes, or the view is "unset"
  by calling \c H5PartSetView(file,-1,-1);

  Before you set a view, the \c H5PartGetNumParticles() will return the
  total number of particles in the current time-step (even for the parallel
  reads).  However, after you set a view, it will return the number of
  particles contained in the view.

  The range is inclusive (the start and the end index).

  \return	\c H5PART_SUCCESS or error code
*/
h5part_int64_t
H5PartSetView (
	H5PartFile *f,			/*!< [in]  Handle to open file */
	h5part_int64_t start,		/*!< [in]  Start particle */
	h5part_int64_t end		/*!< [in]  End particle */
	) {

	static char *__funcname = "H5PartSetView";

	h5part_int64_t total;
	hsize_t stride;
	hsize_t dmax=H5S_UNLIMITED;

#ifdef HDF5V160
	hssize_t range[2];
#else
	hsize_t range[2];
#endif
	int r;

	CHECK_FILEHANDLE( f );

	if(f->mode==H5PART_WRITE || f->mode==H5PART_APPEND)
		return HANDLE_H5PART_SET_VIEW_FILE_ACCESS_TYPE_ERR;

	print_debug ( "%s: Set view (%d,%d).", __funcname, (int)start,(int)end);

	/* if there is already a view selected, lets destroy it */ 
	f->viewstart = -1;
	f->viewend = -1;
	if ( f->shape != 0 ){
		r = H5Sclose(f->shape);
		if ( r < 0 ) return HANDLE_H5S_CLOSE_ERR;
		f->shape=0;
	}
	if(f->diskshape!=0 && f->diskshape!=H5S_ALL){
		r = H5Sclose(f->diskshape);
		if ( r < 0 ) return HANDLE_H5S_CLOSE_ERR;
		f->diskshape=H5S_ALL;
	}
	f->diskshape = H5S_ALL;
	if(f->memshape!=0 && f->memshape!=H5S_ALL){
		H5Sclose(f->memshape);
		if ( r < 0 ) return HANDLE_H5S_CLOSE_ERR;
		f->memshape=H5S_ALL;
	}
	if(start==-1 && end==-1) {
		print_debug( "%s: Early Termination: Unsetting View", __funcname );
		return H5PART_SUCCESS; /* all done */
	}
	/* for now, we interpret start=-1 to mean 0 and 
	   end==-1 to mean end of file */
	total = H5PartGetNumParticles(f);
	if ( total < 0 ) return HANDLE_H5PART_GET_NUM_PARTICLES_ERR ( total );

	print_debug ( "%s: Total nparticles=%d", __funcname, (int)total );
	if(start==-1) start=0;
	if(end==-1) end=total; /* can we trust nparticles (no)? 
				  fortunately, view has been reset
				  so H5PartGetNumParticles will tell
				  us the total number of particles. */

	/* so, is this selection inclusive or exclusive? 
	   it appears to be inclusive for both ends of the range.
	*/
	if(end<start) {
		print_warn ( "%s: Nonfatal error. End of view (%d) is less than start (%d).",
			       __funcname, (int)end,(int)start);
		end=start; /* ensure that we don't have a range error */
	}
	range[0]=start;
	range[1]=end;
	/* setting up the new view */
	f->viewstart=range[0]; /* inclusive start */
	f->viewend=range[1]; /* inclusive end */
	f->nparticles=range[1]-range[0];
	print_debug ( "%s: Range is now %d:%d\n", __funcname, (int)range[0], (int)range[1]);
	/* OK, now we must create a selection from this */
	
	/* declare overall datasize */
	f->shape = H5Screate_simple(1,(hsize_t*)&total, (hsize_t*)&total);
	if ( f->shape < 0 ) return HANDLE_H5S_CREATE_SIMPLE_ERR ( (hsize_t)total );

	/* declare overall data size  but then will select a subset */
	f->diskshape= H5Screate_simple(1, (hsize_t*)&total, (hsize_t*)&total);
	if ( f->diskshape < 0 ) return HANDLE_H5S_CREATE_SIMPLE_ERR ( (hsize_t) total );

	/* declare local memory datasize */
	f->memshape = H5Screate_simple(1,&(f->nparticles),&dmax);
	if ( f->diskshape < 0 ) return HANDLE_H5S_CREATE_SIMPLE_ERR ( f->nparticles );;

	stride=1;
	r = H5Sselect_hyperslab(f->diskshape,H5S_SELECT_SET,range,&stride, (hsize_t*)&total,NULL);
	if ( r < 0 ) return HANDLE_H5S_SELECT_HYPERSLAB_ERR;

	/* OK, now we have selected a reasonable hyperslab (all done) */
	return H5PART_SUCCESS;
}

/*!
   Allows you to query the current view. Start and End
   will be \c -1 if there is no current view established.
   Use \c H5PartHasView() to see if the view is smaller than the
   total dataset.

   \return       the number of elements in the view 
*/
h5part_int64_t
H5PartGetView (
	H5PartFile *f,			/*!< [in]  Handle to open file */
	h5part_int64_t *start,		/*!< [out]  Start particle */
	h5part_int64_t *end		/*!< [out]  End particle */
	) {

	static char *__funcname = "H5PartGetView";

	h5part_int64_t range[2];
	h5part_int64_t viewend;

	CHECK_FILEHANDLE( f );

	range[0] = (f->viewstart>=0) ? f->viewstart : 0;

	if ( f->viewend >= 0 ) {
		viewend = f->viewend;
	}
	else {
		viewend = H5PartGetNumParticles(f);
		if ( viewend < 0 ) return HANDLE_H5PART_GET_NUM_PARTICLES_ERR ( viewend );
	}

	range[1] = viewend;
	if(start) {
		*start=range[0];
	}
	if(end) {
		*end=range[1];
	}
	/* we could return the number of elements in the View as a convenience */
	return range[1]-range[0];
}

/*!
  If it is too tedious to manually set the start and end coordinates
  for a view, the \c H5SetCanonicalView() will automatically select an
  appropriate domain decomposition of the data arrays for the degree
  of parallelism and set the "view" accordingly.

  \return		H5PART_SUCCESS or error code
*/
h5part_int64_t
H5PartSetCanonicalView (
	H5PartFile *f			/*!< [in]  Handle to open file */
	) {

	static char *__funcname = "H5PartSetCanonicalView";

	h5part_int64_t r;

	CHECK_FILEHANDLE( f );

	if(f->mode==H5PART_WRITE || f->mode==H5PART_APPEND)
		return HANDLE_H5PART_SET_VIEW_FILE_ACCESS_TYPE_ERR;

	/* if a read_only file, search for on-disk canonical view */
	/* if this view does not exist, then if MPI, subdivide by numprocs */
	/* else, "unset" any existing View */

	/* unset the view */
	r = H5PartSetView(f,-1,-1);
	if ( r < 0 ) return HANDLE_H5PART_SET_VIEW_ERR( r, -1, -1 );
#ifdef PARALLEL_IO
	{
		h5part_int64_t start = 0;
		h5part_int64_t end = 0;
		h5part_int64_t n = 0;
		int i = 0;
		
		if ( f->timegroup < 0 ) {
			/* set to first step in file */
			r = H5PartSetStep(f,0);
			if ( r < 0 ) return HANDLE_H5PART_SETSTEP_ERR ( r, 0 );
		}
		n = H5PartGetNumParticles(f);
		if ( n < 0 ) return HANDLE_H5PART_GET_NUM_PARTICLES_ERR ( n );
		/* 
		   now lets query the attributes for this group to see if there
		   is a 'pnparticles' group that contains the offsets for the
		   processors.
		*/
		/* try to read pnparticles right off of the disk */
		if(H5PartReadStepAttrib(f,"pnparticles",f->pnparticles) < 0) {
			/*
			  automatically subdivide the view into NP mostly
			  equal pieces
			*/

			n /= f->nprocs;
			for ( i=0; i<f->nprocs; i++ ) {
				f->pnparticles[i] = n;
			}
		}

		/* now we set the view for this processor */
		for ( i = 0; i < f->myproc; i++ ){
			start += f->pnparticles[i];
		}
		end = start + f->pnparticles[f->myproc] - 1;
		r = H5PartSetView ( f, start, end );
		if ( r < 0 ) return HANDLE_H5PART_SET_VIEW_ERR ( r, start, end );
	}
#endif
	/* the canonical view is to see everything if this is serial
	   so there is nothing left to do */
	return H5PART_SUCCESS;
}

/*!
  Read array of 64 bit floating point data from file.

  When retrieving datasets from disk, you ask for them
  by name. There are no restrictions on naming of arrays,
  but it is useful to arrive at some common naming
  convention when sharing data with other groups.

  \return	\c H5PART_SUCCESS or error code
*/
h5part_int64_t
H5PartReadDataFloat64 (
	H5PartFile *f,			/*!< [in] Handle to open file */
	char *name,			/*!< [in] Name to associate array/dataset with */
	h5part_float64_t *array		/*!< [out] Array for data */
	) {

	static char *__funcname = "H5PartReadDataFloat64";
	h5part_int64_t r;

	hid_t space;
	hid_t memspace;
	hid_t dataset;

	CHECK_FILEHANDLE( f );

	if(!f->timegroup) {
		r = H5PartSetStep(f,f->timestep);
		if ( r < 0 ) return HANDLE_H5PART_SETSTEP_ERR ( r, f->timestep );
	}
	dataset = H5Dopen(f->timegroup,name);
	if ( dataset < 0 ) return HANDLE_H5D_OPEN_ERR ( name );
	/* gets space with selection if view is set */
	space = _H5PartGetDiskShape(f,dataset);
	if ( space < 0 ) return HANDLE_H5PART_GET_DISKSHAPE_ERR ( space );

	memspace = _H5PartGetMemShape(f,dataset);
	if ( memspace < 0 ) return HANDLE_H5PART_GET_MEMSHAPE_ERR ( memspace );

	/*  datatype=H5Dget_type(dataset); */
	r = (h5part_int64_t)H5Dread(
		dataset,		/* handle for the dataset */
		H5T_NATIVE_DOUBLE,	/* the datatype we use in memory 
					   you can change it to FLOAT if you
					   want */
		memspace,		/* shape/size of data in memory (the
					   complement to disk hyperslab) */
		space,			/* shape/size of data on disk 
					   (get hyperslab if needed) */
		H5P_DEFAULT,		/* ignore... its for parallel reads */
		array );		/* the data array we are reading into */
	if ( r < 0 ) return HANDLE_H5D_READ_ERR ( name, f->timestep );
	if(space!=H5S_ALL) {
		r = H5Sclose(space);
		if ( r < 0 ) return HANDLE_H5S_CLOSE_ERR;
	}
	if(memspace!=H5S_ALL)
		r = H5Sclose(memspace);
		if ( r < 0 ) return HANDLE_H5S_CLOSE_ERR;
	r = H5Dclose(dataset);
	if ( r < 0 ) return HANDLE_H5D_CLOSE_ERR;

	return H5PART_SUCCESS;
}

/**
  Read array of 64 bit floating point data from file.

  When retrieving datasets from disk, you ask for them
  by name. There are no restrictions on naming of arrays,
  but it is useful to arrive at some common naming
  convention when sharing data with other groups.

  \return	\c H5PART_SUCCESS or error code
*/
h5part_int64_t
H5PartReadDataInt64 (
	H5PartFile *f,			/*!< [in] Handle to open file */
	char *name,			/*!< [in] Name to associate array/dataset with */
	h5part_int64_t *array		/*!< [out] Array for data */
	) {
	hid_t space;
	hid_t memspace;
	hid_t dataset;

	static char *__funcname = "H5PartReadDataInt64";
	h5part_int64_t r;

	CHECK_FILEHANDLE( f );

	if(!f->timegroup) {
		r = H5PartSetStep(f,f->timestep);
		if ( r < 0 ) return HANDLE_H5PART_SETSTEP_ERR ( r, f->timestep );
	}
	dataset = H5Dopen(f->timegroup,name);
	if ( dataset < 0 ) return HANDLE_H5D_OPEN_ERR ( name );
	/* gets space with selection if view is set */
	space = _H5PartGetDiskShape(f,dataset);
	if ( space < 0 ) return HANDLE_H5PART_GET_DISKSHAPE_ERR ( space );

	memspace = _H5PartGetMemShape(f,dataset);
	if ( memspace < 0 ) return HANDLE_H5PART_GET_MEMSHAPE_ERR ( memspace );

	r = (h5part_int64_t)H5Dread( dataset,
				   H5T_NATIVE_INT64,
				   memspace,
				   space,
				   H5P_DEFAULT,
				   array);
	if ( r < 0 ) return HANDLE_H5D_READ_ERR ( name, f->timestep );
	if(space!=H5S_ALL) {
		r = H5Sclose(space);
		if ( r < 0 ) return HANDLE_H5S_CLOSE_ERR;
	}
	if(memspace!=H5S_ALL)
		r = H5Sclose(memspace);
		if ( r < 0 ) return HANDLE_H5S_CLOSE_ERR;
	r = H5Dclose(dataset);
	if ( r < 0 ) return HANDLE_H5D_CLOSE_ERR;

	return H5PART_SUCCESS;
}

/*!
  This is the mongo read function that pulls in all of the data for a
  given timestep in one shot. It also takes the timestep as an argument
  and will call \c H5PartSetStep() internally so that you don't have to 
  make that call separately.

  \note
  See also \c H5PartReadDataInt64() and \c H5PartReadDataFloat64() if you want
  to just read in one of the many datasets.

  \return	\c H5PART_SUCCESS or error code
*/
h5part_int64_t
H5PartReadParticleStep (
	H5PartFile *f,			/*!< [in]  Handle to open file */
	h5part_int64_t step,		/*!< [in]  Step to read */
	h5part_float64_t *x,		/*!< [out] Buffer for dataset named "x" */
	h5part_float64_t *y,		/*!< [out] Buffer for dataset named "y" */
	h5part_float64_t *z,		/*!< [out] Buffer for dataset named "z" */
	h5part_float64_t *px,		/*!< [out] Buffer for dataset named "px" */
	h5part_float64_t *py,		/*!< [out] Buffer for dataset named "py" */
	h5part_float64_t *pz,		/*!< [out] Buffer for dataset named "pz" */
	h5part_int64_t *id		/*!< [out] Buffer for dataset named "id" */
	) {

	static char *__funcname = "H5PartReadParticleStep";
	h5part_int64_t r;

	CHECK_FILEHANDLE( f );

	r = H5PartSetStep(f,step);
	if ( r < 0 ) return HANDLE_H5PART_SETSTEP_ERR ( r, step );

	r = H5PartReadDataFloat64(f,"x",x);
	if ( r < 0 ) return HANDLE_H5PART_READDATAFLOAT64_ERR ( r, "x", step );
	r = H5PartReadDataFloat64(f,"y",y);
	if ( r < 0 ) return HANDLE_H5PART_READDATAFLOAT64_ERR ( r, "y", step );
	r = H5PartReadDataFloat64(f,"z",z);
	if ( r < 0 ) return HANDLE_H5PART_READDATAFLOAT64_ERR ( r, "z", step );
	r = H5PartReadDataFloat64(f,"px",px);
	if ( r < 0 ) return HANDLE_H5PART_READDATAFLOAT64_ERR ( r, "px", step );
	r = H5PartReadDataFloat64(f,"py",py);
	if ( r < 0 ) return HANDLE_H5PART_READDATAFLOAT64_ERR ( r, "py", step );
	r = H5PartReadDataFloat64(f,"pz",pz);
	if ( r < 0 ) return HANDLE_H5PART_READDATAFLOAT64_ERR ( r, "pz", step );
	r = H5PartReadDataInt64(f,"id",id);
	if ( r < 0 ) return HANDLE_H5PART_READDATAINT64_ERR ( r, "id", step );

	return H5PART_SUCCESS;
}

/**************** File Stashing Interfaces *************************/
/**
	H5NameExists: Private
*/

static herr_t
_H5NameExists (
	hid_t group_id,
	const char *member_name,
	void *v
	) {

	if(!strcmp(member_name,(char*)v))
		return 1;
	else
		return 0;
}

/**
	H5PartFileHasName: Private
*/
static h5part_int64_t
_H5PartFileHasName (
	H5PartFile *f,
	char *dir,
	char *name
	) {
	if(H5Giterate(f->file,
		      dir,
		      NULL,
		      _H5NameExists,
		      (void*)name) < 0 )
		return 1;
	else
		return 0;
}

/**
	NOTE: it's missing the read!
  H5PartStashFile: stash a file in the hdf5 file. If it does not exists a UserData
	and a Files group will be created.
  /param f A FileHandle opened by H5PartOpenFile() or H5PartOpenFileParallel().
  /param filename A null-terminated string for the name of the filename. 
  /return 1 on succes, 0 on failure.

*/
h5part_int64_t
H5PartStashFile (
	H5PartFile *f,
	char *filename
	) {

	hid_t udata=0,files=0,rgroup=0;
	FILE *file;
	int returnvalue=0;
	
	rgroup = H5Gopen(f->file,"/");
	if(_H5PartFileHasName(f,"/","UserData")) {
		udata = H5Gopen(rgroup,"UserData");
		if(_H5PartFileHasName(f,"UserData","Files")){
			files=H5Gopen(udata,"Files");
		}
		else {
			files = H5Gcreate(udata,"Files",0);
		}
	}
	else {
		/* must create the UserData group */
		udata = H5Gcreate(rgroup,"UserData",0);
		files = H5Gcreate(udata,"Files",0);
	}
	if(rgroup) H5Gclose(rgroup); rgroup=0;
	if(udata) H5Gclose(udata); udata=0;
	/* now we stash the file into the files subdir */
	/* first make sure there isn't a file with the same
	   name already there? */
	file = fopen(filename,"r");
	if(file){
		hsize_t sz;
		char *buffer,*dsname,*dsbuffer;
		hid_t fspace,fdata;
		fseek(file,0,SEEK_END);
		sz = ftell(file);
		buffer=(char*)malloc(sz);
		fspace=H5Screate_simple(1,&sz,0);
		/* need to strip off the /'s from the filename before creating dataset */
		dsbuffer = (char*)malloc(strlen(filename));
		strcpy(dsbuffer,filename);
		dsname = strrchr(dsbuffer,'/');
		if(!dsname) dsname=dsbuffer;
		fdata=H5Dcreate(files,dsname,H5T_NATIVE_CHAR,fspace,H5P_DEFAULT);
		H5Dwrite(fdata,H5T_NATIVE_CHAR,H5S_ALL,H5S_ALL,H5P_DEFAULT,buffer);
		H5Dclose(fdata);
		H5Sclose(fspace);
		fclose(file);
		free(buffer); free(dsbuffer);
		returnvalue = 1; /* success */
	}
	H5Gclose(files);
	return returnvalue;
}

/**
  H5PartUnstashFile: unstash a file in the hdf5 file.

  /param f A FileHandle opened by H5PartOpenFile() or H5PartOpenFileParallel().

  /param filename A null-terminated string for the name of the filename. 

  /param The path to the directory to create the file.

  /return 1 on succes, 0 on failure.

*/

h5part_int64_t
H5PartUnstashFile (
	H5PartFile *f,
	char *filename,
	char *outputpath
	) {

	if( _H5PartFileHasName(f,"/","UserData")
	    && _H5PartFileHasName(f,"/UserData","Files")
	    && _H5PartFileHasName(f,"/UserData/Files",filename)) {
		/* extract the datafile from the HDF5 file */
		hid_t fdata,fspace,fgroup;
		hsize_t sz;
		char *buffer,*outname;
		FILE *file;
		fgroup = H5Gopen(f->file,"/UserData/Files");
		fdata = H5Dopen(fgroup,filename);
		fspace = H5Dget_space(fdata);
		sz = H5Sget_simple_extent_npoints(fspace);
		buffer = (char*)malloc(sz);
		H5Dread(fdata,H5T_NATIVE_CHAR,H5S_ALL,H5S_ALL,H5P_DEFAULT,buffer);
		outname = (char*)malloc(strlen(filename) + 8 + outputpath?strlen(outputpath):0);
		if(outputpath && strlen(outputpath)>0){
			if(outputpath[strlen(outputpath)-1]!='/') sprintf(outname,"%s/%s",outputpath,filename);
			else sprintf(outname,"%s%s",outputpath,filename);
		}
		else {
			strcpy(outname,filename);
		}
		file = fopen(outname,"w");
		fwrite(buffer,1,sz,file);
		fclose(file);
		free(buffer);
		free(outname);
		H5Sclose(fspace);
		H5Dclose(fdata);
		H5Gclose(fgroup);
		return 1;
	}
	return 0;
}

/**
   H5PartGetNumStashFiles: returns the number of stashed files.

   \param f A FileHandle opened by H5PartOpenFile() or H5PartOpenFileParallel().

   \return Number of stashed files.
*/

h5part_int64_t
H5PartGetNumStashFiles (
	H5PartFile *f
	) {
	
	hsize_t retval;
	if(_H5PartFileHasName(f,"/","UserData")
	   && _H5PartFileHasName(f,"/UserData","Files")){
		hid_t fgroup;
		/* we will use an iterator for this to count stash files */
		fgroup = H5Gopen(f->file,"/UserData/Files");
		H5Gget_num_objs(fgroup,&retval);
		return retval;
	}
	else return 0;
}

h5part_int64_t
H5PartFileGetStashFileName (
	H5PartFile *f,
	int nameindex,
	char *filename,
	int maxnamelen ) {

	return 1;
}

/****************** error handling ******************/

h5part_int64_t
H5PartSetVerbosityLevel (
	unsigned int level
	) {

	_debug = level;
	return H5PART_SUCCESS;
}

h5part_int64_t
H5PartSetErrorHandler (
	h5part_error_handler handler
	) {
	_err_handler = handler;
	return H5PART_SUCCESS;
}

h5part_int64_t
H5PartGetErrno (
	void
	) {
	return _errno;
}

h5part_int64_t
H5PartDefaultErrorHandler (
	const char *funcname,
	const h5part_int64_t eno,
	const char *fmt,
	...
	) {

	_errno = eno;
	if ( _debug > 0 ) {
		va_list ap;
		va_start ( ap, fmt );
		fprintf ( stderr, "%s: ", funcname );
		vfprintf ( stderr, fmt, ap );
		fprintf ( stderr, "\n" );
	}
	return _errno;
}

h5part_int64_t
H5PartAbortErrorHandler (
	const char *funcname,
	const h5part_int64_t eno,
	const char *fmt,
	...
	) {

	_errno = eno;
	if ( _debug > 0 ) {
		va_list ap;
		va_start ( ap, fmt );
		fprintf ( stderr, "%s: ", funcname );
		vfprintf ( stderr, fmt, ap );
		fprintf ( stderr, "\n" );
	}
	exit (-_errno);
}

/********* More Private Function Definitions *************/

static h5part_int64_t
_H5Part_init ( void ) {
	static int __init = 0;

	herr_t r5;
	if ( ! __init ) {
		r5 = H5Eset_auto ( _H5Part_h5_error_handler, NULL );
		if ( r5 < 0 ) return H5PART_ERR_INIT;
	}
	__init = 1;
	return H5PART_SUCCESS;
}

static herr_t
_H5Part_h5_error_handler ( void* unused ) {
	
	if ( _debug >= 5 ) {
		H5Eprint (stderr);
	}
	return 0;
}

void
vprint_error (
	const char *fmt,
	va_list ap
	) {

	if ( _debug < 1 ) return;
	vfprintf ( stderr, fmt, ap );
	fprintf ( stderr, "\n" );
}

void
print_error (
	const char *fmt,
	...
	) {

	va_list ap;
	va_start ( ap, fmt );
	vprint_error ( fmt, ap );
	va_end ( ap );
}

void
vprint_warn (
	const char *fmt,
	va_list ap
	) {

	if ( _debug < 2 ) return;
	vfprintf ( stderr, fmt, ap );
	fprintf ( stderr, "\n" );
}

void
print_warn (
	const char *fmt,
	...
	) {

	va_list ap;
	va_start ( ap, fmt );
	vprint_warn ( fmt, ap );
	va_end ( ap );
}

void
vprint_info (
	const char *fmt,
	va_list ap
	) {

	if ( _debug < 3 ) return;
	vfprintf ( stdout, fmt, ap );
	fprintf ( stdout, "\n" );
}

void
print_info (
	const char *fmt,
	...
	) {

	va_list ap;
	va_start ( ap, fmt );
	vprint_info ( fmt, ap );
	va_end ( ap );
}

void
vprint_debug (
	const char *fmt,
	va_list ap
	) {

	if ( _debug < 4 ) return;
	vfprintf ( stdout, fmt, ap );
	fprintf ( stdout, "\n" );
}

void
print_debug (
	const char *fmt,
	...
	) {

	va_list ap;
	va_start ( ap, fmt );
	vprint_debug ( fmt, ap );
	va_end ( ap );
}
