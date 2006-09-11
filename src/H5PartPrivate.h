#ifndef _H5PARTPRIVATE_H_
#define _H5PARTPRIVATE_H_

/********** static function declarations **********/

static h5part_int64_t
_H5Part_init( void );

static h5part_int64_t
_H5PartFileIsValid (
	H5PartFile *f
	);


static herr_t
_H5PartIOcounter (
	hid_t group_id,
	const char *member_name,
	void *operator_data
	);

static hid_t
_H5PartNormType (
	hid_t type
	);

static herr_t
_H5Part_h5_error_handler (
	void *
	);


/***************** Error Handling ***************/

#define CHECK_FILEHANDLE( f ) \
	if ( _H5PartFileIsValid ( f ) != H5PART_SUCCESS ) \
		return (*_err_handler)( \
			__funcname, \
			H5PART_ERR_BADFD, \
			"Called with bad filehandle." );

#define CHECK_WRITABLE_MODE( f )  \
	if ( f->mode==H5PART_READ ) \
		return (*_err_handler) ( \
			__funcname, \
			H5PART_ERR_INVAL, \
			"Attempting to write to read-only file" );

#define CHECK_TIMEGROUP( f ) \
	if ( f->timegroup <= 0 ) \
		return (*_err_handler) ( \
			__funcname, \
			H5PART_ERR_INVAL, \
			"Timegroup <= 0.");

/**************** H5Part *********************/

#define HANDLE_H5PART_INIT_ERR \
        (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_INIT, \
		"Cannot initialize H5Part." );

#define HANDLE_H5PART_NOMEM_ERR \
	(*_err_handler) ( \
		__funcname, \
		H5PART_ERR_NOMEM, \
		"Out of memory." );

#define HANDLE_H5PART_READDATAFLOAT64_ERR( rc, name, step ) \
	(*_err_handler) ( \
		__funcname, \
		rc, \
		"Cannont read dataset \"%s\" in time-step %d.", name, step );

#define HANDLE_H5PART_READDATAINT64_ERR( rc, name, step ) \
	(*_err_handler) ( \
		__funcname, \
		rc, \
		"Cannont read dataset \"%s\" in time-step %d.", name, step );

#define HANDLE_H5PART_SETSTEP_ERR( rc, step ) \
	(*_err_handler) ( \
		__funcname, \
		rc, \
		"Cannont set time-step to %d.", step );

#define HANDLE_H5PART_FILE_ACCESS_TYPE_ERR( flags ) \
		(*_err_handler) ( \
			__funcname, \
			H5PART_ERR_INVAL, \
			"Invalid file access type \"%d\".", flags);

#define HANDLE_H5PART_STEP_EXISTS_ERR( step ) \
	(*_err_handler)( \
		__funcname, \
		H5PART_ERR_INVAL, \
		"Step #%d already exists, step cannot be set to an existing step in write and append mode", step );

#define HANDLE_H5PART_GET_DISKSHAPE_ERR( rc ) \
		(*_err_handler) ( \
			__funcname, \
			rc, \
			"Cannont get diskshape.");

#define HANDLE_H5PART_GET_MEMSHAPE_ERR( rc ) \
		(*_err_handler) ( \
			__funcname, \
			rc, \
			"Cannont get memshape.");


#define HANDLE_H5PART_SET_VIEW_FILE_ACCESS_TYPE_ERR \
		(*_err_handler) ( \
			__funcname, \
			H5PART_ERR_INVAL, \
			"Set view does not make sense for write-only files." \
			"It is meant to be used for read-only files. " \
			"(maybe later this will change)" );

#define HANDLE_H5PART_SET_VIEW_ERR( rc, start, end ) \
		(*_err_handler) ( \
			__funcname, \
			rc, \
			"Cannot set view to (%d, %d).", start, end );

#define HANDLE_H5PART_GET_NUM_PARTICLES_ERR( rc ) \
		(*_err_handler) ( \
			__funcname, \
			rc, \
			"Cannot get number of particles." );

/**************** HDF5 *********************/
/* H5A: Attribute */
#define HANDLE_H5A_CLOSE_ERR \
	 (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_HDF5, \
		"Cannot terminate access to attribute." );

#define HANDLE_H5A_CREATE_ERR( s ) \
	 (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_HDF5, \
		"Cannot create attribute \"%s\".", s );

#define HANDLE_H5A_GET_NAME_ERR \
	 (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_HDF5, \
		"Cannot get attribute name." );

#define HANDLE_H5A_GET_NUM_ATTRS_ERR \
	 (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_HDF5, \
		"Cannot get number of attributes." );

#define HANDLE_H5A_GET_SPACE_ERR \
	 (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_HDF5, \
		"Cannot get a copy of dataspace for attribute." );

#define HANDLE_H5A_GET_TYPE_ERR \
	 (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_HDF5, \
		"Cannot get attribute datatype." );

