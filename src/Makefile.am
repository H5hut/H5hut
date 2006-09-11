# src level Makefile.am

# PATH SETTING (IMPORTED FROM CONFIGURE)
HDF5ROOT = @HDF5ROOT@
PHDF5ROOT = @PHDF5ROOT@
PYTHON_PATH = -I@PYTHON_PATH@
PY_WRAPPER = @PY_WRAPPER@

# COMPILERS
CC = @CC@
MPICC = @MPICC@

# COMPILER FLAGS
CFLAGS = @CFLAGS@
PCFLAGS = $(CFLAGS) -DPARALLEL_IO -DH5_HAVE_PARALLEL -DMPICH_IGNORE_CXX_SEEK

#INCLUDES
HDFINC = -I$(HDF5ROOT)/include
PHDFINC = -I$(PHDF5ROOT)/include
PINC = $(PHDFINC) $(MPIINC)
INC = $(HDFINC)
MPIINC = @MPIINC@

# H5Part header file location
H5PINC = -I@prefix@/include

# H5Part compiled library location
H5PLIB = -L@prefix@/lib

# HDF5 LIBRARY
HDFLIB = -L$(HDF5ROOT)/lib -lhdf5 -lz $(SZLIB) @LDFLAGS@

# SZ LIBRARY
SZLIB = @SZLIB@

# Extra files that I wish to include in the dist tar ball.
EXTRA_DIST = H5Part.cc H5Part++.cc H5Part++.hh TestUnderscoreC.c TestUnderscore.f COPYRIGHT H5Part.py H5Part_py.i H5Part_py_wrap.c## TO BE TAILORED LATER...

# Files that I don't want to include in the dist tar ball
nodist_include_HEADERS = @UNDERSCORE_H@

# What to build... Will be determined by configure script.
lib_LIBRARIES = @MTARGET@ $(PY_WRAPPER)

# Listing of all possible targets that I may build.
EXTRA_LIBRARIES = libH5Part.a libH5PartF.a libpH5Part.a libpH5PartF.a _H5Part.so

# Header files that I wish to install in $(prefix)/include
include_HEADERS = H5Part.inc H5Part.h H5Part.hh @UNDERSCORE_H@

# Listing of all possible headers that I may include
EXTRA_HEADERS = H5Part.inc H5Part.h H5Part.hh Underscore.h

# Listing of sources
libH5Part_a_SOURCES = H5Part.c

libH5PartF_a_SOURCES = H5Part.c H5PartF.c

libpH5Part_a_SOURCES = H5Part.c

libpH5PartF_a_SOURCES = H5Part.c H5PartF.c

_H5Part_so_SOURCES = H5Part.c H5Part_py_wrap.c

# Specific building instruction (What compilers to use...)
# ------------ Serial Lib build commands ------------
libH5Part.a: H5Part.o
	ar rucs libH5Part.a H5Part.o
 
libH5PartF.a: H5Part.o H5PartF.o
	ar rucs libH5PartF.a H5Part.o H5PartF.o

H5Part.o: H5Part.c H5Part.h
	$(CC) $(CFLAGS) $(INC) -c H5Part.c
 
H5PartF.o: H5PartF.c Underscore.h H5Part.h
	$(CC) $(CFLAGS) $(INC) -w -c H5PartF.c


# ----------- Build Parallel H5Part Stuff ------------

libpH5Part.a: pH5Part.o
	ar rucs libpH5Part.a pH5Part.o

libpH5PartF.a: pH5Part.o pH5PartF.o
	ar rucs libpH5PartF.a pH5Part.o pH5PartF.o

pH5Part.o: H5Part.c H5Part.h
	$(MPICC) $(PCFLAGS) $(PINC) -c H5Part.c -o pH5Part.o

pH5PartF.o: H5PartF.c Underscore.h H5Part.h
	$(MPICC) $(PCFLAGS) $(PINC) -w -c H5PartF.c -o pH5PartF.o


# ----------- Build Python Wrapper ------------

_H5Part.so: H5Part_py.o H5Part_py_wrap.o
	$(CC) -g -shared -o _H5Part.so H5Part_py.o H5Part_py_wrap.o $(HDFLIB)

H5Part_py.o: H5Part.c H5Part.h
	$(CC) -g -fPIC -c -o H5Part_py.o $(PYTHON_PATH) $(HDFINC) H5Part.c

H5Part_py_wrap.o: H5Part_py_wrap.c
	$(CC) -g -fPIC -c $(PYTHON_PATH) $(HDFINC) H5Part_py_wrap.c


clean:
	rm -f *~ *.o *.a *.pyc *.so

distclean: clean
	rm -f *.a
	rm -rf .libs
	rm Underscore.h
	rm Makefile
