MAINTAINERCLEANFILES	= Makefile.in

BUILT_SOURCES 		= asc.cpp mapedit.cpp map2pcx.cpp mount.cpp \
demount.cpp viewid.cpp makegfx.cpp gfx2pcx.cpp tank2pcx.cpp bin2txt.cpp \
weaponguide.cpp

%.cpp %.h: %.gp
	genparse $< -l cpp -c cpp -o $*
