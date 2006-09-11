/**
	H5Part C API	
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <hdf5.h>
#include "H5Part.h"

#ifdef IPL_XT3
# define SEEK_END 2 
#endif

static unsigned h5part_debug=0;

/********* Private Function Declarations *************/
/**
	H5PartIOcounter: Private
*/	
herr_t H5PartIOcounter(hid_t group_id,
		   const char *member_name,
		   void *operator_data);

/********** Definitions of Functions/Subroutines ******/

/*========== File Opening/Closing ===============*/
/**
  H5PartOpenFileParallel:  Opens file with specified filename. 
    If you open with flag H5PART_WRITE, it will truncate any
    file with the specified filename and start writing to it.
    If you open with H5PART_READ, then it will open the file
    readonly.

    H5PartFile should be treated as an essentially opaque
    datastructure.  It acts as the file handle, but internally
    it maintains several key state variables associated with 
    the file.

		\param filename The name of the IEEEIO data file to open.
		The typical extension for these files is .h5
		\param flags The accessmode for the file.  This is
		one of 2 different access modes: H5PART_READ, H5PART_WRITE.
		\param comm This argument is only available if the program has been
		compiled with the PARALLEL_IO
		\return A new filehandle with an open file or NULL if error.
 */
/*
  I could probably do an APPEND mode as well, but
    just haven't done so yet.  APPEND would need to check the
    file to determine its current state and modify the 
    H5PartFile datastructure accordingly.
		#ifdef PARALLEL_IO
		,		   MPI_Comm comm
		#endif
*/
H5PartFile *H5PartOpenFileParallel(const char *filename,
		unsigned flags
		#ifdef PARALLEL_IO
		,		   MPI_Comm comm
		#endif
){
  H5PartFile *f=(H5PartFile*)malloc(sizeof(H5PartFile));
#ifdef PARALLEL_IO
  MPI_Info info = MPI_INFO_NULL; /* for the SP2... perhaps different for linux */
#endif
  f->xfer_prop = f->create_prop = f->access_prop=H5P_DEFAULT;
#ifdef PARALLEL_IO
  MPI_Comm_size(comm,&f->nprocs);
  MPI_Comm_rank(comm,&f->myproc);
  f->pnparticles=(long long*)malloc(sizeof(long long)*f->nprocs);
  /* access_prop: properties like chunking or parallel I/O */
  f->access_prop = H5Pcreate(H5P_FILE_ACCESS);
  if(H5Pset_fapl_mpio(f->access_prop,comm,info)<0){
	fprintf(stderr,"Total failure trying to setup mpi-io for access\n");
	exit(0);
  }
  /* create_prop: tunable parameters like blocksize and btree sizes */
  /* f->create_prop = H5Pcreate(H5P_FILE_CREATE); */
  f->create_prop = H5P_DEFAULT;
  /* currently create_prop is empty */
  /* xfer_prop:  also used for parallel I/O, during actual writes
          rather than the access_prop which is for file creation. */
  f->xfer_prop = H5Pcreate(H5P_DATASET_XFER);
  H5Pset_dxpl_mpio(f->xfer_prop,H5FD_MPIO_COLLECTIVE);
  f->comm = comm;
#endif
  if(flags==H5PART_READ){
    f->file=H5Fopen(filename,H5F_ACC_RDONLY,f->access_prop);
  	f->timestep=0;
    /* just do this serially 
       f->file = H5Fopen(filename,H5F_ACC_RDONLY,H5P_DEFAULT); */
  }
	else if (flags==H5PART_WRITE){
    f->file=H5Fcreate(filename,H5F_ACC_TRUNC,f->create_prop,f->access_prop);
    f->timestep=0;
    if(f->file<=0) {
      free(f);
      fprintf(stderr,"H5Open: File open failed for file [%s]\n",filename);
      exit(0);
      return 0;
    }

	}
  else if(flags==H5PART_APPEND){
    int fd;
    fd = open(filename, O_RDONLY, 0);
		if (fd == -1){
    f->file=H5Fcreate(filename,H5F_ACC_TRUNC,f->create_prop,f->access_prop);
  	f->timestep=0;
		} else if (fd != -1) {
		close(fd);
		f->file=H5Fopen(filename,H5F_ACC_RDWR,f->access_prop);
		if(f->file<=0) {
			free(f);
			fprintf(stderr,"H5Open: File open failed for file [%s]\n",filename);
			exit(0);
			return 0;
		}
		int nSteps = H5PartGetNumSteps(f);
		f->timestep= nSteps;
	}
  }
  else {
    f->file=-1;
    fprintf(stderr,"H5Open: Invalid file access type %d\n",flags);
  }
  if(f->file<=0) {
    free(f);
    fprintf(stderr,"H5Open: File open failed for file [%s]\n",
	    filename);
	exit(0); 
	return 0;
  }
#ifdef PARALLEL_IO
  else {
	if(h5part_debug) fprintf(stderr,"Proc[%d] Opened file %s val=%d\n",
		f->myproc,filename,f->file);
  }
#endif
  f->mode=flags;
  f->maxstep=0;
  f->timegroup=0;
  f->shape=0;
  f->diskshape=H5S_ALL;
  f->memshape=H5S_ALL;
  f->viewstart=-1;
  f->viewend=-1;
  return f;
}
/**
  H5PartOpenFile:  Opens file with specified filename. 
    If you open with flag H5PART_WRITE, it will truncate any
    file with the specified filename and start writing to it.
    If you open with H5PART_READ, then it will open the file
    readonly.

    H5PartFile should be treated as an essentially opaque
    datastructure.  It acts as the file handle, but internally
    it maintains several key state variables associated with 
    the file.

    \param filename The name of the IEEEIO data file to open.
    The typical extension for these files is .h5
    \param flags The accessmode for the file.  This is
    one of 2 different access modes: H5PART_READ, H5PART_WRITE.
    \return A new filehandle with an open file or NULL if error.
 */

/*
  I could probably do an APPEND mode as well, but
    just haven't done so yet.  APPEND would need to check the
    file to determine its current state and modify the 
    H5PartFile datastructure accordingly.
*/

