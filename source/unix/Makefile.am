vpath %.cpp ..

MAINTAINERCLEANFILES	= Makefile.in

INCLUDES		= -I$(top_builddir) -I$(top_src_dir) \
			  -I$(top_builddir)/source -I$(top_src_dir)/source

SUBDIRS 		= mapeditor

CXXFLAGS 		= @CXXFLAGS@ @SDL_CFLAGS@ @SDLMM_CXXFLAGS@ \
-Dsgmain -DFREEMAPZOOM -DNEWKEYB

bin_PROGRAMS		= asc

asc_SOURCES 		= unitctrl.cpp soundList.cpp weather.cpp typen.cpp \
strtmesg.cpp stack.cpp spfst.cpp sgstream.cpp sg.cpp pd.cpp palette.cpp \
newfont.cpp network.cpp missions.cpp misc.cpp loadpcxc.cpp loaders.cpp \
loadbi3.cpp gui.cpp gamedlg.cpp dlg_box.cpp dialog.cpp controls.cpp \
building.cpp basestrm.cpp basegfx.cpp attack.cpp CLoadable.cpp Property.cpp \
PropertyGroup.cpp gameoptions.cpp Named.cpp astar2.cpp vehicletype.cpp \
buildingtype.cpp containerbase.cpp mapalgorithms.cpp viewcalculation.cpp \
gamemap.cpp password.cpp password_dialog.cpp replay.cpp research.cpp \
mapdisplay.cpp resourcenet.cpp dashboard.cpp ascstring.cpp graphicset.cpp \
vehicle.cpp buildings.cpp networkdata.cpp textfileparser.cpp terraintype.cpp \
objecttype.cpp textfiletags.cpp itemrepository.cpp stringtokenizer.cpp
asc_LDADD   		= $(top_builddir)/source/libs/triangul/libtriangul.la \
$(top_builddir)/source/ai/libai.la \
$(top_builddir)/source/sdl/libsdl.la \
@LDADDLIBBZ2@
