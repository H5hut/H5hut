%module H5Part
%{
#include "H5Part.h"
#include "/usr/local/hdf5/include/H5pubconf.h"
#include "/usr/local/hdf5/include/H5api_adpt.h"
#include "/usr/local/hdf5/include/H5Tpublic.h"
#include "/usr/local/hdf5/include/H5public.h"
#include "/usr/local/hdf5/include/H5Ipublic.h"
%}

%include "H5Part.h"

%include "/usr/local/hdf5/include/H5pubconf.h"
%include "/usr/local/hdf5/include/H5api_adpt.h"
%include "/usr/local/hdf5/include/H5Tpublic.h"
%include "/usr/local/hdf5/include/H5public.h"
%include "/usr/local/hdf5/include/H5Ipublic.h"

%include "cstring.i"
%cstring_bounded_output(char* name, 100)
%include "typemaps.i"
int H5PartGetDatasetName(H5PartFile*, int, char* name, size_t);
int H5PartReadDataFloat64(H5PartFile*, char *INPUT, double*);
int H5PartReadDataInt64(H5PartFile*, char *INPUT, long long*);
void H5PartGetStepAttribInfo(H5PartFile*, int, char *name, size_t, int *OUTPUT, int *OUTPUT);
void H5PartGetFileAttribInfo(H5PartFile*, int, char *name, size_t, int *OUTPUT, int *OUTPUT);
int H5PartReadStepAttrib(H5PartFile*, char*, void *OUTPUT);
void H5PartReadAttrib(H5PartFile*, char* , void *OUTPUT);
int H5PartReadFileAttrib(H5PartFile*, char*, void *OUTPUT);
int H5PartFileGetStashFileName(H5PartFile*, int, char *name, int);
void H5PartGetDatasetInfo(H5PartFile*, int, char *name, size_t, int *OUTPUT, long long *OUTPUT);


%include "carrays.i"
%array_class(long long, longArray);
%array_class(double, doubleArray);
%array_class(char, charArray);
%array_class(hid_t, hid_tArray);

