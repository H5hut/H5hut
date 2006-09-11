! Declaration of subroutines for Fortran Bindings
! open/close interface 
      INTEGER*8 h5pt_openr
!      FUNCTION h5pt_openr (file)
!         in CHARACTER file(*) : the filename to open for reading

      INTEGER*8 h5pt_openw
!     FUNCTION h5pt_openw(file) 
!       in CHARACTER file(*) : the filename to open for writing

      INTEGER*8 h5pt_openr_par
!     FUNCTION h5pt_openr_par(file,mpi_communicator)
!       in CHARACTER file(*) : the filename to open for writing
!       in INTEGER mpi_communicator : the MPI_Communicator used by the program
!       returns INTEGER*8 : and open filehandle for parallel reads

      INTEGER*8 h5pt_openw_par
!     FUNCTION h5pt_openw_par(file,mpi_communicator)
!       in CHARACTER file(*)
!       in INTEGER mpi_communicator
!       returns INTEGER*8 : an open filehandle for parallel writes

      INTEGER*8 h5pt_close
!     FUNCTION h5pt_close(filehandle)
!       in INTEGER*8 filehandle : close this open filehandle
      
!==============Writing and Setting Dataset info========
!     c h5pt_readstep(in INTEGER*8 filehandle,INTEGER step,
!     REAL*8 x(*),REAL*8 y(*),REAL*8 z(*),
!     REAL*8 px(*),REAL*8 py(*),REAL*8 pz(*),
!     INTEGER*8 id(*))
      
      INTEGER*8 h5pt_setnpoints
!     SUBROUTINE h5pt_setnpoints(filehandle,npoints)
!       in INTEGER*8 filehandle
!       in INTEGER*8 npoints : The number of particles on *this* processor
      
      INTEGER*8 h5pt_setstep
!     FUNCTION h5pt_setstep(filehandle,step)
!        in INTEGER*8 filehandle
!        in INTEGER*8 step  : Set the current timestep in the file to this
      
      INTEGER*8 h5pt_writedata_r8
!     SUBROUTINE h5pt_writedata_r8(filehandle,name,data)
!       in INTEGER*8 filehandle
!       in CHARACTER name(*) : The name of the data we are writing
!                 eg. "X" or "Y" or "PX" etc...
!       in REAL*8 data(*) : The dataarray to write.  The number of
!                        elements is presumably set earlier with
!                        h5pt_setnpoints(f,npoints)
      
      INTEGER*8 h5pt_writedata_i8
!     SUBROUTINE h5pt_writedata_i8(filehandle,name,data)
!       in INTEGER*8 filehandle
!       in CHARACTER name(*)
!       in INTEGER*8 data(*)
!==============Reading Data Characteristics============
      
      INTEGER*8 h5pt_getnsteps
!     FUNCTION h5pt_getnsteps(filehandle)
!       in INTEGER*8 filehandle
!       returns INTEGER : number of timesteps stored in the file
      
      INTEGER*8  h5pt_getndatasets
!     FUNCTION h5pt_getndatasets(filehandle)
!        in INTEGER*8 filehandle
!        returns INTEGER*8 : number of datasets stored per timestep
      
      INTEGER*8 h5pt_getnpoints
!     FUNCTION h5pt_getnpoints(filehandle)
!        in INTEGER*8 filehandle
!        returns INTEGER*8 : total number of points in this timestep
!                If a "view" has been set using h5pt_setview()
!                then it returns the number of points that are
!                in the current view.
      
!     probably should get index from name 
      INTEGER*8 h5pt_getdatasetname
!     FUNCTION h5pt_getdatasetname(filehandle,index,name)
!        in INTEGER*8 filehandle
!        in INTEGER index : Index for a given dataset name
!        out CHARACTER name(*) returns the name of the dataset at that index
      
!=============Setting and getting views================
      INTEGER*8 h5pt_setview
!     SUBROUTINE h5pt_setview(filehandle,start,end)
!        in INTEGER*8 filehandle
!        in INTEGER*8 start : offset of the first particle in the view
!        in INTEGER*8 end : offset of the first particle after the end of the view
      
      INTEGER*8 h5pt_resetview
!     SUBROUTINE h5pt_resetview(filehandle)
!         in INTEGER*8 filehandle : reset the view on this filehandle to default
      
      INTEGER*8 h5pt_hasview
!     FUNCTION h5pt_hasview(filehandle)
!         in INTEGER*8 filehandle
!         returns INTEGER : 1 if a view is in place and 0 if the view is default

      INTEGER*8 h5pt_getview
!     FUNCTION h5pt_getview(filehandle,start,end)
!         in INTEGER*8 filehandle
!         out INTEGER*8 start : offset of first particle in the view
!         out INTEGER*8 end  : offset of first particle beyond the current view

!==============Reading Data=========================
      INTEGER*8 h5pt_readdata_r8