H5PartFile *H5PartOpenFile(const char *filename,unsigned flags){
  H5PartFile *f=(H5PartFile*)malloc(sizeof(H5PartFile));
  /* printf("filename is [%s] flags=%0X\n",filename,flags); */
  
  f->xfer_prop = f->create_prop = f->access_prop=H5P_DEFAULT;
#ifdef PARALLEL_IO
  f->pnparticles=0;
  f->comm = MPI_COMM_WORLD;
  f->nprocs=1;
  f->myproc=0;
#endif
  if(flags==H5PART_READ){
    /*  f->file=H5Fopen(filename,H5F_ACC_RDONLY,f->access_prop); */
    /* just do this serially */
    f->file = H5Fopen(filename,H5F_ACC_RDONLY,H5P_DEFAULT);
    if(f->file==-1){
      fprintf(stderr,"H5Open: File %s not found\n",filename);
    }
  	f->timestep=0;
  }
  else if (flags==H5PART_WRITE){
    f->file=H5Fcreate(filename,H5F_ACC_TRUNC,f->create_prop,f->access_prop);
    f->timestep=0;
    if(f->file<=0) {
      free(f);
      fprintf(stderr,"H5Open: File open failed for file [%s]\n",filename);
      exit(0);
      return 0;
    }
  }
  else if(flags==H5PART_APPEND){
		int fd;
		fd = open(filename, O_RDONLY, 0);
    if (fd == -1){
    f->file=H5Fcreate(filename,H5F_ACC_TRUNC,f->create_prop,f->access_prop);
    f->timestep=0;
    } else if (fd != -1) {
		close(fd);
    f->file=H5Fopen(filename,H5F_ACC_RDWR,f->access_prop);
    if(f->file<=0) {
      free(f);
      fprintf(stderr,"H5Open: File open failed for file [%s]\n",filename);
      exit(0);
      return 0;
    }
    int nSteps = H5PartGetNumSteps(f);
    f->timestep= nSteps;
  }

  }
  else {
    f->file=-1;
    fprintf(stderr,"H5Open: Invalid file access type %d\n",flags);
  }
  if(f->file<0) {
    free(f);
    fprintf(stderr,"H5Open: File open failed for file [%s]\n",
	    filename);
    return 0;
  }
  f->mode=flags;
  f->maxstep=0;
  f->timegroup=0;
  f->shape=0;
  f->diskshape=H5S_ALL;
  f->memshape=H5S_ALL;
  f->viewstart=-1;
  f->viewend=-1;
  return f;
}

/**

	H5PartFileIsValid: checks if a file was successfully opened.
	\param f filehandle of the file to check validity
	\return 1 if success 0 if failure 

 */
int H5PartFileIsValid(H5PartFile *f){
  if(!f)
  {
   /* if f is NULL... (There was a case...) */
   return 0;
  }
  else if(f->file > 0) return 1;
  else return 0;
}
/**
  H5PartCloseFile:  closes an open file.
	\param  f filehandle of the file to close
	\return void
*/
void H5PartCloseFile(H5PartFile *f){
  if(f->shape>0) H5Sclose(f->shape); f->shape=0;
  if(f->timegroup>0) H5Gclose(f->timegroup); f->timegroup=0;
  if(f->diskshape!=H5S_ALL) H5Sclose(f->diskshape);
#ifdef PARALLEL_IO
  if(f->pnparticles) free(f->pnparticles);
#endif
  if(f->xfer_prop!=H5P_DEFAULT) {
    H5Pclose(f->xfer_prop); f->xfer_prop=H5P_DEFAULT;
  }
  if(f->access_prop!=H5P_DEFAULT) {
    H5Pclose(f->access_prop); f->access_prop=H5P_DEFAULT;
  }  
  if(f->create_prop!=H5P_DEFAULT) {
    H5Pclose(f->create_prop); f->create_prop=H5P_DEFAULT;
  }
  if(f->file) H5Fclose(f->file); f->file=0;
  free(f);
}


/*============== File Writing Functions ==================== */
/**
	H5PartSetNumParticles: This function's sole purpose is to prevent 
	needless creation of new HDF5 DataSpace handles if the number of 
	particles is invariant throughout the sim. That's its only reason 
	for existence. After you call this subroutine, all subsequent 
	operations will assume this number of particles will be written.
	\param f A FileHandle opened by H5PartOpenFile() or H5PartOpenFileParallel().
	\param nparticles A long long integer specifying the number of particles
	\return void 
 */
void H5PartSetNumParticles(H5PartFile *f,long long nparticles){
#ifdef HDF5V160
  hssize_t start[1];
#else
  hsize_t start[1];
#endif
  hsize_t stride[1],count[1],total;
  register int i,r;
  /*  printf("Set Number of Particles to %lld\n",nparticles); */
#ifndef PARALLEL_IO
  /* if we are not using parallel-IO, there is enough information
     to know that we can short circuit this routine.  However,
     for parallel IO, this is going to cause problems because
     we don't know if things have changed globally */
  if(f->nparticles==nparticles) return;  /* no change  */
#endif
  if(f->diskshape!=H5S_ALL) H5Sclose(f->diskshape);
  if(f->memshape!=H5S_ALL) H5Sclose(f->memshape);
  f->memshape=f->diskshape = H5S_ALL;
  if(f->shape) H5Sclose(f->shape);
  f->nparticles=(hsize_t)nparticles;
#ifndef PARALLEL_IO
  f->shape=H5Screate_simple(1, /* 1 dimensional shape */
			    &(f->nparticles), /* with nparticles elements */
			    NULL); /* ignore this :-) */
#else /* PARALLEL_IO */
  /* The Gameplan here is to declare the overall size of the on-disk
     data structure the same way we do for the serial case.  But
     then we must have additional "DataSpace" structures to define
     our in-memory layout of our domain-decomposed portion of the particle
     list as well as a "selection" of a subset of the on-disk 
     data layout that will be written in parallel to mutually exclusive
     regions by all of the processors during a parallel I/O operation.
     These are f->shape, f->memshape and f->diskshape respectively. */
  /* acquire the number of particles to be written from each MPI process */
  MPI_Allgather(&nparticles,1,MPI_LONG_LONG,f->pnparticles,1,MPI_LONG_LONG,f->comm);

  if(f->myproc==0 && h5part_debug){
  	puts("AllGather:  Particle offsets:\n");
  	for(i=0;i<f->nprocs;i++) 
		printf("\tnp=%d\n",(int) f->pnparticles[i]); /* compute total nparticles */
  }
  /* should I create a selection here? */
  stride[0]=1;
  start[0]=0;
  for(i=0;i<f->myproc;i++) start[0]+=f->pnparticles[i]; /* compute start offsets */
  total=0;
  for(i=0;i<f->nprocs;i++) total+=f->pnparticles[i]; /* compute total nparticles */
  f->shape = H5Screate_simple(1,&total,&total); /* declare overall datasize */
  f->diskshape = H5Screate_simple(1,&total,&total); /* declare overall data size  but then will select a subset */
  {
    hsize_t dmax=H5S_UNLIMITED;
    f->memshape = H5Screate_simple(1,&(f->nparticles),&dmax); /* declare local memory datasize */
  }

  if(f->shape<0 || f->memshape<0 || f->diskshape<0){
	fprintf(stderr,"Abort:  shape construction failed\n");
	if(f->shape<0) fprintf(stderr,"\tf->shape\n");
	if(f->diskshape<0) fprintf(stderr,"\tf->diskshape\n");
	if(f->memshape<0) fprintf(stderr,"\tf->memshape\n");
 	exit(0);
}
  count[0]=nparticles; /* based on local nparticles (for the selection */
  /* and then set the subset of the data you will write to */
  r=H5Sselect_hyperslab(f->diskshape,H5S_SELECT_SET,start,stride,count,NULL);
  if(r<0){
	fprintf(stderr,"Abort: Selection Failed!\n");
	exit(0);
  }
  if(f->timegroup<0){
    H5PartSetStep(f,0);
  }
#endif
}

