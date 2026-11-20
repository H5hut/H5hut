/*
  Copyright (c) 2026, ETH Zurich.
  License: see file COPYING in top level of source distribution.

  Smoke test for the HDF5 subfiling VFD in H5hut.

  All ranks jointly write one dataset through the subfiling VFD, then
  re-open the file and verify the data.  The test is skipped with status 0
  if the linked HDF5 library has no subfiling support.  IOC selection,
  stripe size and thread pool size can be exercised with the standard
  H5FD_SUBFILING_* and H5FD_IOC_* environment variables.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>

#include "H5hut.h"

#define NUM_PARTICLES 1024

int
main (
	int argc,
	char* argv[]
        ) {
	int provided = MPI_THREAD_SINGLE;
	MPI_Init_thread (&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
	if (provided < MPI_THREAD_MULTIPLE) {
		fprintf (stderr, "MPI_THREAD_MULTIPLE not available\n");
		MPI_Finalize ();
		return 1;
	}

	MPI_Comm comm = MPI_COMM_WORLD;
	int comm_rank = 0;
	int comm_size = 1;
	MPI_Comm_rank (comm, &comm_rank);
	MPI_Comm_size (comm, &comm_size);

#if !defined(H5_HAVE_PARALLEL) || !H5_VERSION_GE(1,14,0) || !defined(H5_HAVE_SUBFILING_VFD)
	if (comm_rank == 0)
		printf ("skipped: HDF5 lacks subfiling support\n");
	MPI_Finalize ();
	return 0;
#else
	int result = 1;

	double data[NUM_PARTICLES];
	for (int i = 0; i < NUM_PARTICLES; i++) {
		data[i] = comm_rank * 1000.0 + i;
	}

	// ---- write --------------------------------------------------
	h5_prop_t props = H5CreateFileProp ();
	// Selection: by default the subfiling VFD is selected with
	// H5SetPropFileSubfiling(); with H5HUT_VFD=subfiling in the
	// environment the selection happens inside H5OpenFile() instead,
	// without any code change in the caller.  The test covers both
	// paths.
	int env_selected = getenv ("H5HUT_VFD") != NULL;
	if (!env_selected &&
	    H5SetPropFileSubfiling (props, &comm) != H5_SUCCESS) {
		if (comm_rank == 0)
			fprintf (stderr, "H5SetPropFileSubfiling failed\n");
		goto cleanup;
	}
	if (comm_rank == 0)
		printf ("opening %s for writing\n", argv[0]);

	h5_file_t file = H5OpenFile ("test_h5u_subfiling.h5",
				     H5_O_WRONLY, props);
	if (file == H5_ERR || file == 0) {
		if (comm_rank == 0)
			fprintf (stderr, "H5OpenFile (write) failed\n");
		goto cleanup;
	}
	if (H5SetStep (file, 0) != H5_SUCCESS ||
	    H5PartSetNumParticles (file, NUM_PARTICLES) != H5_SUCCESS ||
	    H5PartWriteDataFloat64 (file, "data", data) != H5_SUCCESS) {
		if (comm_rank == 0)
			fprintf (stderr, "writing data failed\n");
		H5CloseFile (file);
		goto cleanup;
	}
	if (H5CloseFile (file) != H5_SUCCESS) {
		fprintf (stderr, "H5CloseFile failed\n");
		goto cleanup;
	}
	MPI_Barrier (comm);

	// ---- read back and verify ------------------------------------
	if (!env_selected &&
	    H5SetPropFileSubfiling (props, &comm) != H5_SUCCESS)
		goto cleanup;
	file = H5OpenFile ("test_h5u_subfiling.h5", H5_O_RDONLY, props);
	if (file == H5_ERR || file == 0) {
		if (comm_rank == 0)
			fprintf (stderr, "H5OpenFile (read) failed\n");
		goto cleanup;
	}
	{
		double read_data[NUM_PARTICLES];
		if (H5SetStep (file, 0) != H5_SUCCESS ||
		    H5PartSetNumParticles (file, NUM_PARTICLES) != H5_SUCCESS ||
		    H5PartReadDataFloat64 (file, "data", read_data) != H5_SUCCESS) {
			if (comm_rank == 0)
				fprintf (stderr, "reading data failed\n");
			H5CloseFile (file);
			goto cleanup;
		}
		H5CloseFile (file);

		for (int i = 0; i < NUM_PARTICLES; i++) {
			if (read_data[i] != data[i]) {
				fprintf (stderr,
					 "rank %d: data mismatch at %d: "
					 "%f != %f\n",
					 comm_rank, i, read_data[i], data[i]);
				goto cleanup;
			}
		}
	}
	result = 0;
	if (comm_rank == 0)
		printf ("passed: %d ranks x %d particles through subfiling VFD\n",
			comm_size, NUM_PARTICLES);

cleanup:
	H5CloseProp (props);
	MPI_Finalize ();
	return result;
#endif
}
