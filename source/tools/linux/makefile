#####################################################################
# Note: this is a very early Makefile
#####################################################################
all: demount mountcon

demount: ../demount.cpp
	g++ -g -o demount ../demount.cpp ../../basestrm.o ../../misc.o -lbz2
	
mountcon: ../mount.cpp
	g++ -g -o mountcon ../mount.cpp ../../basestrm.o ../../misc.o -lbz2

# vim: noexpandtab