/**
  H5PartWriteDataFloat64: 
		After setting the number of particles with H5PartSetNumParticles() and the current timestep using H5PartSetStep(), you can start writing datasets into the file. Each dataset has a name associated with it (chosen by the user) in order to facilitate later retrieval. The writing routines also implicitly store the datatype of the array so that the array can be reconstructed properly on other systems with incompatible type representations. The data is committed to disk before the routine returns. All data that is written after setting the timestep is associated with that timestep. While the number of particles can change for each timestep, you cannot change the number of particles in the middle of a given timestep.

	\param f A FileHandle opened by H5PartOpenFile() or H5PartOpenFileParallel().
	\param name A null-terminated string for the name of the array. When retrieving datasets from disk, you ask for them by name. There are no restrictions on naming of arrays, but it is useful to arrive at some common naming convention when sharing data with other groups.
	\param array A buffer containing an array of particle data to commit to disk. The datatype for elements in the buffer is implicit in the name of the subroutine call.
	\return 1 on success, 0 on failure.
 */
int H5PartWriteDataFloat64(H5PartFile *f,char *name,double *array){
  register int r;
  hid_t dataset;
 if(f->mode==H5PART_READ){
	fprintf(stderr,"H5PartWriteDataFloat64:  Error!  Attempting to write to read-only file\n");
	return 0;
  }
  /* fprintf(stderr,"Create a dataset[%s]  mounted on the timegroup %d\n",
  	 name,f->timestep); */
  if(f->timegroup<=0) {fprintf(stderr,"Eeeerrrrroooorrrr!!!!\n");}
  dataset = H5Dcreate(f->timegroup,name,H5T_NATIVE_DOUBLE,f->shape,H5P_DEFAULT);
  if(dataset<0)
    fprintf(stderr,"Dataset open failed for name=[%s] step=%d!\n",
	    name,f->timestep);
  r=H5Dwrite(dataset,H5T_NATIVE_DOUBLE,f->memshape,f->diskshape,H5P_DEFAULT,array);
  H5Dclose(dataset);
  return r;
}
/**
  H5PartWriteDataInt64: 
    After setting the number of particles with H5PartSetNumParticles() and the current timestep using H5PartSetStep(), you can start writing datasets into the file. Each dataset has a name associated with it (chosen by the user) in order to facilitate later retrieval. The writing routines also implicitly store the datatype of the array so that the array can be reconstructed properly on other systems with incompatible type representations. The data is committed to disk before the routine returns. All data that is written after setting the timestep is associated with that timestep. While the number of particles can change for each timestep, you cannot change the number of particles in the middle of a given timestep.

  \param f A FileHandle opened by H5PartOpenFile() or H5PartOpenFileParallel().
  \param name A null-terminated string for the name of the array. When retrieving datasets from disk, you ask for them by name. There are no restrictions on naming of arrays, but it is useful to arrive at some common naming convention when sharing data with other groups.
  \param array A buffer containing an array of particle data to commit to disk. The datatype for elements in the buffer is implicit in the name of the subroutine call.
  \return 1 on success, 0 on failure.
 */

int H5PartWriteDataInt64(H5PartFile *f,char *name,long long *array){
  register int r;
  hid_t dataset;
  /*fprintf(stderr,"Create a dataset[%s]  mounted on the timegroup %d\n",
  	 name,f->timestep); */
 if(f->mode==H5PART_READ){
	fprintf(stderr,"H5PartWriteDataFloat64:  Error!  Attempting to write to read-only file\n");
	return 0;
  }
  if(f->timegroup<=0) {fprintf(stderr,"Eeeerrrrroooorrrr!!!!\n");}
  dataset = H5Dcreate(f->timegroup,name,H5T_NATIVE_INT64,f->shape,H5P_DEFAULT);
  if(dataset<0){
    fprintf(stderr,"Dataset open failed for name=[%s] step=%d!\n",
	    name,f->timestep);
    exit(0);
  }
  r=H5Dwrite(dataset,H5T_NATIVE_INT64,f->memshape,f->diskshape,H5P_DEFAULT,array);
  if(r<0) {
	fprintf(stderr,"Attempt to write dataset failed!\n");
	exit(0);
  }
  H5Dclose(dataset);
  return r;
}

/**
	H5PartWriteFileAttribString: writes a string attribute bound to a file.

	\param f A FileHandle opened by H5PartOpenFile() or H5PartOpenFileParallel().
	\param name A null-terminated string for the name of the attribute.
	\param attrib  A null-terminated string for the attribute. 
	\return 1 on success, 0 on failure.
*/

int H5PartWriteFileAttribString(H5PartFile *f,char *name,
				char *attrib){
 if(f->mode==H5PART_READ){
	fprintf(stderr,"H5PartWriteDataFloat64:  Error!  Attempting to write to read-only file\n");
	return 0;
  }
  return H5PartWriteFileAttrib(f,name,H5T_NATIVE_CHAR,attrib,strlen(attrib)+1);
}
/**
  H5PartWriteStepAttrString: writes a string attribute bound to the current step.

  \param f A FileHandle opened by H5PartOpenFile() or H5PartOpenFileParallel().
  \param name A null-terminated string for the name of the attribute.
  \param attrib  A null-terminated string for the attribute. 
  \return 1 on success, 0 on failure.
*/

int H5PartWriteStepAttribString(H5PartFile *f,char *name,
				char *attrib){
 if(f->mode==H5PART_READ){
	fprintf(stderr,"H5PartWriteDataFloat64:  Error!  Attempting to write to read-only file\n");
	return 0;
  }
  return H5PartWriteStepAttrib(f,name,H5T_NATIVE_CHAR,attrib,strlen(attrib)+1);
}

/**
	H5PartWriteStepAttrib: writes a step attribute.
  \param f A FileHandle opened by H5PartOpenFile() or H5PartOpenFileParallel().
  \param name A null-terminated string for the name of the attribute.
	\param type One of H5T_NATIVE_DOUBLE, H5T_NATIVE_INT64, H5T_NATIVE_CHAR.
  \param value Value of the attribute.
	\param nelem Number of elements of type "type".
  \return 1 on success, 0 on failure.
*/


int H5PartWriteStepAttrib(H5PartFile *f,char *name,
			  hid_t type,void *value,int nelem){
  register int r;
  hid_t attrib;
  hid_t space;
  hsize_t len;
 if(f->mode==H5PART_READ){
	fprintf(stderr,"H5PartWriteDataFloat64:  Error!  Attempting to write to read-only file\n");
	return 0;
  }
  /*fprintf(stderr,"Create a attribute[%s]  mounted on the timegroup %d\n",
  	 name,f->timestep); */
  if(f->timegroup<=0) {fprintf(stderr,"Eeeerrrrroooorrrr!!!!\n");}
  len = nelem;
  space = H5Screate_simple(1,&len,NULL);
  attrib = H5Acreate(f->timegroup,name,type,space,H5P_DEFAULT);
  r=H5Awrite(attrib,type,value);
  H5Aclose(attrib);
  H5Sclose(space);
  return r;
}

/**
  H5PartWriteAttr: writes a step attribute.
	Note: this call has been superseeded by  H5PartWriteStepAttr.

  \param f A FileHandle opened by H5PartOpenFile() or H5PartOpenFileParallel().
  \param name A null-terminated string for the name of the attribute.
	\param type One of H5T_NATIVE_DOUBLE, H5T_NATIVE_INT64, H5T_NATIVE_CHAR.
  \param value Value of the attribute.
	\param nelem Number of elements of type "type".
  \return 1 on success, 0 on failure.
*/

