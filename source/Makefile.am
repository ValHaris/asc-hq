bin_PROGRAMS = asc
asc_SOURCES = unitctrl.cpp soundList.cpp weather.cpp typen.cpp strtmesg.cpp \
stack.cpp spfst.cpp sgstream.cpp sg.cpp pd.cpp palette.cpp newfont.cpp \
network.cpp missions.cpp misc.cpp loadpcxc.cpp loaders.cpp loadbi3.cpp \
gui.cpp gamedlg.cpp dlg_box.cpp dialog.cpp controls.cpp building.cpp \
basestrm.cpp basegfx.cpp attack.cpp artint.cpp CLoadable.cpp Property.cpp \
PropertyGroup.cpp gameoptions.cpp Named.cpp astar2.cpp vehicletype.cpp \
buildingtype.cpp containerbase.cpp mapalgorithms.cpp viewcalculation.cpp \
gamemap.cpp
asc_LDADD   = ./sdl/libsdl.a ./libs/triangul/libtriangul.a -lSDL -lpthread \
-lSDL_image @LDADDLIBBZ2@

SUBDIRS = docs sdl libs mapedit tools

EXTRA_DIST = attack.cpp basegfx.cpp basestrm.cpp building.cpp controls.cpp dialog.cpp dlg_box.cpp gamedlg.cpp gui.cpp loadbi3.cpp loaders.cpp loadpcxc.cpp misc.cpp missions.cpp network.cpp newfont.cpp palette.cpp pd.cpp sg.cpp sgstream.cpp spfst.cpp stack.cpp strtmesg.cpp typen.cpp weather.cpp attack.h basegfx.h basegui.h basestrm.h basetemp.h building.h controls.h dialog.h dlg_box.h gamedlg.h global.h gui.h keybp.h loadbi3.h loaders.h loadjpg.h loadpcx.h misc.h missions.h mousehnd.h network.h newfont.h palette.h pd.h sg.h sgstream.h spfldutl.h spfst.h stack.h strtmesg.h timer.h typen.h weather.h soundList.cpp soundList.h unitctrl.cpp unitctrl.h artint.cpp vehicletype.cpp buildingtype.cpp

CXXFLAGS = @CXXFLAGS@ -I./ -Dsgmain -DFREEMAPZOOM -DNEWKEYB