!     FUNCTION h5pt_readdata_r8(filehandle,name,data)
!       in INTEGER*8 filehandle
!       in CHARACTER name(*) : The name of the data we are writing
!                 eg. "X" or "Y" or "PX" etc...
!       out REAL*8 data(*) : The dataarray to read. Number of points
!                     read is either the number within the view set
!                     by h5pt_setview() or the default (the total
!                     number of particles in the file.
!       returns INTEGER : 1 for success, 0 for failure
      INTEGER*8 h5pt_readdata_i8
!     FUNCTION h5pt_readdata_i8(filehandle,name,data)
!       in INTEGER*8 filehandle
!       in CHARACTER name(*) : The name of the data we are writing
!                 eg. "X" or "Y" or "PX" etc...
!       out REAL*8 data(*) : The dataarray to read. Number of points
!                     read is either the number within the view set
!                     by h5pt_setview() or the default (the total
!                     number of particles in the file.
!       returns INTEGER : 1 for success, 0 for failure
!     =================== Attributes ================
      
!     Attributes 
!     writing 
      INTEGER*8 h5pt_writefileattrib_r8
!     FUNCTION h5pt_writefileattrib_r8(filehandle,name,attrib,nelem)
!       in INTEGER*8 filehandle : The filehandle
!       in CHARACTER name(*) : The name of the attribute
!       in REAL*8 attrib(*) : The array of data to write into the attribute
!       in INTEGER nelem : Number of elements in the attrib array
      
      INTEGER*8 h5pt_writefileattrib_i8
!     FUNCTION h5pt_writefileattrib_i8(filehandle,name,attrib,nelem)
!       in INTEGER*8 filehandle : The filehandle
!       in CHARACTER name(*) : The name of the attribute
!       in INTEGER*8 attrib(*) : The array of data to write into the attribute
!       in INTEGER nelem : Number of elements in the attrib array
      
      INTEGER*8 h5pt_writefileattrib_string
!     FUNCTION h5pt_writefileattrib_string(filehandle,name,string)
!       in INTEGER*8 filehandle : The filehandle
!       in CHARACTER name(*) : The name of the attribute
!       in CHARACTER*8 attrib(*) : The array of data to write into the attribute
      
      INTEGER*8 h5pt_writestepattrib_r8
!     FUNCTION h5pt_writestepattrib_r8(filehandle,name,attrib,nelem)
!       in INTEGER*8 filehandle : The filehandle
!       in CHARACTER name(*) : The name of the attribute
!       in INTEGER*8 attrib(*) : The array of data to write into the attribute
!       in INTEGER nelem : Number of elements in the attrib array
         
      INTEGER*8 h5pt_writestepattrib_i8
!     FUNCTION h5pt_writestepattrib_i8(filehandle,name,attrib,nelem)
!       in INTEGER*8 filehandle : The filehandle
!       in CHARACTER name(*) : The name of the attribute
!       in INTEGER*8 attrib(*) : The array of data to write into the attribute
!       in INTEGER nelem : Number of elements in the attrib array
      
      INTEGER*8 h5pt_writestepattrib_string
!     FUNCTION h5pt_writestepattrib_string(filehandle,name,string)
!        in INTEGER*8 filehandle : The filehandle
!        in CHARACTER name(*) : The name of the attribute
!        in CHARACTER*8 attrib(*) : The array of data to write into the attribute

!     reading attributes ************************* 
      INTEGER*8 h5pt_getnstepattribs
!     FUNCTION h5pt_getnstepattribs(filehandle)
!       in INTEGER*8 filehandle : The filehandle
!       returns INTEGER : number of attributes bound to this particular step
      
      INTEGER*8 h5pt_getnfileattribs
!     FUNCTION h5pt_getnfileattribs(filehandle)
!       in INTEGER*8 filehandle : The filehandle
!       returns INTEGER : number of attributes bound to the file
      
      INTEGER*8 h5pt_getstepattribinfo
!     FUNCTION h5pt_getstepattribinfo(filehandle,idx,attribname,nelem)
!       in INTEGER*8 filehandle : The filehandle
!       in INTEGER idx : index of the attribute being queried
!       out CHARACTER name(*) : The name of the attribute
!       out INTEGER nelem : Number of elements in the attrib array
      
      
      INTEGER*8 h5pt_getfileattribinfo
!     FUNCTION h5pt_getfileattribinfo(filehandle,idx,attribname,nelem)
!       in INTEGER*8 filehandle : The filehandle
!       in INTEGER idx : index of the attribute being queried
!       out CHARACTER name(*) : The name of the attribute
!       out INTEGER nelem : Number of elements in the attrib array
      
      INTEGER*8 h5pt_readstepattrib
!     FUNCTION h5pt_readstepattrib(filehandle,name,data)
!       in INTEGER*8 filehandle
!       in CHARACTER attributename(*) : name of the attribute to read
!       out <anytype> data(*) : the attribute data will be read into this array
      
      INTEGER*8 h5pt_readfileattrib
!     FUNCTION h5pt_readfileattrib(filehandle,name,data)
!       in INTEGER*8 filehandle
!       in CHARACTER attributename(*) : name of the attribute to read
!       out <anytype> data(*) : the attribute data will be read into this array
  