int H5PartWriteAttrib(H5PartFile *f,char *name,
    hid_t type,void *value,int nelem){
 if(f->mode==H5PART_READ){
	fprintf(stderr,"H5PartWriteDataFloat64:  Error!  Attempting to write to read-only file\n");
	return 0;
  }
    return H5PartWriteStepAttrib(f,name,type,value,nelem);
}

/**
  H5PartWriteStepAttr: writes a string attribute bound to a step.

  \param f A FileHandle opened by H5PartOpenFile() or H5PartOpenFileParallel().
  \param name A null-terminated string for the name of the attribute.
	\param type One of H5T_NATIVE_DOUBLE, H5T_NATIVE_INT64, H5T_NATIVE_CHAR.
  \param value Value of the attribute.
	\param nelem Number of elements of type "type".
  \return 1 on success, 0 on failure.
*/

int H5PartWriteFileAttrib(H5PartFile *f,char *name,
			  hid_t type,void *value,int nelem){
  register int r;
  hid_t attrib,rootgroup;
  hid_t space;
  hsize_t len;
 if(f->mode==H5PART_READ){
	fprintf(stderr,"H5PartWriteDataFloat64:  Error!  Attempting to write to read-only file\n");
	return 0;
  }
 if(h5part_debug) fprintf(stderr,"Create a File attribute[%s] step=%d\n",
			  name,f->timestep);
  if(f->file<=0) {
    fprintf(stderr,"Eeeerrrrroooorrrr!!!! File is not open!\n");
    return 0;
  }
  len = nelem;
  space = H5Screate_simple(1,&len,NULL); 
  if(space<=0) { 
    fprintf(stderr,"Eeeerrrrroooorrrr!!!! Could not create space with len: %d!\n",(int)len);
    return 0;
  }
  rootgroup = H5Gopen(f->file,"/");
  attrib = H5Acreate(rootgroup,name,type,space,H5P_DEFAULT);
  H5Gclose(rootgroup);
  if(attrib<=0) { 
    fprintf(stderr,"Eeeerrrrroooorrrr!!!! Attribute Creation Failed!\n");
    return 0;
  }
  r=H5Awrite(attrib,type,value);
  H5Aclose(attrib);
  H5Sclose(space);
  return r;
}
herr_t H5PartAttribcounter(hid_t group_id,
		   const char *member_name,
		   void *operator_data){
  int *count = (int*)operator_data;
  (*count)++;
  return 0;
}
/**
  H5PartGetNumStepAttribs: gets the number of attributes bound to the current step.
  The step has to be set with H5PartSetStep().
  \param f A FileHandle opened by H5PartOpenFile() or H5PartOpenFileParallel().
  \return number of attributes of the current step.
*/
int H5PartGetNumStepAttribs(H5PartFile *f){ /* for current filestep */
  return H5Aget_num_attrs(f->timegroup);
}
/**
  H5PartGetNumFileAttribs: gets the number of attributes bound to the file.
  \param f A FileHandle opened by H5PartOpenFile() or H5PartOpenFileParallel().
  \return number of attributes of the File.
*/
int H5PartGetNumFileAttribs(H5PartFile *f){
  /* must walk the attributes */
  /* iterate to get numsteps */
  int nattribs;
  hid_t rootgroup=H5Gopen(f->file,"/");;
  nattribs=H5Aget_num_attrs(rootgroup); /* open / for the file?
				       or is there a problem with 
				       file attributes? */
  H5Gclose(rootgroup);
  return nattribs;
}
/**
	H5PartNormType: Private
*/
hid_t H5PartNormType(hid_t type){
  H5T_class_t tclass = H5Tget_class(type);
  int size = H5Tget_size(type);
  switch(tclass){
  case H5T_INTEGER:
    if(size==8) return H5T_NATIVE_INT64;
    else if(size==1) return H5T_NATIVE_CHAR;
    else fprintf(stderr,"Unknown type %d.  Cannot infer normalized type\n",(int)type);
    break;
  case H5T_FLOAT:
    return H5T_NATIVE_DOUBLE;
    break;
  }
  return -1;
}

/**
  H5PartGetStepAttribInfo: gets the name, type and number of elements of a step attribute .
  \param f A FileHandle opened by H5PartOpenFile() or H5PartOpenFileParallel().
	\param idx Index of the attribute
	\param name A null-terminated string with the name of the attribute.
	\param maxname the length of the name of the attribute.
	\param type One of the following: H5T_NATIVE_DOUBLE H5T_NATIVE_INT64 H5T_NATIVE_CHAR. 
	\param nelem Number of elements of type "type".
  \return void.
*/
void H5PartGetStepAttribInfo(H5PartFile *f,int idx,
			 char *name,size_t maxname,hid_t *type,int *nelem){
  hid_t attrib=H5Aopen_idx(f->timegroup,idx);
  hid_t mytype=H5Aget_type(attrib);
  hid_t space = H5Aget_space(attrib);
  if(nelem)
    *nelem=H5Sget_simple_extent_npoints(space);
  if(name)
    H5Aget_name(attrib,maxname,name);
  if(type)
    *type=H5PartNormType(mytype); /* normalize the type to be machine-native
				 this means one of 
				H5T_NATIVE_DOUBLE
				H5T_NATIVE_INT64
				H5T_NATIVE_CHAR */
  H5Sclose(space);
  H5Tclose(mytype);
  H5Aclose(attrib);
}

/**
  H5PartGetFileAttribInfo: gets the name, type and number of elements of a file attribute .
  \param f A FileHandle opened by H5PartOpenFile() or H5PartOpenFileParallel().
  \param idx Index of the attribute
  \param name A null-terminated string with the name of the attribute.
	\param maxname The length of the attribute name.
  \param type One of the following: H5T_NATIVE_DOUBLE H5T_NATIVE_INT64 H5T_NATIVE_CHAR. 
  \param nelem Number of elements of type "type".
  \return void.
*/

void H5PartGetFileAttribInfo(H5PartFile *f,int idx,
			 char *name,size_t maxname,hid_t *type,int *nelem){
  hid_t rootgroup = H5Gopen(f->file,"/");
  hid_t attrib=H5Aopen_idx(rootgroup,idx);
  hid_t mytype=H5Aget_type(attrib);
  hid_t space = H5Aget_space(attrib);
  if(nelem)
    *nelem=H5Sget_simple_extent_npoints(space);
  if(name)
    H5Aget_name(attrib,maxname,name);
  if(type)
    *type=H5PartNormType(mytype); /* normalize the type to be machine-native
				 this means one of 
				H5T_NATIVE_DOUBLE
				H5T_NATIVE_INT64
				H5T_NATIVE_CHAR */
  H5Sclose(space);
  H5Tclose(mytype);
  H5Aclose(attrib);
}

/**
	H5PartReadStepAttrib: reads a step attribute
		\param f A FileHandle opened by H5PartOpenFile() or H5PartOpenFileParallel().
		\param name	A null-terminated string with the name of the attribute.
		\param data of the attribute.
		\return  1 on success, -1 on failure.
*/

