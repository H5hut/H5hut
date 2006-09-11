# This file was created automatically by SWIG 1.3.29.
# Don't modify this file, modify the SWIG interface instead.
# This file is compatible with both classic and new-style classes.

import _H5Part
import new
new_instancemethod = new.instancemethod
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'PySwigObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0
del types


class H5PartFile(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, H5PartFile, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, H5PartFile, name)
    __repr__ = _swig_repr
    __swig_setmethods__["file"] = _H5Part.H5PartFile_file_set
    __swig_getmethods__["file"] = _H5Part.H5PartFile_file_get
    if _newclass:file = property(_H5Part.H5PartFile_file_get, _H5Part.H5PartFile_file_set)
    __swig_setmethods__["timestep"] = _H5Part.H5PartFile_timestep_set
    __swig_getmethods__["timestep"] = _H5Part.H5PartFile_timestep_get
    if _newclass:timestep = property(_H5Part.H5PartFile_timestep_get, _H5Part.H5PartFile_timestep_set)
    __swig_setmethods__["timegroup"] = _H5Part.H5PartFile_timegroup_set
    __swig_getmethods__["timegroup"] = _H5Part.H5PartFile_timegroup_get
    if _newclass:timegroup = property(_H5Part.H5PartFile_timegroup_get, _H5Part.H5PartFile_timegroup_set)
    __swig_setmethods__["properties"] = _H5Part.H5PartFile_properties_set
    __swig_getmethods__["properties"] = _H5Part.H5PartFile_properties_get
    if _newclass:properties = property(_H5Part.H5PartFile_properties_get, _H5Part.H5PartFile_properties_set)
    __swig_setmethods__["nparticles"] = _H5Part.H5PartFile_nparticles_set
    __swig_getmethods__["nparticles"] = _H5Part.H5PartFile_nparticles_get
    if _newclass:nparticles = property(_H5Part.H5PartFile_nparticles_get, _H5Part.H5PartFile_nparticles_set)
    __swig_setmethods__["shape"] = _H5Part.H5PartFile_shape_set
    __swig_getmethods__["shape"] = _H5Part.H5PartFile_shape_get
    if _newclass:shape = property(_H5Part.H5PartFile_shape_get, _H5Part.H5PartFile_shape_set)
    __swig_setmethods__["mode"] = _H5Part.H5PartFile_mode_set
    __swig_getmethods__["mode"] = _H5Part.H5PartFile_mode_get
    if _newclass:mode = property(_H5Part.H5PartFile_mode_get, _H5Part.H5PartFile_mode_set)
    __swig_setmethods__["maxstep"] = _H5Part.H5PartFile_maxstep_set
    __swig_getmethods__["maxstep"] = _H5Part.H5PartFile_maxstep_get
    if _newclass:maxstep = property(_H5Part.H5PartFile_maxstep_get, _H5Part.H5PartFile_maxstep_set)
    __swig_setmethods__["xfer_prop"] = _H5Part.H5PartFile_xfer_prop_set
    __swig_getmethods__["xfer_prop"] = _H5Part.H5PartFile_xfer_prop_get
    if _newclass:xfer_prop = property(_H5Part.H5PartFile_xfer_prop_get, _H5Part.H5PartFile_xfer_prop_set)
    __swig_setmethods__["create_prop"] = _H5Part.H5PartFile_create_prop_set
    __swig_getmethods__["create_prop"] = _H5Part.H5PartFile_create_prop_get
    if _newclass:create_prop = property(_H5Part.H5PartFile_create_prop_get, _H5Part.H5PartFile_create_prop_set)
    __swig_setmethods__["access_prop"] = _H5Part.H5PartFile_access_prop_set
    __swig_getmethods__["access_prop"] = _H5Part.H5PartFile_access_prop_get
    if _newclass:access_prop = property(_H5Part.H5PartFile_access_prop_get, _H5Part.H5PartFile_access_prop_set)
    __swig_setmethods__["diskshape"] = _H5Part.H5PartFile_diskshape_set
    __swig_getmethods__["diskshape"] = _H5Part.H5PartFile_diskshape_get
    if _newclass:diskshape = property(_H5Part.H5PartFile_diskshape_get, _H5Part.H5PartFile_diskshape_set)
    __swig_setmethods__["memshape"] = _H5Part.H5PartFile_memshape_set
    __swig_getmethods__["memshape"] = _H5Part.H5PartFile_memshape_get
    if _newclass:memshape = property(_H5Part.H5PartFile_memshape_get, _H5Part.H5PartFile_memshape_set)
    __swig_setmethods__["viewstart"] = _H5Part.H5PartFile_viewstart_set
    __swig_getmethods__["viewstart"] = _H5Part.H5PartFile_viewstart_get
    if _newclass:viewstart = property(_H5Part.H5PartFile_viewstart_get, _H5Part.H5PartFile_viewstart_set)
    __swig_setmethods__["viewend"] = _H5Part.H5PartFile_viewend_set
    __swig_getmethods__["viewend"] = _H5Part.H5PartFile_viewend_get
    if _newclass:viewend = property(_H5Part.H5PartFile_viewend_get, _H5Part.H5PartFile_viewend_set)
    def __init__(self, *args): 
        this = _H5Part.new_H5PartFile(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _H5Part.delete_H5PartFile
    __del__ = lambda self : None;
H5PartFile_swigregister = _H5Part.H5PartFile_swigregister
H5PartFile_swigregister(H5PartFile)

H5PART_READ = _H5Part.H5PART_READ
H5PART_WRITE = _H5Part.H5PART_WRITE
H5PART_APPEND = _H5Part.H5PART_APPEND
H5PartOpenFile = _H5Part.H5PartOpenFile
H5PartFileIsValid = _H5Part.H5PartFileIsValid
H5PartCloseFile = _H5Part.H5PartCloseFile
H5PartSetNumParticles = _H5Part.H5PartSetNumParticles
H5PartWriteDataFloat64 = _H5Part.H5PartWriteDataFloat64
H5PartWriteDataInt64 = _H5Part.H5PartWriteDataInt64
H5PartSetStep = _H5Part.H5PartSetStep
H5PartGetNumSteps = _H5Part.H5PartGetNumSteps
H5PartGetNumDatasets = _H5Part.H5PartGetNumDatasets
H5PartNormType = _H5Part.H5PartNormType
H5PartGetNumParticles = _H5Part.H5PartGetNumParticles
H5PartSetView = _H5Part.H5PartSetView
H5PartGetView = _H5Part.H5PartGetView
H5PartSetCanonicalView = _H5Part.H5PartSetCanonicalView
H5PartReadDataFloat64 = _H5Part.H5PartReadDataFloat64
H5PartReadDataInt64 = _H5Part.H5PartReadDataInt64
H5PartReadParticleStep = _H5Part.H5PartReadParticleStep
H5PartWriteStepAttrib = _H5Part.H5PartWriteStepAttrib
H5PartWriteFileAttrib = _H5Part.H5PartWriteFileAttrib
H5PartWriteAttrib = _H5Part.H5PartWriteAttrib
H5PartWriteFileAttribString = _H5Part.H5PartWriteFileAttribString
H5PartWriteStepAttribString = _H5Part.H5PartWriteStepAttribString
H5PartGetNumStepAttribs = _H5Part.H5PartGetNumStepAttribs
H5PartGetNumFileAttribs = _H5Part.H5PartGetNumFileAttribs
H5PartReadStepAttrib = _H5Part.H5PartReadStepAttrib
H5PartReadAttrib = _H5Part.H5PartReadAttrib
H5PartReadFileAttrib = _H5Part.H5PartReadFileAttrib
H5PartStashFile = _H5Part.H5PartStashFile
H5PartUnstashFile = _H5Part.H5PartUnstashFile
H5PartGetNumStashFiles = _H5Part.H5PartGetNumStashFiles
H5PartFileGetStashFileName = _H5Part.H5PartFileGetStashFileName
H5_CONVERT_DENORMAL_FLOAT = _H5Part.H5_CONVERT_DENORMAL_FLOAT
H5_DEV_T_IS_SCALAR = _H5Part.H5_DEV_T_IS_SCALAR
H5_GETTIMEOFDAY_GIVES_TZ = _H5Part.H5_GETTIMEOFDAY_GIVES_TZ
H5_HAVE_ALARM = _H5Part.H5_HAVE_ALARM
H5_HAVE_ATTRIBUTE = _H5Part.H5_HAVE_ATTRIBUTE
H5_HAVE_DIFFTIME = _H5Part.H5_HAVE_DIFFTIME
H5_HAVE_DLFCN_H = _H5Part.H5_HAVE_DLFCN_H
H5_HAVE_FEATURES_H = _H5Part.H5_HAVE_FEATURES_H
H5_HAVE_FILTER_DEFLATE = _H5Part.H5_HAVE_FILTER_DEFLATE
H5_HAVE_FILTER_FLETCHER32 = _H5Part.H5_HAVE_FILTER_FLETCHER32
H5_HAVE_FILTER_SHUFFLE = _H5Part.H5_HAVE_FILTER_SHUFFLE
H5_HAVE_FORK = _H5Part.H5_HAVE_FORK
H5_HAVE_FREXPF = _H5Part.H5_HAVE_FREXPF
H5_HAVE_FREXPL = _H5Part.H5_HAVE_FREXPL
H5_HAVE_FUNCTION = _H5Part.H5_HAVE_FUNCTION
H5_HAVE_GETHOSTNAME = _H5Part.H5_HAVE_GETHOSTNAME
H5_HAVE_GETPWUID = _H5Part.H5_HAVE_GETPWUID
H5_HAVE_GETRUSAGE = _H5Part.H5_HAVE_GETRUSAGE
H5_HAVE_GETTIMEOFDAY = _H5Part.H5_HAVE_GETTIMEOFDAY
H5_HAVE_INTTYPES_H = _H5Part.H5_HAVE_INTTYPES_H
H5_HAVE_IOCTL = _H5Part.H5_HAVE_IOCTL
H5_HAVE_LARGE_HSIZET = _H5Part.H5_HAVE_LARGE_HSIZET
H5_HAVE_LIBM = _H5Part.H5_HAVE_LIBM
H5_HAVE_LIBZ = _H5Part.H5_HAVE_LIBZ
H5_HAVE_LONGJMP = _H5Part.H5_HAVE_LONGJMP
H5_HAVE_LSEEK64 = _H5Part.H5_HAVE_LSEEK64
H5_HAVE_MEMORY_H = _H5Part.H5_HAVE_MEMORY_H
H5_HAVE_NETINET_IN_H = _H5Part.H5_HAVE_NETINET_IN_H
H5_HAVE_NETINET_TCP_H = _H5Part.H5_HAVE_NETINET_TCP_H
H5_HAVE_SETJMP_H = _H5Part.H5_HAVE_SETJMP_H
H5_HAVE_SIGACTION = _H5Part.H5_HAVE_SIGACTION
H5_HAVE_SIGNAL = _H5Part.H5_HAVE_SIGNAL
H5_HAVE_SNPRINTF = _H5Part.H5_HAVE_SNPRINTF
H5_HAVE_SOCKLEN_T = _H5Part.H5_HAVE_SOCKLEN_T
H5_HAVE_STAT_ST_BLOCKS = _H5Part.H5_HAVE_STAT_ST_BLOCKS
H5_HAVE_STDDEF_H = _H5Part.H5_HAVE_STDDEF_H
H5_HAVE_STDINT_H = _H5Part.H5_HAVE_STDINT_H
H5_HAVE_STDLIB_H = _H5Part.H5_HAVE_STDLIB_H
H5_HAVE_STRDUP = _H5Part.H5_HAVE_STRDUP
H5_HAVE_STREAM = _H5Part.H5_HAVE_STREAM
H5_HAVE_STRINGS_H = _H5Part.H5_HAVE_STRINGS_H
H5_HAVE_STRING_H = _H5Part.H5_HAVE_STRING_H
H5_HAVE_STRUCT_TIMEZONE = _H5Part.H5_HAVE_STRUCT_TIMEZONE
H5_HAVE_STRUCT_TM_TM_ZONE = _H5Part.H5_HAVE_STRUCT_TM_TM_ZONE
H5_HAVE_SYSTEM = _H5Part.H5_HAVE_SYSTEM
H5_HAVE_SYS_IOCTL_H = _H5Part.H5_HAVE_SYS_IOCTL_H
H5_HAVE_SYS_RESOURCE_H = _H5Part.H5_HAVE_SYS_RESOURCE_H
H5_HAVE_SYS_SOCKET_H = _H5Part.H5_HAVE_SYS_SOCKET_H
H5_HAVE_SYS_STAT_H = _H5Part.H5_HAVE_SYS_STAT_H
H5_HAVE_SYS_TIMEB_H = _H5Part.H5_HAVE_SYS_TIMEB_H
H5_HAVE_SYS_TIME_H = _H5Part.H5_HAVE_SYS_TIME_H
H5_HAVE_SYS_TYPES_H = _H5Part.H5_HAVE_SYS_TYPES_H
H5_HAVE_TIOCGETD = _H5Part.H5_HAVE_TIOCGETD
H5_HAVE_TIOCGWINSZ = _H5Part.H5_HAVE_TIOCGWINSZ
H5_HAVE_TM_GMTOFF = _H5Part.H5_HAVE_TM_GMTOFF
H5_HAVE_TM_ZONE = _H5Part.H5_HAVE_TM_ZONE
H5_HAVE_UNISTD_H = _H5Part.H5_HAVE_UNISTD_H
H5_HAVE_VSNPRINTF = _H5Part.H5_HAVE_VSNPRINTF
H5_HAVE_WAITPID = _H5Part.H5_HAVE_WAITPID
H5_HAVE_ZLIB_H = _H5Part.H5_HAVE_ZLIB_H
H5_PACKAGE_BUGREPORT = _H5Part.H5_PACKAGE_BUGREPORT
H5_PACKAGE_NAME = _H5Part.H5_PACKAGE_NAME
H5_PACKAGE_STRING = _H5Part.H5_PACKAGE_STRING
H5_PACKAGE_TARNAME = _H5Part.H5_PACKAGE_TARNAME
H5_PACKAGE_VERSION = _H5Part.H5_PACKAGE_VERSION
H5_PRINTF_LL_WIDTH = _H5Part.H5_PRINTF_LL_WIDTH
H5_SIZEOF_CHAR = _H5Part.H5_SIZEOF_CHAR
H5_SIZEOF_DOUBLE = _H5Part.H5_SIZEOF_DOUBLE
H5_SIZEOF_FLOAT = _H5Part.H5_SIZEOF_FLOAT
H5_SIZEOF_INT = _H5Part.H5_SIZEOF_INT
H5_SIZEOF_INT16_T = _H5Part.H5_SIZEOF_INT16_T
H5_SIZEOF_INT32_T = _H5Part.H5_SIZEOF_INT32_T
H5_SIZEOF_INT64_T = _H5Part.H5_SIZEOF_INT64_T
H5_SIZEOF_INT8_T = _H5Part.H5_SIZEOF_INT8_T
H5_SIZEOF_INT_FAST16_T = _H5Part.H5_SIZEOF_INT_FAST16_T
H5_SIZEOF_INT_FAST32_T = _H5Part.H5_SIZEOF_INT_FAST32_T
H5_SIZEOF_INT_FAST64_T = _H5Part.H5_SIZEOF_INT_FAST64_T
H5_SIZEOF_INT_FAST8_T = _H5Part.H5_SIZEOF_INT_FAST8_T
H5_SIZEOF_INT_LEAST16_T = _H5Part.H5_SIZEOF_INT_LEAST16_T
H5_SIZEOF_INT_LEAST32_T = _H5Part.H5_SIZEOF_INT_LEAST32_T
H5_SIZEOF_INT_LEAST64_T = _H5Part.H5_SIZEOF_INT_LEAST64_T
H5_SIZEOF_INT_LEAST8_T = _H5Part.H5_SIZEOF_INT_LEAST8_T
H5_SIZEOF_LONG = _H5Part.H5_SIZEOF_LONG
H5_SIZEOF_LONG_DOUBLE = _H5Part.H5_SIZEOF_LONG_DOUBLE
H5_SIZEOF_LONG_LONG = _H5Part.H5_SIZEOF_LONG_LONG
H5_SIZEOF_OFF_T = _H5Part.H5_SIZEOF_OFF_T
H5_SIZEOF_SHORT = _H5Part.H5_SIZEOF_SHORT
H5_SIZEOF_SIZE_T = _H5Part.H5_SIZEOF_SIZE_T
H5_SIZEOF_SSIZE_T = _H5Part.H5_SIZEOF_SSIZE_T
H5_SIZEOF_UINT16_T = _H5Part.H5_SIZEOF_UINT16_T
H5_SIZEOF_UINT32_T = _H5Part.H5_SIZEOF_UINT32_T
H5_SIZEOF_UINT64_T = _H5Part.H5_SIZEOF_UINT64_T
H5_SIZEOF_UINT8_T = _H5Part.H5_SIZEOF_UINT8_T
H5_SIZEOF_UINT_FAST16_T = _H5Part.H5_SIZEOF_UINT_FAST16_T
H5_SIZEOF_UINT_FAST32_T = _H5Part.H5_SIZEOF_UINT_FAST32_T
H5_SIZEOF_UINT_FAST64_T = _H5Part.H5_SIZEOF_UINT_FAST64_T
H5_SIZEOF_UINT_FAST8_T = _H5Part.H5_SIZEOF_UINT_FAST8_T
H5_SIZEOF_UINT_LEAST16_T = _H5Part.H5_SIZEOF_UINT_LEAST16_T
H5_SIZEOF_UINT_LEAST32_T = _H5Part.H5_SIZEOF_UINT_LEAST32_T
H5_SIZEOF_UINT_LEAST64_T = _H5Part.H5_SIZEOF_UINT_LEAST64_T
H5_SIZEOF_UINT_LEAST8_T = _H5Part.H5_SIZEOF_UINT_LEAST8_T
H5_SIZEOF___INT64 = _H5Part.H5_SIZEOF___INT64
H5_STDC_HEADERS = _H5Part.H5_STDC_HEADERS
H5_SYSTEM_SCOPE_THREADS = _H5Part.H5_SYSTEM_SCOPE_THREADS
H5_TIME_WITH_SYS_TIME = _H5Part.H5_TIME_WITH_SYS_TIME
H5T_NO_CLASS = _H5Part.H5T_NO_CLASS
H5T_INTEGER = _H5Part.H5T_INTEGER
H5T_FLOAT = _H5Part.H5T_FLOAT
H5T_TIME = _H5Part.H5T_TIME
H5T_STRING = _H5Part.H5T_STRING
H5T_BITFIELD = _H5Part.H5T_BITFIELD
H5T_OPAQUE = _H5Part.H5T_OPAQUE
H5T_COMPOUND = _H5Part.H5T_COMPOUND
H5T_REFERENCE = _H5Part.H5T_REFERENCE
H5T_ENUM = _H5Part.H5T_ENUM
H5T_VLEN = _H5Part.H5T_VLEN
H5T_ARRAY = _H5Part.H5T_ARRAY
H5T_NCLASSES = _H5Part.H5T_NCLASSES
H5T_ORDER_ERROR = _H5Part.H5T_ORDER_ERROR
H5T_ORDER_LE = _H5Part.H5T_ORDER_LE
H5T_ORDER_BE = _H5Part.H5T_ORDER_BE
H5T_ORDER_VAX = _H5Part.H5T_ORDER_VAX
H5T_ORDER_NONE = _H5Part.H5T_ORDER_NONE
H5T_SGN_ERROR = _H5Part.H5T_SGN_ERROR
H5T_SGN_NONE = _H5Part.H5T_SGN_NONE
H5T_SGN_2 = _H5Part.H5T_SGN_2
H5T_NSGN = _H5Part.H5T_NSGN
H5T_NORM_ERROR = _H5Part.H5T_NORM_ERROR
H5T_NORM_IMPLIED = _H5Part.H5T_NORM_IMPLIED
H5T_NORM_MSBSET = _H5Part.H5T_NORM_MSBSET
H5T_NORM_NONE = _H5Part.H5T_NORM_NONE
H5T_CSET_ERROR = _H5Part.H5T_CSET_ERROR
H5T_CSET_ASCII = _H5Part.H5T_CSET_ASCII
H5T_CSET_RESERVED_1 = _H5Part.H5T_CSET_RESERVED_1
H5T_CSET_RESERVED_2 = _H5Part.H5T_CSET_RESERVED_2
H5T_CSET_RESERVED_3 = _H5Part.H5T_CSET_RESERVED_3
H5T_CSET_RESERVED_4 = _H5Part.H5T_CSET_RESERVED_4
H5T_CSET_RESERVED_5 = _H5Part.H5T_CSET_RESERVED_5
H5T_CSET_RESERVED_6 = _H5Part.H5T_CSET_RESERVED_6
H5T_CSET_RESERVED_7 = _H5Part.H5T_CSET_RESERVED_7
H5T_CSET_RESERVED_8 = _H5Part.H5T_CSET_RESERVED_8
H5T_CSET_RESERVED_9 = _H5Part.H5T_CSET_RESERVED_9
H5T_CSET_RESERVED_10 = _H5Part.H5T_CSET_RESERVED_10
H5T_CSET_RESERVED_11 = _H5Part.H5T_CSET_RESERVED_11
H5T_CSET_RESERVED_12 = _H5Part.H5T_CSET_RESERVED_12
H5T_CSET_RESERVED_13 = _H5Part.H5T_CSET_RESERVED_13
H5T_CSET_RESERVED_14 = _H5Part.H5T_CSET_RESERVED_14
H5T_CSET_RESERVED_15 = _H5Part.H5T_CSET_RESERVED_15
H5T_STR_ERROR = _H5Part.H5T_STR_ERROR
H5T_STR_NULLTERM = _H5Part.H5T_STR_NULLTERM
H5T_STR_NULLPAD = _H5Part.H5T_STR_NULLPAD
H5T_STR_SPACEPAD = _H5Part.H5T_STR_SPACEPAD
H5T_STR_RESERVED_3 = _H5Part.H5T_STR_RESERVED_3
H5T_STR_RESERVED_4 = _H5Part.H5T_STR_RESERVED_4
H5T_STR_RESERVED_5 = _H5Part.H5T_STR_RESERVED_5
H5T_STR_RESERVED_6 = _H5Part.H5T_STR_RESERVED_6
H5T_STR_RESERVED_7 = _H5Part.H5T_STR_RESERVED_7
H5T_STR_RESERVED_8 = _H5Part.H5T_STR_RESERVED_8
H5T_STR_RESERVED_9 = _H5Part.H5T_STR_RESERVED_9
H5T_STR_RESERVED_10 = _H5Part.H5T_STR_RESERVED_10
H5T_STR_RESERVED_11 = _H5Part.H5T_STR_RESERVED_11
H5T_STR_RESERVED_12 = _H5Part.H5T_STR_RESERVED_12
H5T_STR_RESERVED_13 = _H5Part.H5T_STR_RESERVED_13
H5T_STR_RESERVED_14 = _H5Part.H5T_STR_RESERVED_14
H5T_STR_RESERVED_15 = _H5Part.H5T_STR_RESERVED_15
H5T_PAD_ERROR = _H5Part.H5T_PAD_ERROR
H5T_PAD_ZERO = _H5Part.H5T_PAD_ZERO
H5T_PAD_ONE = _H5Part.H5T_PAD_ONE
H5T_PAD_BACKGROUND = _H5Part.H5T_PAD_BACKGROUND
H5T_NPAD = _H5Part.H5T_NPAD
H5T_CONV_INIT = _H5Part.H5T_CONV_INIT
H5T_CONV_CONV = _H5Part.H5T_CONV_CONV
H5T_CONV_FREE = _H5Part.H5T_CONV_FREE
H5T_BKG_NO = _H5Part.H5T_BKG_NO
H5T_BKG_TEMP = _H5Part.H5T_BKG_TEMP
H5T_BKG_YES = _H5Part.H5T_BKG_YES
class H5T_cdata_t(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, H5T_cdata_t, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, H5T_cdata_t, name)
    __repr__ = _swig_repr
    __swig_setmethods__["command"] = _H5Part.H5T_cdata_t_command_set
    __swig_getmethods__["command"] = _H5Part.H5T_cdata_t_command_get
    if _newclass:command = property(_H5Part.H5T_cdata_t_command_get, _H5Part.H5T_cdata_t_command_set)
    __swig_setmethods__["need_bkg"] = _H5Part.H5T_cdata_t_need_bkg_set
    __swig_getmethods__["need_bkg"] = _H5Part.H5T_cdata_t_need_bkg_get
    if _newclass:need_bkg = property(_H5Part.H5T_cdata_t_need_bkg_get, _H5Part.H5T_cdata_t_need_bkg_set)
    __swig_setmethods__["recalc"] = _H5Part.H5T_cdata_t_recalc_set
    __swig_getmethods__["recalc"] = _H5Part.H5T_cdata_t_recalc_get
    if _newclass:recalc = property(_H5Part.H5T_cdata_t_recalc_get, _H5Part.H5T_cdata_t_recalc_set)
    __swig_setmethods__["priv"] = _H5Part.H5T_cdata_t_priv_set
    __swig_getmethods__["priv"] = _H5Part.H5T_cdata_t_priv_get
    if _newclass:priv = property(_H5Part.H5T_cdata_t_priv_get, _H5Part.H5T_cdata_t_priv_set)
    def __init__(self, *args): 
        this = _H5Part.new_H5T_cdata_t(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _H5Part.delete_H5T_cdata_t
    __del__ = lambda self : None;
H5T_cdata_t_swigregister = _H5Part.H5T_cdata_t_swigregister
H5T_cdata_t_swigregister(H5T_cdata_t)

H5T_PERS_DONTCARE = _H5Part.H5T_PERS_DONTCARE
H5T_PERS_HARD = _H5Part.H5T_PERS_HARD
H5T_PERS_SOFT = _H5Part.H5T_PERS_SOFT
H5T_DIR_DEFAULT = _H5Part.H5T_DIR_DEFAULT
H5T_DIR_ASCEND = _H5Part.H5T_DIR_ASCEND
H5T_DIR_DESCEND = _H5Part.H5T_DIR_DESCEND
class hvl_t(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, hvl_t, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, hvl_t, name)
    __repr__ = _swig_repr
    __swig_setmethods__["len"] = _H5Part.hvl_t_len_set
    __swig_getmethods__["len"] = _H5Part.hvl_t_len_get
    if _newclass:len = property(_H5Part.hvl_t_len_get, _H5Part.hvl_t_len_set)
    __swig_setmethods__["p"] = _H5Part.hvl_t_p_set
    __swig_getmethods__["p"] = _H5Part.hvl_t_p_get
    if _newclass:p = property(_H5Part.hvl_t_p_get, _H5Part.hvl_t_p_set)
    def __init__(self, *args): 
        this = _H5Part.new_hvl_t(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _H5Part.delete_hvl_t
    __del__ = lambda self : None;
hvl_t_swigregister = _H5Part.hvl_t_swigregister
hvl_t_swigregister(hvl_t)

H5T_OPAQUE_TAG_MAX = _H5Part.H5T_OPAQUE_TAG_MAX
H5Topen = _H5Part.H5Topen
H5Tcreate = _H5Part.H5Tcreate
H5Tcopy = _H5Part.H5Tcopy
H5Tclose = _H5Part.H5Tclose
H5Tequal = _H5Part.H5Tequal
H5Tlock = _H5Part.H5Tlock
H5Tcommit = _H5Part.H5Tcommit
H5Tcommitted = _H5Part.H5Tcommitted
H5Tinsert = _H5Part.H5Tinsert
H5Tpack = _H5Part.H5Tpack
H5Tenum_create = _H5Part.H5Tenum_create
H5Tenum_insert = _H5Part.H5Tenum_insert
H5Tenum_nameof = _H5Part.H5Tenum_nameof
H5Tenum_valueof = _H5Part.H5Tenum_valueof
H5Tvlen_create = _H5Part.H5Tvlen_create
H5Tarray_create = _H5Part.H5Tarray_create
H5Tget_array_ndims = _H5Part.H5Tget_array_ndims
H5Tget_array_dims = _H5Part.H5Tget_array_dims
H5Tset_tag = _H5Part.H5Tset_tag
H5Tget_tag = _H5Part.H5Tget_tag
H5Tget_super = _H5Part.H5Tget_super
H5Tget_class = _H5Part.H5Tget_class
H5Tdetect_class = _H5Part.H5Tdetect_class
H5Tget_size = _H5Part.H5Tget_size
H5Tget_order = _H5Part.H5Tget_order
H5Tget_precision = _H5Part.H5Tget_precision
H5Tget_offset = _H5Part.H5Tget_offset
H5Tget_pad = _H5Part.H5Tget_pad
H5Tget_sign = _H5Part.H5Tget_sign
H5Tget_fields = _H5Part.H5Tget_fields
H5Tget_ebias = _H5Part.H5Tget_ebias
H5Tget_norm = _H5Part.H5Tget_norm
H5Tget_inpad = _H5Part.H5Tget_inpad
H5Tget_strpad = _H5Part.H5Tget_strpad
H5Tget_nmembers = _H5Part.H5Tget_nmembers
H5Tget_member_name = _H5Part.H5Tget_member_name
H5Tget_member_index = _H5Part.H5Tget_member_index
H5Tget_member_offset = _H5Part.H5Tget_member_offset
H5Tget_member_class = _H5Part.H5Tget_member_class
H5Tget_member_type = _H5Part.H5Tget_member_type
H5Tget_member_value = _H5Part.H5Tget_member_value
H5Tget_cset = _H5Part.H5Tget_cset
H5Tis_variable_str = _H5Part.H5Tis_variable_str
H5Tget_native_type = _H5Part.H5Tget_native_type
H5Tset_size = _H5Part.H5Tset_size
H5Tset_order = _H5Part.H5Tset_order
H5Tset_precision = _H5Part.H5Tset_precision
H5Tset_offset = _H5Part.H5Tset_offset
H5Tset_pad = _H5Part.H5Tset_pad
H5Tset_sign = _H5Part.H5Tset_sign
H5Tset_fields = _H5Part.H5Tset_fields
H5Tset_ebias = _H5Part.H5Tset_ebias
H5Tset_norm = _H5Part.H5Tset_norm
H5Tset_inpad = _H5Part.H5Tset_inpad
H5Tset_cset = _H5Part.H5Tset_cset
H5Tset_strpad = _H5Part.H5Tset_strpad
H5Tregister = _H5Part.H5Tregister
H5Tunregister = _H5Part.H5Tunregister
H5Tfind = _H5Part.H5Tfind
H5Tconvert = _H5Part.H5Tconvert
H5Tget_overflow = _H5Part.H5Tget_overflow
H5Tset_overflow = _H5Part.H5Tset_overflow
H5_VERS_MAJOR = _H5Part.H5_VERS_MAJOR
H5_VERS_MINOR = _H5Part.H5_VERS_MINOR
H5_VERS_RELEASE = _H5Part.H5_VERS_RELEASE
H5_VERS_SUBRELEASE = _H5Part.H5_VERS_SUBRELEASE
H5_VERS_INFO = _H5Part.H5_VERS_INFO
H5_SIZEOF_HSIZE_T = _H5Part.H5_SIZEOF_HSIZE_T
H5_SIZEOF_HADDR_T = _H5Part.H5_SIZEOF_HADDR_T
H5_PRINTF_HADDR_FMT = _H5Part.H5_PRINTF_HADDR_FMT
H5open = _H5Part.H5open
H5close = _H5Part.H5close
H5dont_atexit = _H5Part.H5dont_atexit
H5garbage_collect = _H5Part.H5garbage_collect
H5set_free_list_limits = _H5Part.H5set_free_list_limits
H5get_libversion = _H5Part.H5get_libversion
H5check_version = _H5Part.H5check_version
H5I_BADID = _H5Part.H5I_BADID
H5I_FILE = _H5Part.H5I_FILE
H5I_GROUP = _H5Part.H5I_GROUP
H5I_DATATYPE = _H5Part.H5I_DATATYPE
H5I_DATASPACE = _H5Part.H5I_DATASPACE
H5I_DATASET = _H5Part.H5I_DATASET
H5I_ATTR = _H5Part.H5I_ATTR
H5I_REFERENCE = _H5Part.H5I_REFERENCE
H5I_VFL = _H5Part.H5I_VFL
H5I_GENPROP_CLS = _H5Part.H5I_GENPROP_CLS
H5I_GENPROP_LST = _H5Part.H5I_GENPROP_LST
H5I_NGROUPS = _H5Part.H5I_NGROUPS
H5I_INVALID_HID = _H5Part.H5I_INVALID_HID
H5Iget_type = _H5Part.H5Iget_type
H5Iget_file_id = _H5Part.H5Iget_file_id
H5Iget_name = _H5Part.H5Iget_name
H5Iinc_ref = _H5Part.H5Iinc_ref
H5Idec_ref = _H5Part.H5Idec_ref
H5Iget_ref = _H5Part.H5Iget_ref
class longArray(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, longArray, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, longArray, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _H5Part.new_longArray(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _H5Part.delete_longArray
    __del__ = lambda self : None;
    def __getitem__(*args): return _H5Part.longArray___getitem__(*args)
    def __setitem__(*args): return _H5Part.longArray___setitem__(*args)
    def cast(*args): return _H5Part.longArray_cast(*args)
    __swig_getmethods__["frompointer"] = lambda x: _H5Part.longArray_frompointer
    if _newclass:frompointer = staticmethod(_H5Part.longArray_frompointer)
longArray_swigregister = _H5Part.longArray_swigregister
longArray_swigregister(longArray)
cvar = _H5Part.cvar
H5PartGetDatasetName = _H5Part.H5PartGetDatasetName
H5PartGetStepAttribInfo = _H5Part.H5PartGetStepAttribInfo
H5PartGetFileAttribInfo = _H5Part.H5PartGetFileAttribInfo
H5PartGetDatasetInfo = _H5Part.H5PartGetDatasetInfo
longArray_frompointer = _H5Part.longArray_frompointer

class doubleArray(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, doubleArray, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, doubleArray, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _H5Part.new_doubleArray(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _H5Part.delete_doubleArray
    __del__ = lambda self : None;
    def __getitem__(*args): return _H5Part.doubleArray___getitem__(*args)
    def __setitem__(*args): return _H5Part.doubleArray___setitem__(*args)
    def cast(*args): return _H5Part.doubleArray_cast(*args)
    __swig_getmethods__["frompointer"] = lambda x: _H5Part.doubleArray_frompointer
    if _newclass:frompointer = staticmethod(_H5Part.doubleArray_frompointer)
doubleArray_swigregister = _H5Part.doubleArray_swigregister
doubleArray_swigregister(doubleArray)
doubleArray_frompointer = _H5Part.doubleArray_frompointer

class charArray(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, charArray, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, charArray, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _H5Part.new_charArray(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _H5Part.delete_charArray
    __del__ = lambda self : None;
    def __getitem__(*args): return _H5Part.charArray___getitem__(*args)
    def __setitem__(*args): return _H5Part.charArray___setitem__(*args)
    def cast(*args): return _H5Part.charArray_cast(*args)
    __swig_getmethods__["frompointer"] = lambda x: _H5Part.charArray_frompointer
    if _newclass:frompointer = staticmethod(_H5Part.charArray_frompointer)
charArray_swigregister = _H5Part.charArray_swigregister
charArray_swigregister(charArray)
charArray_frompointer = _H5Part.charArray_frompointer

class hid_tArray(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, hid_tArray, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, hid_tArray, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _H5Part.new_hid_tArray(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _H5Part.delete_hid_tArray
    __del__ = lambda self : None;
    def __getitem__(*args): return _H5Part.hid_tArray___getitem__(*args)
    def __setitem__(*args): return _H5Part.hid_tArray___setitem__(*args)
    def cast(*args): return _H5Part.hid_tArray_cast(*args)
    __swig_getmethods__["frompointer"] = lambda x: _H5Part.hid_tArray_frompointer
    if _newclass:frompointer = staticmethod(_H5Part.hid_tArray_frompointer)
hid_tArray_swigregister = _H5Part.hid_tArray_swigregister
hid_tArray_swigregister(hid_tArray)
hid_tArray_frompointer = _H5Part.hid_tArray_frompointer



