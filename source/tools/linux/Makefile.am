## Process this file with automake to produce Makefile.in
######################################################################
## $Id: Makefile.am,v 1.4 2000-08-03 19:21:42 mbickel Exp $

vpath %.cpp ..:../..

CXXFLAGS = @CXXFLAGS@ -Dconverter

bin_PROGRAMS = ascdemount ascmount

ascdemount_SOURCES = demount.cpp basestrm.cpp misc.cpp

ascmount_SOURCES = mount.cpp basestrm.cpp misc.cpp

ascmount_LDADD   = -lSDL
ascdemount_LDADD   = -lSDL

# vim: noexpandtab ai