int H5PartReadStepAttrib(H5PartFile *f,char *name,void *data){
  /* use the open attribute by name mode of operation */
  hid_t attrib=H5Aopen_name(f->timegroup,name);
  hid_t mytype;
  hid_t space;
  hsize_t nelem;
  hid_t type;

  if(attrib<=0) return -1;
  mytype=H5Aget_type(attrib);
  space = H5Aget_space(attrib);
  nelem=H5Sget_simple_extent_npoints(space);
  type=H5PartNormType(mytype); /* normalize the type to be machine-native
				 this means one of 
				H5T_NATIVE_DOUBLE
				H5T_NATIVE_INT64
				H5T_NATIVE_CHAR */
  H5Aread(attrib,type,data);
  H5Sclose(space);
  H5Tclose(mytype);
  H5Aclose(attrib);
  return 1;
}

/**
	H5PartReadAttrib: reads a step attribute
	NOTE: it has been superseeded by H5PartReadStepAttrib.
		\param f A FileHandle opened by H5PartOpenFile() or H5PartOpenFileParallel().
		\param name	A null-terminated string with the name of the attribute.
		\param data of the attribute.
		\return  1 on success, -1 on failure.
*/
void H5PartReadAttrib(H5PartFile *f,char *name,void *data){
    /* use the open attribute by name mode of operation */
    H5PartReadStepAttrib(f,name,data);
}

/**
  H5PartReadFileAttrib: reads a file attribute
    \param f A FileHandle opened by H5PartOpenFile() or H5PartOpenFileParallel().
    \param name  A null-terminated string with the name of the attribute.
    \param data Value of the attribute.
    \return  1 on success, -1 on failure.
*/


int H5PartReadFileAttrib(H5PartFile *f, char *name,void *data){
  hid_t rootgroup = H5Gopen(f->file,"/");
  hid_t attrib=H5Aopen_name(rootgroup,name);
  hid_t mytype;
  hid_t space;
  hsize_t nelem;
  hid_t type;
  if(attrib<0) return -1;
  mytype=H5Aget_type(attrib);
  space = H5Aget_space(attrib);
  nelem=H5Sget_simple_extent_npoints(space);
  type=H5PartNormType(mytype); /* normalize the type to be machine-native
				 this means one of 
				H5T_NATIVE_DOUBLE
				H5T_NATIVE_INT64
				H5T_NATIVE_CHAR */
  H5Aread(attrib,type,data);
  H5Sclose(space);
  H5Tclose(mytype);
  H5Aclose(attrib);
  return 1;
}


/*================== File Reading Routines =================*/
/*
  H5PartSetStep:  This routine is *only* useful when you are reading
  data.  Using it while you are writing will generate nonsense results!
  (This file format is only half-baked... robustness is not std equipment!)
  So you use this to random-access the file for a particular timestep.
  Failure to explicitly set the timestep on each read will leave you
  stuck on the same timestep for *all* of your reads.  That is to say
  the writes auto-advance the file pointer, but the reads do not
  (they require explicit advancing by selecting a particular timestep).
*/
/**
  H5PartSetStep:
  When writing data to a file the current time step must be set (even if there is only one). In a file with N time steps, the steps are numbered from 0 to N-1.
  \param f A FileHandle opened by H5PartOpenFile() or H5PartOpenFileParallel().
	\param step the time step to set.
  \returns void
*/

void H5PartSetStep(H5PartFile *f,int step){
  char name[128];
	int	nsteps;
#ifdef PARALLEL_IO
  if(h5part_debug) printf("Proc[%d] SetStep to %d for file %d\n",f->myproc,step,(int) f->file);
#else
  if(h5part_debug) fprintf(stderr,"SetStep to %d for file\n",step);
#endif
	nsteps = H5PartGetNumSteps(f);
  if (f->mode == H5PART_APPEND && step < nsteps) {
		fprintf(stderr,"SetStep to %d for file failed, there are %d steps already\n",step, nsteps);
		return;
	}
  f->timestep=step;   /* because we start at 0 */
  if(f->timegroup>0) {
	H5Gclose(f->timegroup); 
  }
  f->timegroup=-1;
  sprintf(name,"Particles#%d",f->timestep);
  /*if(h5part_debug) fprintf(stderr,"P[%d] Open timegroup [%s] for file %d\n",
	f->myproc,name,f->file);*/
  if(f->mode==H5PART_READ) { /* kludge to prevent error messages */
/*	if(h5part_debug) fprintf(stderr,"P[%d] open group\n"); */
    f->timegroup=H5Gopen(f->file,name);
  }
  if(f->timegroup<=0){
    /* timegroup doesn't exist, so we need to create one */
	/* if(h5part_debug) fprintf(stderr,"P[%d] create group\n"); */
    if(f->mode == H5PART_READ){
      fprintf(stderr,"Error in H5PartSetStep() Step #%d does not exist!\n",step);
    }
    f->timegroup = H5Gcreate(f->file,name,0);
    if(f->timegroup<0) {
#ifdef PARALLEL_IO
	fprintf(stderr,"p[%d] timegroup creation failed!\n",
		f->myproc);
#endif
	return;
    }
  }
  /*
    #ifdef PARALLEL_IO
    H5PartWriteStepAttrib(f,"pnparticles",H5T_NATIVE_INT64,f->pnparticles,f->nprocs);
    #endif
  */
}

/**
  H5PartIOcounter:  This is an entirely internal callback function 
   which is used in conjunction with HDF5 iterators.  The HDF5 Group
   iterator will call this repeatedly in order to count how many
   timesteps of data have been stored in a particular file.
   This is used by H5PartGetNumSteps().
*/
herr_t H5PartIOcounter(hid_t group_id,
		   const char *member_name,
		   void *operator_data){
  int *count = (int*)operator_data;
  H5G_stat_t objinfo;
  /* only count the particle groups... ignore all others */
  if(!strncmp(member_name,"Particles",9)) (*count)++;
  return 0;
}

/**
	H5PartDScounter: Private
*/
herr_t H5PartDScounter(hid_t group_id,
		   const char *member_name,
		   void *operator_data){
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
  int indx,count;
  char *name;
}H5IO_getname_t;

/**
	H5IOgetname: Private
*/
herr_t H5IOgetname(hid_t group_id,
			  const char *member_name, 
			  void *operator_data){
  H5IO_getname_t *getn = (H5IO_getname_t*)operator_data;
  /* check type first (only respond if it is a dataset) */
  H5G_stat_t objinfo;
  /* request info about the type of objects in root group */
  if(H5Gget_objinfo(group_id, 
		    member_name,
		    1 /* follow links */, 
		    &objinfo)<0) return 0; /* error (probably bad symlink) */
  /* only count objects that are datasets (not subgroups) */
  if(objinfo.type!=H5G_DATASET) 
    return 0; /* do not increment count if it isn't a dataset. */
  if(getn->indx==getn->count){
    strcpy(getn->name,member_name);
    return 1; /* success */
  }
  getn->count++;
  return 0;
}

/** 
H5PartGetNumSteps:  This reads the number of datasteps that are 
  currently stored in the datafile.  (simple return of an int).
  It works for both reading and writing of files, but is probably
  only typically used when you are reading.
  \param f A FileHandle opened by H5PartOpenFile() or H5PartOpenFileParallel().   
  \returns number of timesteps currently stored in the file.
*/
int H5PartGetNumSteps(H5PartFile *f){
  /* iterate to get numsteps */
  int count=0;
  int idx=0;
  while(H5Giterate(f->file, /* hid_t loc_id, */
                   "/", /*const char *name, */
                   &idx, /* int *idx, */
                   H5PartIOcounter,
                   &count)<0){}
  return count;
}
/**
	H5PartGetNumDatasets: This reads the number of datasetes that are 
		stored at a particular time step.
	\param f A FileHandle opened by H5PartOpenFile() or H5PartOpenFileParallel().
	\returns number of datasets.
*/

