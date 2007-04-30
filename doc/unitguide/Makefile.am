MAINTAINERCLEANFILES	= Makefile.in

SUBDIRS = output

unitguide:
	mkdir -p output/html
	$(top_builddir)/source/tools/linux/asc_weaponguide -d output/html/  -s 100 -i -l html -t ../asc.css -m asc.css --roottech 101000,101913 --allbuildings
	$(top_builddir)/source/tools/linux/asc_weaponguide -d output/html/  -s 500 -i -l html -t ../asc.css -m asc.css --roottech 101000,101913 --allbuildings
	cd output 
        $(abs_top_srcdir)/doc/unitguide/menu.pl $(abs_top_srcdir)/doc/unitguide/output/entries.txt

EXTRA_DIST = menu.pl
