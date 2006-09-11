#ifndef H5BLOCK_H
#define H5BLOCK_H

/*! 

    Interface for block structured field data

    This  is used to store and retrieve scalar or vector valued field data.
    With \f$  \Omega\ \f$  .... 

    Basic items are blocks (\f$ \Omaga_i \f$ ), cartesian subdomains of \f$ \Omega \f$ . If a  \f$ \Omega_i \f$ 
    is refined we view this \f$ \Omaga_i \f$  as a patch.
   
    \note My note  
   

	This is to discuss the HDF5 architecture:
	
    \timestep#k
	            \particles
				\topo
	            \block
	                 \patch1
							\block 	

	Idea: because we have more than one block per timestep
	in case of AMR, the activity of a block could help.


*/


//! Checks if the block of the actual time step is refined i.e. if we have a patch
/*!
      \param H5PartFile *f : file handle
      \param char *name    : name of the data set
*/
bool H5BlockIsPatch(H5PartFile *f, char *name);


//! return the maximum level refinement of refinement at the current time step
/*!
      \param H5PartFile *f : file handle
      \param char *name    : name of the data set
*/
int H5BlockGetMaxRefinementLevel(H5PartFile *f, char *name);



//! Define the field layout (FL) given the dense index space at the actual time step
/*!
	  \param H5PartFile *f : file handle
      \param char *name    : name of the data set
      \param unsigned int Istart : start of index I
      \param unsigned int Jstart : start of index J
      \param unsigned int Kstart : start of index K
      \param unsigned int Istart : end of index I
      \param unsigned int Jstart : end of index J
      \param unsigned int Kstart : end of index K
      \note this defines  \f$ \Omega \f$ and the view for HDF5	
	  
	  \note we have to make a 1D and 2D version
	  \note Q: what about a dimension independent version probably hard for the FOR-People?

*/
void H5BlockDefine3DFieldLayout(H5PartFile *f, 
	char *name, 	unsigned int Istart, unsigned int Iend,	
 	                        unsigned int Jstart, unsigned int Jend,	
	                        unsigned int Kstart, unsigned int Kend);	


//! Write the mesh spacing for the active (current) block 
/*!
	  \param H5PartFile *f : file handle
      \param char *name    : name of teh data set
      \param double dx: mesh spacing in x 
      \param double dy: mesh spacing in y 
      \param double dz: mesh spacing in z 
	  
*/
void H5BlockSetMeshSpacing(H5PartFile *f, char *name, double dx, double dy, double dz,);

//! Write a 3D real valued vector field using the defined FL for this block
/*!
      \param H5PartFile *f : file handle
      \param char *name    : name of teh data set
      \param double *xval : array of x component data
      \param double *yval : array of y component data
      \param double *zval : array of z component data

	  \note we have to make a 1D and 2D version
	  \note Q: what about a dimension independent version?
	  
*/
void H5BlockWrite3DVField(H5PartFile *f, 
	char *name, 	
	double *xval,	
	double *yval,	
	double *zval);	

// ===========================================================================================
/*
The following reflects the fact that we have fields which are decomposed into modes.

Augment the field name with the mode number and use this name to store the field.
For the mode freqency use the same procedure.

*/

	
	//! Read the number of stored modes at actual timestep 
	/*!
      \param H5PartFile *f : file handle
      \param char *name    : name of teh data set
	  
	*/
	int  H5BlockGetNumberOfModes(H5PartFile *f, char *name); 
	
	//! Set the number of stored modes at actual timestep 
	/*!
      \param H5PartFile *f : file handle
      \param char *name    : name of teh data set
	  \param int modes: the number of modes
	*/
	void H5BlockSetNumberOfModes(H5PartFile *f, char *name, int modes); 


	//! Write a 3D real valued vector field using the defined FL for this block
	/*!
      \param H5PartFile *f : file handle
      \param char *name : name of teh data set
	  \param int modeNumber : the mode number of the field
	  \param double omega :  the eingenmode of the field
      \param double *xval : array of x component data
      \param double *yval : array of y component data
      \param double *zval : array of z component data

	  \note we have to make a 1D and 2D version
	  \note Q: what about a dimension independent version?
	  
	*/
	void H5BlockWrite3DVField(H5PartFile *f, 
		char *name, int modeNumber, double omega, 	
		double *xval,	
		double *yval,	
		double *zval);	


	//! Write the quality factor for a given mode
	/*!
      \param H5PartFile *f : file handle
      \param char *name : name of the data set
	  \param int modeNumber : the mode number of the field
	  \param double Q: the quality factor	
	*/
	void H5BlockSetQFactor(H5PartFile *f, char *name, int mode, double Q);

	//! Write the S-parameters for a given mode
	/*!
      \param H5PartFile *f : file handle
      \param char *name : name of the data set
	  \param int modeNumber : the mode number of the field
	  \param double **S: the quality factor	
	*/	
	void H5BlockSetSParams(H5PartFile *f, char *name, int mode, double **s);

	//! Read the quality factor for a given mode
	/*!
      \param H5PartFile *f : file handle
      \param char *name : name of the data set
	  \param int modeNumber : the mode number of the field
	  \param double *Q: the quality factor	
	*/
	void H5BlockGetQFactor(H5PartFile *f, char *name, int mode, double *Q);

	//! Read the S-parameters for a given mode
	/*!
      \param H5PartFile *f : file handle
      \param char *name : name of the data set
	  \param int modeNumber : the mode number of the field
	  \param double **S: the quality factor	
	*/	
	void H5BlockGetSParams(H5PartFile *f, char *name, int mode, double **s);

#endif // H5BLOCK_H