#define HANDLE_H5A_OPEN_IDX_ERR( n ) \
	 (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_HDF5, \
		"Cannot open attribute specified by index \"%d\".", n );

#define HANDLE_H5A_OPEN_NAME_ERR( s ) \
	 (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_HDF5, \
		"Cannot open attribute specified by name \"%d\".", s );

#define HANDLE_H5A_READ_ERR \
	 (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_HDF5, \
		"Cannot read attribute" );

#define HANDLE_H5A_WRITE_ERR( s ) \
	 (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_HDF5, \
		"Cannot write attribute \"%s\".", s );

/* H5D: Dataset */
#define HANDLE_H5D_CLOSE_ERR \
	 (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_HDF5, \
		"Close of dataset failed." );

#define HANDLE_H5D_CREATE_ERR( s, n ) \
	 (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_HDF5, \
		"Cannot create dataset for name \"%s\", step \"%d\".", s, n );

#define HANDLE_H5D_GET_SPACE_ERR \
	 (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_HDF5, \
		"Cannot get dataspace identifier.");

#define HANDLE_H5D_GET_TYPE_ERR \
	 (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_HDF5, \
		"Cannot determine dataset type.");

#define HANDLE_H5D_OPEN_ERR( s ) \
	 (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_HDF5, \
		"Cannot open dataset \"%s\".", s );

#define HANDLE_H5D_READ_ERR( s, n ) \
	 (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_HDF5, \
		"Read from dataset \"%s\" failed, step \"%d\".", s, n );

#define HANDLE_H5D_WRITE_ERR( s, n ) \
	 (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_HDF5, \
		"Write to dataset \"%s\" failed, step \"%d\".", s, n );

/* H5F: file */
#define HANDLE_H5F_CLOSE_ERR \
	 (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_HDF5, \
		"Cannot terminate access to file." );

#define HANDLE_H5F_OPEN_ERR( filename, flags ) \
		(*_err_handler) ( \
			__funcname, \
			H5PART_ERR_HDF5, \
			"Cannot open file \"%s\" with mode \"%d\"", filename, flags );



/* H5G: group */
#define HANDLE_H5G_CLOSE_ERR \
	 (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_HDF5, \
		"Cannot terminate access to datagroup." );

#define HANDLE_H5G_CREATE_ERR( s ) \
	 (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_HDF5, \
		"Cannot create datagroup \"%s\".", s );

#define HANDLE_H5G_GET_OBJINFO_ERR( s ) \
	 (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_HDF5, \
		"Cannot get information about object \"%s\".", s );

#define HANDLE_H5G_OPEN_ERR( s ) \
	 (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_HDF5, \
		"Cannot open group \"%s\".", s );


/* H5P: property */
#define HANDLE_H5P_CLOSE_ERR( s ) \
	 (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_HDF5, \
		"Cannot terminate access to property list \"%s\".", s );

#define HANDLE_H5P_CREATE_ERR \
	 (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_HDF5, \
		"Cannot create property list." );

#define HANDLE_H5P_SET_DXPL_MPIO_ERR \
		(*_err_handler) ( \
			__funcname, \
			H5PART_ERR_HDF5, \
			"MPI: Cannot set data transfer mode." );


#define HANDLE_H5P_SET_FAPL_MPIO_ERR \
		(*_err_handler) ( \
			__funcname, \
			H5PART_ERR_HDF5, \
			"Cannot store IO communicator information to the " \
			"file access property list.");

/* H5S: dataspace */
#define HANDLE_H5S_CREATE_SIMPLE_ERR( n ) \
	 (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_HDF5, \
		"Cannot create dataspace with len \"%d\".", n );

#define HANDLE_H5S_CLOSE_ERR \
	 (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_HDF5, \
		"Cannot terminate access to dataspace." ); 

#define HANDLE_H5S_GET_SELECT_NPOINTS_ERR \
	 (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_HDF5, \
		"Cannot determine the number of elements in dataspace selection." ); 

#define HANDLE_H5S_GET_SIMPLE_EXTENT_NPOINTS_ERR \
	 (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_HDF5, \
		"Cannot determine number of elements in dataspace." ); 

#define HANDLE_H5S_SELECT_HYPERSLAB_ERR \
	 (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_HDF5, \
		"Cannot set select hyperslap region or add the specified region" );

/* H5T:  type */
#define HANDLE_H5T_CLOSE_ERR \
	 (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_HDF5, \
		"Cannot release datatype." );


/* MPI */
#define HANDLE_MPI_ALLGATHER_ERR \
	 (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_MPI, \
		"Cannot gather data." );

#define HANDLE_MPI_COMM_SIZE_ERR \
	 (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_MPI, \
		"Cannot get number of processes in my group." );

#define HANDLE_MPI_COMM_RANK_ERR \
	 (*_err_handler) ( \
		__funcname, \
		H5PART_ERR_MPI, \
		"Cannot get rank of the calling process in my group." );

#endif

