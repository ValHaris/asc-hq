## Process this file with automake to produce Makefile.in
######################################################################
## $Id: Makefile.am,v 1.5 2000-10-11 14:26:58 mbickel Exp $

vpath %.cpp ..:../..

CXXFLAGS = @CXXFLAGS@ -Dconverter

bin_PROGRAMS = ascdemount ascmount

ascdemount_SOURCES = demount.cpp basestrm.cpp misc.cpp

ascmount_SOURCES = mount.cpp basestrm.cpp misc.cpp

ascmount_LDADD   = -lSDL ../../libs/bzlib/libbz2.a
ascdemount_LDADD   = -lSDL ../../libs/bzlib/libbz2.a

# vim: noexpandtab ai

