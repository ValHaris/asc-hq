MAINTAINERCLEANFILES	= Makefile.in

SUBDIRS 		= src
EXTRA_DIST=$(srcdir)/include/*.h $(srcdir)/VisualC/paragui_dynamic.vcproj 

paraguidoc: paragui.doxy
	doxygen paragui.doxy
