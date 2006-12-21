MAINTAINERCLEANFILES	= Makefile.in

SUBDIRS = output

unitguide:
	mkdir -p output/html
	$(top_srcdir)/source/tools/linux/asc_weaponguide -d output/html/  -s 100 -i -l html -t ../asc.css -m asc.css --roottech 101000,101913 --allbuildings
	$(top_srcdir)/source/tools/linux/asc_weaponguide -d output/html/  -s 500 -i -l html -t ../asc.css -m asc.css --roottech 101000,101913 --allbuildings
	cd output && $(top_srcdir)/../menu.pl

EXTRA_DIST = menu.pl