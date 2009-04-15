MAINTAINERCLEANFILES	= Makefile.in

SUBDIRS = output


unitguidehtml:
	mkdir -p output/html
	$(top_builddir)/source/tools/linux/asc_weaponguide -d output/html/  -s 100 -i -l html -t ../asc.css -m asc.css --roottech 101000,101913 --allbuildings
	$(top_builddir)/source/tools/linux/asc_weaponguide -d output/html/  -s 500 -i -l html -t ../asc.css -m asc.css --roottech 101000,101913 --allbuildings
	
unitguideallhtml:
	mkdir -p output/html
	$(top_builddir)/source/tools/linux/asc_weaponguide -d output/html/  -i -l html -t ../asc.css -m asc.css --allbuildings
	
unitguideallmenu:	
	echo 'Units;asc.css;-;' > output/entries-all.txt
	ls output/html/unitset_id*.groups | sed 's,^output/,.#,' >> output/entries-all.txt
	echo 'Buildings;asc.css;-;' >> output/entries-all.txt
	ls output/html/buildingset_id*.groups | sed 's,^output/,.#,' >> output/entries-all.txt
	cd output && "$(abs_top_srcdir)/doc/unitguide/menu.pl" "--input=$(abs_top_srcdir)/doc/unitguide/output/entries-all.txt"
	
unitguidemenu:	
	cd output && "$(abs_top_srcdir)/doc/unitguide/menu.pl" "--input=$(abs_top_srcdir)/doc/unitguide/output/entries.txt"

unitguide: unitguidehtml unitguidemenu

EXTRA_DIST = menu.pl
