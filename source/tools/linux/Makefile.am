## Process this file with automake to produce Makefile.in
######################################################################
## $Id: Makefile.am,v 1.7 2001-01-28 20:42:17 mbickel Exp $

vpath %.cpp ..:../..

CXXFLAGS = @CXXFLAGS@ -Dconverter

bin_PROGRAMS = ascdemount ascmount

ascdemount_SOURCES = demount.cpp basestrm.cpp misc.cpp ascstring.cpp

ascmount_SOURCES = mount.cpp basestrm.cpp misc.cpp ascstring.cpp

ascmount_LDADD   = -lSDL -lpthread @LDADDLIBBZ2@
ascdemount_LDADD   = -lSDL -lpthread @LDADDLIBBZ2@

# vim: noexpandtab ai