int H5PartGetNumDatasets(H5PartFile *f){
  int count=0;
  int idx=0;
  char name[128];
  /* we need to have scanned file to get min timestep
     before we call this */
  sprintf(name,"Particles#%d",f->timestep);
  while(H5Giterate(f->file, /* hid_t loc_id */
		   name,
		   &idx,
		   H5PartDScounter,
		   &count)<0){}
  return count;   
}
/**
  H5PartGetDatasetName: This reads the name of a datasetes specified by it's index.
  \param f A FileHandle opened by H5PartOpenFile() or H5PartOpenFileParallel().
	\param _index  integer specifying the index of the dataset. If the number of datasets is nds, the range of index is 0 to nds-1.
	\param name A null-terminated string for the name of the dataset.
	\param maxlen  An integer specifying the maximum length of the name array.
  \returns 1 on success, 0 on failure.
*/
int H5PartGetDatasetName(H5PartFile *f,
			 int _index, /* index of the dataset (0 to N-1) */
			 char *name, /* buffer to store name of dataset */
			 int maxlen){ /* max size of the "name" buffer */
  H5IO_getname_t getn;
  int idx=_index;
  char gname[128];
  sprintf(gname,"Particles#%d",f->timestep);
  getn.indx=_index; getn.name=name; getn.count=_index;
  while(H5Giterate(f->file, /* hid_t loc_id, */
		   gname, /*const char *name, */
		   &idx, /* int *idx, */
		   H5IOgetname,
		   &getn)<0){}
  return 1;
}

/**
  Author: Antino Kim
  H5PartGetDatasetInfo: gets the name, type and number of elements of a Dataset.
  \param f A FileHandle opened by H5PartOpenFile() or H5PartOpenFileParallel().
	\param idx Index of the dataset
	\param name A null-terminated string with the name of the dataset.
	\param maxname the length of the name of the dataset.
	\param type One of the following: H5T_NATIVE_DOUBLE H5T_NATIVE_INT64.
	\param nelem Number of elements of type "type".
  \return void.
*/
void H5PartGetDatasetInfo(H5PartFile *f, int _index, char *name, size_t maxlen, hid_t *type, long long *nelem)
{
  H5IO_getname_t getn;
  int idx=_index;
  char gname[128];
  hid_t dataset_id;
  hid_t mytype;

  sprintf(gname,"Particles#%d",f->timestep);
  getn.indx=_index; getn.name=name; getn.count=_index;
  while(H5Giterate(f->file, /* hid_t loc_id, */
		   gname, /*const char *name, */
		   &idx, /* int *idx, */
		   H5IOgetname,
		   &getn)<0){}

  *nelem = H5PartGetNumParticles(f); /* number of particles (not number of timesteps nor number of sets) */

  if(!f->timegroup) H5PartSetStep(f,f->timestep); /* choose current step */

  dataset_id=H5Dopen(f->timegroup,name);
  mytype = H5Dget_type(dataset_id);

  if(type)
    *type=H5PartNormType(mytype); /* H5T_NATIVE_INT64 H5T_NATIVE_DOUBLE */

  H5Tclose(mytype);
  H5Dclose(dataset_id); /* release the dataset handle */
}

/**
	H5PartGetDiskShape: Private
*/
hid_t H5PartGetDiskShape(H5PartFile *f,hid_t dataset){
  hid_t space = H5Dget_space(dataset);
  if(h5part_debug) fprintf(stderr,"H5PartGetDiskShape\n");
  if(H5PartHasView(f)){ 
    int r;
    hsize_t total,  stride, count;
#ifdef HDF5V160
  hssize_t range[2];
#else
  hsize_t range[2];
#endif
    if(h5part_debug) fprintf(stderr,"\tSelection is available\n");
    /* so, is this selection inclusive or exclusive? */
    range[0]=f->viewstart;
    range[1]=f->viewend;
    count = range[1]-range[0]; /* to be inclusive */
    stride=1;
    /* now we select a subset */
    if(f->diskshape>0)
      r=H5Sselect_hyperslab(f->diskshape,H5S_SELECT_SET,
			    range /* only using first element */,
			    &stride,&count,NULL);
    /* now we select a subset */
    r=H5Sselect_hyperslab(space,H5S_SELECT_SET,
			  range,&stride,&count,NULL);
    if(h5part_debug) fprintf(stderr,"\tSelection: range=%d:%d, npoints=%d s=%d\n",
	    (int)range[0],(int)range[1],
	    (int)H5Sget_simple_extent_npoints(space),
	    (int)H5Sget_select_npoints(space));
    if(r<0){
      fprintf(stderr,"Abort: Selection Failed!\n");
      return space;
    }
  }
  else {
    if(h5part_debug) fprintf(stderr,"\tNo Selection\n");
  }
  return space;
}

hid_t H5PartGetMemShape(H5PartFile *f,hid_t dataset){  
  if(h5part_debug) fprintf(stderr,"H5PartGetMemShape\n");
  if(H5PartHasView(f)) {
    hsize_t dmax=H5S_UNLIMITED;
    hsize_t len = f->viewend - f->viewstart;
    return H5Screate_simple(1,&len,&dmax);
  }
  else {
    return H5S_ALL;
  }
}

/**
	H5PartGetFirstDS: Private
*/
herr_t H5PartGetFirstDS(hid_t group_id,
		   const char *member_name,
		   void *operator_data){
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
    (*dataset)=H5Dopen(group_id,member_name);
    return 1; /* all done: return success */
  }
  return 0; /* causes iterator to continue to next item */
}

/**
  H5PartGetNumParticles:  This gets the number of particles 
    that are stored in the current timestep.  It will arbitrarily
    select a timestep if you haven't already set the timestep
    with H5PartSetStep().
		\param f A FileHandle opened by H5PartOpenFile() or H5PartOpenFileParallel().
		\returns number of particles in current timestep.
 */
long long H5PartGetNumParticles(H5PartFile *f){
  hid_t space,dataset;
  hsize_t nparticles;
  int idx=0;
  char name[128];
  /* we need to have scanned file to get min timestep
     before we call this */
  if(f->timegroup<0) {
    if(f->timestep<0)
      H5PartSetStep(f,0);
    else
      H5PartSetStep(f,f->timestep); /* choose a step */
  }
  if(f->timegroup<=0){
    fprintf(stderr,"Damnit!!! tried to select a timestep %d\n", f->timestep);
    exit(0);
  }
  /* lets open up a dataset in this group... we know "x" is there */
  /*  dataset=H5Dopen(f->timegroup,"x"); choice of X is a kludge of sorts */
  sprintf(name,"Particles#%d",f->timestep);
  while(H5Giterate(f->file, /* hid_t loc_id */
		   name,
		   &idx,
		   H5PartGetFirstDS,
		   &dataset)<0){}
  /*  space = H5Dget_space(dataset); */
  /* need to use H5PartGetDiskShape for any changes to f->diskshape */
  space = H5PartGetDiskShape(f,dataset); /* kludge (or utility depending on point of view) */
  if(H5PartHasView(f)){
    if(h5part_debug) fprintf(stderr,"\tGetNumParticles::get_selection\n");
    nparticles=H5Sget_select_npoints(space);
  }
  else {
    if(h5part_debug) fprintf(stderr,"\tGetNumParticles::get_simple_extent from space\n");
    nparticles= H5Sget_simple_extent_npoints(space);
  }
  if(space!=H5S_ALL) H5Sclose(space); /* release data shape handle */
  H5Dclose(dataset); /* release the dataset handle */
  return (long long)nparticles;
}


