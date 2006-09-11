#ifndef _H5Part_H_
#define _H5Part_H_
#include <stdlib.h>
#include <hdf5.h>
#ifdef PARALLEL_IO
#include <mpi.h>
#endif

/**
	/struct		
	H5PartFile:  This is an essentially opaque datastructure that
	acts as the filehandle for all practical purposes.
	It is created by H5PartOpenFile<xx>() and destroyed by
	H5PartCloseFile().  
*/
typedef struct H5PartFile {
  hid_t file;
  int timestep;

  hid_t timegroup;
  hid_t properties;
  hsize_t nparticles;
  hid_t shape;
  unsigned mode;
  int maxstep;
  hid_t xfer_prop,create_prop,access_prop;
  hid_t diskshape,memshape; /* for parallel I/O (this is on-disk) H5S_ALL 
		    if serial I/O */
  long long viewstart,viewend; /* -1 if no view is available: A "view" looks at a subset of the data. */
  
#ifdef PARALLEL_IO
  long long *pnparticles; /* the number of particles in each processor.
			     With respect to the "VIEW", these numbers
			     can be regarded as non-overlapping subsections
			     of the particle array stored in the file.
			     So they can be used to compute the offset of
			     the view for each processor */
  MPI_Comm comm;
  int nprocs,myproc;
#endif
}H5PartFile;

#define H5PART_READ 0x01
#define H5PART_WRITE 0x02
#define H5PART_APPEND 0x03

/*========== File Opening/Closing ===============*/
#ifdef PARALLEL_IO
#include <mpi.h>
H5PartFile *H5PartOpenFileParallel(const char *filename,
				   unsigned flags,
				   MPI_Comm communicator);
#endif
#define H5PartOpenFileSerial(x,y) H5PartOpenFile(x,y)

H5PartFile *H5PartOpenFile(const char *filename, /* name of datafile */
				 unsigned flags); /* H5PART_READ | H5PART_WRITE | H5PART_APPEND*/
int H5PartFileIsValid(H5PartFile *f);
void H5PartCloseFile(H5PartFile *f);


/*============== File Writing Functions ==================== */
void H5PartSetNumParticles(H5PartFile *f,long long nparticles);
int H5PartWriteDataFloat64(H5PartFile *f,char *name,double *array);
int H5PartWriteDataInt64(H5PartFile *f,char *name,long long *array);

/*================== File Reading Routines =================*/
void H5PartSetStep(H5PartFile *f, /* file handle */
		   int step); /* current timestep to select (0 to n-1) */
int H5PartGetNumSteps(H5PartFile *f);
int H5PartGetNumDatasets(H5PartFile *f);
int H5PartGetDatasetName(H5PartFile *f,int indx,char *name,int maxlen);

/* ANTINO KIM */
void H5PartGetDatasetInfo(H5PartFile *f, int idx, char *name, size_t maxnamelen, hid_t *type, long long *nelem);
hid_t H5PartNormType(hid_t type);


long long H5PartGetNumParticles(H5PartFile *f);
void H5PartSetView(H5PartFile *f,long long start,long long end);
#define H5PartResetView(f) H5PartSetView(f,-1,-1)
#define H5PartHasView(f) ((f->viewstart<0||f->viewend<0)?0:1)
int H5PartGetView(H5PartFile *f,long long *start,long long *end);
void H5PartSetCanonicalView(H5PartFile *f);

int H5PartReadDataFloat64(H5PartFile *f,
			  char *name, /* name of the array to read
					 "x"=position in x direction (y,z)
					 "vx"=velocity in x directio (y,z)
					 "px"=position in x dir (y,z) */
			  double *array); /* array to read data into */
int H5PartReadDataInt64(H5PartFile *f,
			  char *name, /* name of the array to read
					 "x"=position in x direction (y,z)
					 "vx"=velocity in x directio (y,z)
					 "px"=position in x dir (y,z) */
			  long long *array); /* array to read data into */

/* the following is a back-door for extensions to the data writing */
#if 0
int H5PartReadData(H5PartFile *f,char *name,void *array,hid_t type);
int H5PartWriteData(H5PartFile *f,char *name,void *array,hid_t type);
#endif
int H5PartReadParticleStep(H5PartFile *f, /* filehandle */
			   int step, /* selects timestep to read from*/
			   double *x,double *y,double *z, /* particle positions */
                           double *px,double *py,double *pz, /* particle momenta */
			   long long *id); /* and phase */
/**********==============Attributes Interface============***************/
/* currently there is file attributes:  Attributes bound to the file
   and step attributes which are bound to the current timestep.  You 
   must set the timestep explicitly before writing the attributes (just
   as you must do when you write a new dataset.  Currently there are no
   attributes that are bound to a particular data array, but this could
   easily be done if required.
*/
int H5PartWriteStepAttrib(H5PartFile *f,char *name,
    hid_t type,void *attrib,int nelem);
int H5PartWriteFileAttrib(H5PartFile *f,char *name,
    hid_t type,void *attrib,int nelem);
int H5PartWriteAttrib(H5PartFile *f,char *name,
		      hid_t type,void *attrib,int nelem); /* this should be deprecated */

int H5PartWriteFileAttribString(H5PartFile *f,char *name,
    char *attrib);
int H5PartWriteStepAttribString(H5PartFile *f,char *name,
    char *attrib);
int H5PartGetNumStepAttribs(H5PartFile *f); /* for current filestep */
int H5PartGetNumFileAttribs(H5PartFile *f);
void H5PartGetStepAttribInfo(H5PartFile *f,int idx,
    char *name,size_t maxnamelen,
			 hid_t *type,int *nelem);
void H5PartGetFileAttribInfo(H5PartFile *f,int idx,
    char *name,size_t maxnamelen,
    hid_t *type,int *nelem);
int H5PartReadStepAttrib(H5PartFile *f,char *name,void *data);
void H5PartReadAttrib(H5PartFile *f,char *name,void *data);
int H5PartReadFileAttrib(H5PartFile *f, char *name,void *data);


/**************** File Stashing Interfaces *************************/
int H5PartStashFile(H5PartFile *f,char *filename);
int H5PartUnstashFile(H5PartFile *f, char *filename, char *outputpath); /* outputpath can be null for cwd */
int H5PartGetNumStashFiles(H5PartFile *f);
int H5PartFileGetStashFileName(H5PartFile *f,int nameindex,char *filename,int maxnamelen);


#endif
