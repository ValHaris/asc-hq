## Process this file with automake to produce Makefile.in
######################################################################
## $Id: Makefile.am,v 1.1 2000-05-05 21:15:05 mbickel Exp $

vpath %.cpp ..:../..

CXXFLAGS = @CXXFLAGS@ -Usgmain -Dconverter

bin_PROGRAMS = demount ascmount

demount_SOURCES = demount.cpp basestrm.cpp misc.cpp

ascmount_SOURCES = mount.cpp basestrm.cpp misc.cpp

# vim: noexpandtab ai

