#ifndef _H5Part_H_
#define _H5Part_H_

#include <stdlib.h>
#include <stdarg.h>
#include <hdf5.h>
#ifdef PARALLEL_IO
#include <mpi.h>
#endif
#include "H5PartTypes.h"


#define H5PART_SUCCESS		0
#define H5PART_ERR_NOMEM	-12
#define H5PART_ERR_INVAL	-22
#define H5PART_ERR_BADFD	-77

#define H5PART_ERR_INIT         -200

#define H5PART_ERR_MPI		-201
#define H5PART_ERR_HDF5		-202


#define H5PART_READ		0x01
#define H5PART_WRITE		0x02
#define H5PART_APPEND		0x03

/*========== File Opening/Closing ===============*/
H5PartFile*
H5PartOpenFile(
	const char *filename,
	unsigned flags
	);

#define H5PartOpenFileSerial(x,y) H5PartOpenFile(x,y)

#ifdef PARALLEL_IO
H5PartFile*
H5PartOpenFileParallel (
	const char *filename,
	unsigned flags,
	MPI_Comm communicator
	);
#endif


h5part_int64_t
H5PartCloseFile (
	H5PartFile *f
	);


/*============== File Writing Functions ==================== */
h5part_int64_t
H5PartSetNumParticles ( 
	H5PartFile *f, 
	h5part_int64_t nparticles
	);

h5part_int64_t
H5PartWriteDataFloat64 (
	H5PartFile *f,
	char *name,
	h5part_float64_t *array
	);

h5part_int64_t
H5PartWriteDataInt64 (
	H5PartFile *f,
	char *name,
	h5part_int64_t *array
	);

/*================== File Reading Routines =================*/
h5part_int64_t
H5PartSetStep (
	H5PartFile *f,
	h5part_int64_t step
	);

h5part_int64_t
H5PartGetNumSteps (
	H5PartFile *f
	);

h5part_int64_t
H5PartGetNumDatasets (
	H5PartFile *f
	);

h5part_int64_t
H5PartGetDatasetName (
	H5PartFile *f,
	int indx,
	char *name,
	size_t maxlen
	);

h5part_int64_t
H5PartGetDatasetInfo (
	H5PartFile *f,
	int idx,
	char *name,
	size_t maxlen,
	hid_t *type,
	h5part_int64_t *nelem);


h5part_int64_t
H5PartGetNumParticles (
	H5PartFile *f
	);

h5part_int64_t
H5PartSetView (
	H5PartFile *f,
	h5part_int64_t start,
	h5part_int64_t end
	);

#define H5PartResetView(f) H5PartSetView(f,-1,-1)
#define H5PartHasView(f) ((f->viewstart<0||f->viewend<0)?0:1)

h5part_int64_t
H5PartGetView (
	H5PartFile *f,
	h5part_int64_t *start,
	h5part_int64_t *end
	);

h5part_int64_t
H5PartSetCanonicalView (
	H5PartFile *f
	);

h5part_int64_t
H5PartReadDataFloat64(
	H5PartFile *f,
	char *name,
	h5part_float64_t *array
	);

h5part_int64_t
H5PartReadDataInt64 (
	H5PartFile *f,
	char *name,
	h5part_int64_t *array
	);

/* the following is a back-door for extensions to the data writing */
#if 0
h5part_int64_t
H5PartReadData ( 
	H5PartFile *f,
	char *name,
	void *array,
	hid_t type
	);
h5part_int64_t
H5PartWriteData (
	H5PartFile *f,
	char *name,
	void *array,
	hid_t type
	);
#endif

h5part_int64_t
H5PartReadParticleStep (
	H5PartFile *f,
	h5part_int64_t step,
	h5part_float64_t *x, /* particle positions */
	h5part_float64_t *y,
	h5part_float64_t *z,
	h5part_float64_t *px, /* particle momenta */
	h5part_float64_t *py,
	h5part_float64_t *pz,
	h5part_int64_t *id /* and phase */
	);

/**********==============Attributes Interface============***************/
/* currently there is file attributes:  Attributes bound to the file
   and step attributes which are bound to the current timestep.  You 
   must set the timestep explicitly before writing the attributes (just
   as you must do when you write a new dataset.  Currently there are no
   attributes that are bound to a particular data array, but this could
   easily be done if required.
*/
h5part_int64_t
H5PartWriteStepAttrib (
	H5PartFile *f,
	char *name,
	hid_t type,
	void *attrib,
	int nelem
	);

h5part_int64_t
H5PartWriteFileAttrib (
	H5PartFile *f,
	char *name,
	hid_t type,
	void *attrib,
	int nelem
	);

h5part_int64_t
H5PartWriteAttrib (
	H5PartFile *f,
	char *name,
	hid_t type,
	void *attrib,
	int nelem /* this should be deprecated ??? */
	);

h5part_int64_t
H5PartWriteFileAttribString (
	H5PartFile *f,
	char *name,
	char *attrib
	);

h5part_int64_t
H5PartWriteStepAttribString ( 
	H5PartFile *f,
	char *name,
	char *attrib
	);

h5part_int64_t
H5PartGetNumStepAttribs ( /* for current filestep */
	H5PartFile *f
	);

h5part_int64_t
H5PartGetNumFileAttribs (
	H5PartFile *f
	);

h5part_int64_t
H5PartGetStepAttribInfo (
	 H5PartFile *f,
	 int idx,
	 char *name,
	 size_t maxnamelen, /* ??? */
	 hid_t *type,
	 int *nelem
	 );

h5part_int64_t
H5PartGetFileAttribInfo (
	H5PartFile *f,
	int idx,
	char *name,
	size_t maxnamelen,
	hid_t *type,
	int *nelem
	);

h5part_int64_t
H5PartReadStepAttrib (
	H5PartFile *f,
	char *name,
	void *data
	);

h5part_int64_t
H5PartReadAttrib (
	H5PartFile *f,
	char *name,
	void *data
	);

h5part_int64_t
H5PartReadFileAttrib (
	H5PartFile *f,
	char *name,
	void *data
	);

/**************** File Stashing Interfaces *************************/
h5part_int64_t
H5PartStashFile (
	H5PartFile *f,
	char *filename
	);

h5part_int64_t
H5PartUnstashFile (
	H5PartFile *f,
	char *filename,
	char *outputpath  /* outputpath can be null for cwd */
	);

h5part_int64_t
H5PartGetNumStashFiles (
	H5PartFile *f
	);

h5part_int64_t
_H5PartFileGetStashFileName (
	H5PartFile *f,
	int nameindex,
	char *filename,
	int maxlen
	);

h5part_int64_t
H5PartSetVerbosityLevel (
	unsigned int
	);

h5part_int64_t
H5PartSetErrorHandler (
	h5part_error_handler handler
	);

h5part_int64_t
H5PartGetErrno (
	void
	);

h5part_int64_t
H5PartDefaultErrorHandler (
	const char *funcname,
	const h5part_int64_t eno,
	const char *fmt,
	...
	);

h5part_int64_t
H5PartAbortErrorHandler (
	const char *funcname,
	const h5part_int64_t eno,
	const char *fmt,
	...
	);

void vprint_error ( const char *fmt, va_list ap );
void print_error  ( const char *fmt, ... );
void vprint_warn  ( const char *fmt, va_list ap );
void print_warn   ( const char *fmt, ... );
void vprint_info  ( const char *fmt, va_list ap );
void print_info   ( const char *fmt, ... );
void vprint_debug ( const char *fmt, va_list ap );
void print_debug  ( const char *fmt, ... );

#endif
