bin_PROGRAMS = asc
asc_SOURCES = unitctrl.cpp soundList.cpp weather.cpp typen.cpp strtmesg.cpp \
stack.cpp spfst.cpp sgstream.cpp sg.cpp pd.cpp palette.cpp newfont.cpp \
network.cpp missions.cpp misc.cpp loadpcxc.cpp loaders.cpp loadbi3.cpp \
gui.cpp gamedlg.cpp dlg_box.cpp dialog.cpp controls.cpp building.cpp \
basestrm.cpp basegfx.cpp attack.cpp artint.cpp CLoadable.cpp Property.cpp \
PropertyGroup.cpp gameoptions.cpp Named.cpp astar2.cpp vehicletype.cpp \
buildingtype.cpp containerbase.cpp mapalgorithms.cpp viewcalculation.cpp \
gamemap.cpp password.cpp password_dialog.cpp replay.cpp research.cpp \
mapdisplay.cpp resourcenet.cpp dashboard.cpp ascstring.cpp graphicset.cpp \
vehicle.cpp buildings.cpp networkdata.cpp

asc_LDADD   = ./sdl/libsdl.a ./libs/triangul/libtriangul.a -lSDL -lpthread \
-lSDL_image @LDADDLIBBZ2@

SUBDIRS = docs sdl libs mapedit tools

EXTRA_DIST = *.cpp *.h *.inc

CXXFLAGS = @CXXFLAGS@ -I./ -Dsgmain -DFREEMAPZOOM -DNEWKEYB

