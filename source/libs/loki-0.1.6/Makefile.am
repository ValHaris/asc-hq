
MAINTAINERCLEANFILES	= Makefile.in

noinst_LTLIBRARIES 	= libloki.la

vpath %.cpp $(srcdir)/src

# as the sources are in a subdirectory, we are grabbing them seperately ...
nodist_libloki_la_SOURCES	= SmallObj.cpp Singleton.cpp SmartPtr.cpp 

# ... which is here!
EXTRA_DIST= $(srcdir)/include/loki/*.h $(srcdir)/src/*.cpp
