## Process this file with automake to produce Makefile.in
######################################################################
## $Id: Makefile.am,v 1.6 2000-11-22 08:48:35 mbickel Exp $

vpath %.cpp ..:../..

CXXFLAGS = @CXXFLAGS@ -Dconverter

bin_PROGRAMS = ascdemount ascmount

ascdemount_SOURCES = demount.cpp basestrm.cpp misc.cpp

ascmount_SOURCES = mount.cpp basestrm.cpp misc.cpp

ascmount_LDADD   = -lSDL -lpthread @LDADDLIBBZ2@
ascdemount_LDADD   = -lSDL -lpthread @LDADDLIBBZ2@

# vim: noexpandtab ai