/**
  H5SetView: For parallel I/O or for subsetting operations on
  the datafile, the H5SetView subroutine allows you to define
  a subset of the total particle dataset to read.  The concept
  of "view" works for both serial and for parallel I/O.  The
  "view" will remain in effect until a new view is set, or the
  number of particles in a dataset changes, or the view is
    "unset" by calling H5SetView(file,-1,-1);

    Before you set a view, the H5PartGetNumParticles will
    return the total number of particles in a file (even for
    the parallel reads).  However, after you set a view, it
    will return the number of particles contained in the view.

		The range is inclusive (the start and the end index).

		\param f A FileHandle opened by H5PartOpenFile() or H5PartOpenFileParallel().
		\param start start particle
		\param end end particle
		\returns void
*/
void H5PartSetView(H5PartFile *f,long long start,long long end){
  hsize_t total,  stride, count;
#ifdef HDF5V160
  hssize_t range[2];
#else
  hsize_t range[2];
#endif
  int r;
  if(h5part_debug) fprintf(stderr,"SetView(%d,%d)\n",
	  (int)start,(int)end);
  if(f->mode==H5PART_WRITE || f->mode==H5PART_APPEND){
    fprintf(stderr,"H5PartSetView(): SetView does not make sense for write-only files.  It is meant to be used for read-only files. (maybe later this will change)\n");
    return;
  }
  /* if there is already a view selected, lets destroy it */ 
  f->viewstart=-1;
  f->viewend=-1;
  if(f->shape!=0){
    H5Sclose(f->shape);
    f->shape=0;
  }
  if(f->diskshape!=0 && f->diskshape!=H5S_ALL){
    H5Sclose(f->diskshape);
    f->diskshape=H5S_ALL;
  }
  f->diskshape = H5S_ALL;
  if(f->memshape!=0 && f->memshape!=H5S_ALL){
    H5Sclose(f->memshape);
    f->memshape=H5S_ALL;
  }
  if(start==-1 && end==-1) {
    if(h5part_debug) fprintf(stderr,"\tEarly Termination: Unsetting View\n");
    return; /* all done */
  }
  /* for now, we interpret start=-1 to mean 0 and 
     end==-1 to mean end of file */
  total=H5PartGetNumParticles(f);
  if(h5part_debug) fprintf(stderr,"\tTotal nparticles=%d\n",(int)total);
  if(start==-1) start=0;
  if(end==-1) end=total; /* can we trust nparticles (no)? 
					       fortunately, view has been reset
					      so H5PartGetNumParticles will tell
					      us the total number of particles. */

  /* so, is this selection inclusive or exclusive? 
     it appears to be inclusive for both ends of the range.
  */
  if(end<start) {
	fprintf(stderr,"H5PartSetView(min=%d, max=%d):  Nonfatal error.  End of view is less than start.\n",
		(int)start,(int)end);
	end=start; /* ensure that we don't have a range error */
  }
  range[0]=start;
  range[1]=end;
  /* setting up the new view */
  f->viewstart=range[0]; /* inclusive start */
  f->viewend=range[1]; /* inclusive end */
  f->nparticles=range[1]-range[0];
  if(h5part_debug) fprintf(stderr,"\tRange is now %d:%d\n",(int)range[0],(int)range[1]);
  /* OK, now we must create a selection from this */

  /* how to check shape... is default H5S_ALL? */
  f->shape = H5Screate_simple(1,&total,&total); /* declare overall datasize */
  /*  f->diskshape = H5S_ALL; */
  f->diskshape= H5Screate_simple(1,&total,&total); /* declare overall data size  but then will select a subset */
  {
    hsize_t dmax=H5S_UNLIMITED;
  f->memshape = H5Screate_simple(1,&(f->nparticles),&dmax); /* declare local memory datasize */
  }
  
  if(f->shape<0 || f->memshape<0 || f->diskshape<0){
    fprintf(stderr,"Abort: shape construction failed\n");
    if(f->shape<0) fprintf(stderr,"\tf->shape\n");
    if(f->diskshape<0) fprintf(stderr,"\tf->diskshape\n");
    if(f->memshape<0) fprintf(stderr,"\tf->memshape\n");
    exit(0);
  }
  if(h5part_debug) fprintf(stderr,"\tcount=%d\n",(int)total);
  stride=1;
  /* now we select a subset */
  r=H5Sselect_hyperslab(f->diskshape,H5S_SELECT_SET,range,&stride,&total,NULL);
  if(r<0){
    fprintf(stderr,"Abort: Selection Failed!\n");
    exit(0);
  }
     /* OK, now we have selected a reasonable hyperslab (all done) */
}

/**
  H5PartGetView: Allows you to query the current view. Start and End
  will be -1 if there is no current view established.
	Use H5PartHasView to see if the view is smaller than the
  total dataset.
  \param f A FileHandle opened by H5PartOpenFile() or H5PartOpenFileParallel().
  \param start start particle
  \param end end particle
  \returns the number of elements in the view 
 */
int H5PartGetView(H5PartFile *f,long long *start,long long *end){
  long long range[2];
  range[0]=(f->viewstart>=0)?f->viewstart:0;
  range[1]=(f->viewend>=0)?f->viewend:H5PartGetNumParticles(f);
  if(start) {
    *start=range[0];
  }
  if(end) {
    *end=range[1];
  }
  /* we could return the number of elements in the View as a convenience */
  return range[1]-range[0];
}

/** 
	H5SetCanonicalView: If it is too tedious to manually set the
		start and end coordinates for a view, the H5SetCanonicalView()
		will automatically select an appropriate domain decomposition of
		the data arrays for the degree of parallelism and set the "view" 
		accordingly.
		\param f A FileHandle opened by H5PartOpenFile() or H5PartOpenFileParallel().
		\returns void
*/
void H5PartSetCanonicalView(H5PartFile *f){
  /* if a read_only file, search for on-disk canonical view */
  /* if this view does not exist, then if MPI, subdivide by numprocs */
  /* else, "unset" any existing View */
  if(f->mode != H5PART_READ){
    fprintf(stderr,"H5PartSetCanonicalView(): Views do not make sense for write-only files.  It is meant to be used for read-only files. (maybe later this will change)\n");
    return;
  }
  H5PartSetView(f,-1,-1); /* unset the view */
#ifdef PARALLEL_IO
  if(f->timegroup<0){
    H5PartSetStep(f,0); /* set to first step in file */
  }
  /* 
     now lets query the attributes for this group to see if there is
     a 'pnparticles' group that contains the offsets for the processors.
  */
  
  if(H5PartReadStepAttrib(f,"pnparticles",f->pnparticles)<0){ /* try to read pnparticles right off of the disk */
    /* automatically subdivide the view into NP mostly equal pieces */
    int i;
    long long total=0, n = H5PartGetNumParticles(f);
    n/=f->nprocs;
    f->pnparticles[0]=n;
    total=n;
    for(i=1;i<f->nprocs;i++){
      f->pnparticles[i]=n;
      total+=n;
    }
  }
  {
    int i;
    long long total = 0, n = H5PartGetNumParticles(f);
    /* now we set the view for this processor */
    for(i=0;i<f->myproc;i++){
      total+=f->pnparticles[i];
    }
    H5PartSetView(f,total,total+f->pnparticles[f->myproc]-1);
  }
#endif
  /* the canonical view is to see everything if this is serial
     so there is nothing left to do */
}

