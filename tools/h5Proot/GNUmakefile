#---------------------------------------------------
# get compilation and library flags from root-config

ROOTCFLAGS    = $(shell $(ROOTSYS)/bin/root-config --cflags)
ROOTLIBS      = $(shell $(ROOTSYS)/bin/root-config --libs)
ROOTGLIBS     = $(shell $(ROOTSYS)/bin/root-config --glibs)

# depending on the architecture, choose the compiler,
# linker, and the flags to use
#

# -- Linux
ifeq ($(OSTYPE),linux)
CXX           = g++
CXXFLAGS      = -g -Wall -fPIC
LD            = g++
LDFLAGS       = -g
SOFLAGS       = -shared
endif

# the output from the root-config script:
CXXFLAGS      += $(ROOTCFLAGS)
LDFLAGS       += -lhdf5_cpp -lhdf5

# include paths
CXX           += -I$(HDF5)/include 
CXX           += -L$(HDF5)/lib
CXX           += -I$(H5Part)/src/

# include H5Part.o in linking
##LD            += $(H5Part)/src/H5Part.o

# the ROOT libraries (G = graphic)
LIBS          = $(ROOTLIBS)
GLIBS         = $(ROOTGLIBS)

LIBS          += -lhdf5_cpp -lhdf5
LIBS          += -L$(HDF5)/lib

GLIBS         += -lhdf5_cpp -lhdf5
GLIBS         += -L$(HDF5)/lib

# some definitions: headers, sources, objects,...
OBJS          =  H5root.o 
OBJS          += TH5Dataset.o TH5DatasetDict.o 
OBJS          += TH5MainFrame.o TH5MainFrameDict.o 
OBJS          += TH5Legend.o TH5LegendDict.o 
PROGRAM       = H5root

# the link command
#
all:            $(PROGRAM)
$(PROGRAM):     $(OBJS) 
		@echo "---> Linking $(PROGRAM) ..."
		@/bin/rm -f $(PROGRAM)
		@$(LD) $(OBJS)  -o $(PROGRAM) $(H5Part)/src/H5Part.o $(GLIBS) 
		@chmod 555 $(PROGRAM)
		@echo "done"

# clean up: remove all object file (and core files)
# semicolon needed to tell make there is no source
# for this target!
#
clean:; 	@rm -f $(OBJS) *Dict* core* $(PROGRAM)
		@echo "---> removing $(OBJS) and dictionaries"
###
$(OBJS): %.o: %.cc H5Style.h 
		$(CXX) $(CXXFLAGS) -c $<

# Create the dictionaries with rootcint:
# (temporarily remove H5Part.hh because rootcint cannot find it)
DICT          =  TH5DatasetDict.cc TH5MainFrameDict.cc
$(DICT): TH5Dataset.h TH5MainFrame.h
		@echo "Generating dictionary for TH5Dataset and TH5MainFrame"
		@awk '$$0!~/H5Part.hh/ {print $$0}' TH5Dataset.h > TH5Dataset.h_
		@mv TH5Dataset.h TH5Dataset.h_orig
		@mv TH5Dataset.h_ TH5Dataset.h
		@rootcint -f TH5DatasetDict.cc -c TH5Dataset.h
		@rootcint -f TH5MainFrameDict.cc -c TH5MainFrame.h
		@rootcint -f TH5LegendDict.cc -c TH5Legend.h
		@mv -f TH5Dataset.h_orig TH5Dataset.h
#---------------------------------------------------