/**
  H5PartReadDataFloat64:  This reads in an individual array from a
    particlar timestep.
    If you haven't selected a particular timestep, it will pick
    the current one.
		\param f A FileHandle opened by H5PartOpenFile() or H5PartOpenFileParallel().
		\param name A null-terminated string for the name of the array. When retrieving datasets from disk, you ask for them by name. There are no restrictions on naming of arrays, but it is useful to arrive at some common naming convention when sharing data with other groups.
		\param array  A buffer to read array of particle data. The datatype for elements in the buffer is implicit in the name of the subroutine call.
		\returns 1 on success, 0 on failure.
*/
int H5PartReadDataFloat64(H5PartFile *f,char *name,double *array){
  hid_t space,memspace,dataset,datatype;
  if(!f->timegroup) H5PartSetStep(f,f->timestep); /* choose current step */
  dataset=H5Dopen(f->timegroup,name);
  space = H5PartGetDiskShape(f,dataset); /* gets space with selection if view is set */
  memspace = H5PartGetMemShape(f,dataset);
  /*  datatype=H5Dget_type(dataset); */
  H5Dread(dataset, /* handle for the dataset */
	  H5T_NATIVE_DOUBLE, /* the datatype we use in memory 
			      you can change it to FLOAT if you want */
	  memspace, /* shape/size of data in memory (the complement to disk hyperslab) */
	  space, /* shape/size of data on disk  (get hyperslab if needed) */
	  H5P_DEFAULT,/* ignore... its for parallel reads */
	  array); /* the data array we are reading into */
  if(space!=H5S_ALL) H5Sclose(space); /* release data shape handle */
  if(memspace!=H5S_ALL) H5Sclose(memspace);
  H5Dclose(dataset); /* release the dataset handle */
  return 1;
}

/**
  H5PartReadDataInt64:  This reads in an individual array from a
    particlar timestep.
    If you haven't selected a particular timestep, it will pick
    the current one.
		\param f A FileHandle opened by H5PartOpenFile() or H5PartOpenFileParallel().
		\param name A null-terminated string for the name of the array. When retrieving datasets from disk, you ask for them by name. There are no restrictions on naming of arrays, but it is useful to arrive at some common naming convention when sharing data with other groups.
		\param array  A buffer to read array of particle data. The datatype for elements in the buffer is implicit in the name of the subroutine call.
		\returns 1 on success, 0 on failure.
*/
int H5PartReadDataInt64(H5PartFile *f,char *name,long long *array){
  hid_t space,memspace,dataset,datatype;
  if(!f->timegroup) H5PartSetStep(f,f->timestep); /* choose a step */
  dataset=H5Dopen(f->timegroup,name);
  space = H5PartGetDiskShape(f,dataset); /* H5Dget_space(dataset); */
  memspace = H5PartGetMemShape(f,dataset);
  /*  datatype=H5Dget_type(dataset); */
  H5Dread(dataset, /* handle for the dataset */
	  H5T_NATIVE_INT64, /* the datatype we use in memory 
			      you can change it to FLOAT if you want */
	  memspace, /* shape/size of data in memory (complement to disk hyperslab) */
	  space, /* shape/size of data on disk  (currently get all) */
	  H5P_DEFAULT,/* ignore... its for parallel reads */
	  array); /* the data array we are reading into */
  if(space!=H5S_ALL) H5Sclose(space); /* release data shape handle */
  if(memspace!=H5S_ALL) H5Sclose(memspace);
  H5Dclose(dataset); /* release the dataset handle */
  return 1; /* totally bogus */
}

/**
  H5PartReadParticleStep:  This is the mongo read function that
    pulls in all of the data for a given timestep in one shot.
    It also takes the timestep as an argument and will call
    H5PartSetStep() internally so that you don't have to 
    make that call separately.
    See also: H5PartReadArray() if you want to just
    read in one of the many arrays.
*/
int H5PartReadParticleStep(H5PartFile *f,
			   int step,
			   double *x,double *y,double *z,
                           double *px,double *py,double *pz,
			   long long *id){
  H5PartSetStep(f,step);
  /* or smuggle it into the array names */
  H5PartReadDataFloat64(f,"x",x);
  H5PartReadDataFloat64(f,"y",y);
  H5PartReadDataFloat64(f,"z",z);
  H5PartReadDataFloat64(f,"px",px);
  H5PartReadDataFloat64(f,"py",py);
  H5PartReadDataFloat64(f,"pz",pz);
  H5PartReadDataInt64(f,"id",id);
  return 1;
}

/**************** File Stashing Interfaces *************************/
/**
	H5NameExists: Private
*/

herr_t H5NameExists(hid_t group_id,
		   const char *member_name,
		     void *v){
  if(!strcmp(member_name,(char*)v)) return 1;
  else return 0;
}

/**
	H5PartFileHasName: Private
*/
int H5PartFileHasName(H5PartFile *f,
		      char *dir,char *name){
  if(H5Giterate(f->file, /* hid_t loc_id, */
		dir, /*const char *name, */
		NULL, /* int *idx, */
		H5NameExists,
		(void*)name)<0)
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
int H5PartStashFile(H5PartFile *f,char *filename){
  hid_t udata=0,files=0,rgroup=0;
  FILE *file;
  int returnvalue=0;
  
  rgroup = H5Gopen(f->file,"/");
  if(H5PartFileHasName(f,"/","UserData")){
    udata = H5Gopen(rgroup,"UserData");
    if(H5PartFileHasName(f,"UserData","Files")){
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


int H5PartUnstashFile(H5PartFile *f,char *filename, char *outputpath){
  if(H5PartFileHasName(f,"/","UserData") && 
     H5PartFileHasName(f,"/UserData","Files") && 
     H5PartFileHasName(f,"/UserData/Files",filename)){
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

int H5PartGetNumStashFiles(H5PartFile *f){
  hsize_t retval;
  if(H5PartFileHasName(f,"/","UserData") && 
     H5PartFileHasName(f,"/UserData","Files")){
    hid_t fgroup;
    /* we will use an iterator for this to count stash files */
    fgroup = H5Gopen(f->file,"/UserData/Files");
    H5Gget_num_objs(fgroup,&retval);
    return retval;
  }
  else return 0;
}

int H5PartFileGetStashFileName(H5PartFile *f,int nameindex,char *filename,int maxnamelen){
  return 1;
}

