# ---------------------------------------------------------------------------
!if !$d(BCB)
BCB = $(MAKEDIR)\..
!endif

# ---------------------------------------------------------------------------
# IDE SECTION
# ---------------------------------------------------------------------------
# The following section of the project makefile is managed by the BCB IDE.
# It is recommended to use the IDE to change any of the values in this
# section.
# ---------------------------------------------------------------------------

VERSION = BCB.06.00
# ---------------------------------------------------------------------------
PROJECT = ..\mapeditor.exe
OBJFILES = obj\SDL_main.obj obj\CLoadable.obj obj\getopt.obj obj\getopt1.obj \
    obj\Named.obj obj\Property.obj obj\PropertyGroup.obj obj\ascstring.obj \
    obj\attack.obj obj\basegfx.obj obj\basestrm.obj obj\buildings.obj \
    obj\buildingtype.obj obj\containerbase.obj obj\containerbasetype.obj \
    obj\dialog.obj obj\dlg_box.obj obj\edevents.obj obj\edgen.obj \
    obj\edglobal.obj obj\edmain.obj obj\edmisc.obj obj\edselfnt.obj \
    obj\gameevents.obj obj\gamemap.obj obj\gameoptions.obj obj\graphicset.obj \
    obj\itemrepository.obj obj\loadbi3.obj obj\loaders.obj obj\loadpcxc.obj \
    obj\mapalgorithms.obj obj\mapdisplay.obj obj\messages.obj obj\misc.obj \
    obj\networkdata.obj obj\newfont.obj obj\objecttype.obj obj\palette.obj \
    obj\paradialog.obj obj\password.obj obj\password_dialog.obj obj\pd.obj \
    obj\research.obj obj\SDLStretch.obj obj\asc_IMG_jpg.obj obj\events.obj \
    obj\graphics.obj obj\loadimage.obj obj\sound.obj obj\sgstream.obj \
    obj\spfst.obj obj\stack.obj obj\stringtokenizer.obj obj\strtmesg.obj \
    obj\terraintype.obj obj\textfile_evaluation.obj obj\textfileparser.obj \
    obj\textfiletags.obj obj\typen.obj obj\vehicle.obj obj\vehicletype.obj \
    obj\viewcalculation.obj obj\missions.obj obj\mission_dialog.obj \
    obj\soundList.obj obj\simplestream.obj obj\prehistoricevents.obj \
    obj\mappolygons.obj
RESFILES = 
MAINSOURCE = mapeditor.bpf
RESDEPEN = $(RESFILES)
LIBFILES = ..\..\..\..\..\SDL_image\bin\SDL_image.lib \
    ..\..\..\..\..\SDL_mixer\bin\SDL_mixer.LIB ..\..\..\..\..\sdl\bin\sdl.lib \
    ..\..\..\ai\ai.lib ..\..\..\libs\bzlib\win\bzlib.lib \
    ..\..\..\libs\jpeg-6b\libjpeg.lib ..\..\..\libs\sdlmm\SDLmm.lib \
    ..\..\..\libs\triangul\win32\triangulation.lib \
    ..\..\..\libs\libsigc++\libsigcpp.lib
IDLFILES = 
IDLGENFILES = 
LIBRARIES = bcb2kaxserver.lib indy.lib dbxcds.lib dclocx.lib soaprtl.lib bcbie.lib \
    nmfast.lib dbexpress.lib inetdbxpress.lib inetdb.lib inetdbbde.lib \
    inet.lib xmlrtl.lib visualdbclx.lib ibxpress.lib teeqr.lib tee.lib \
    teedb.lib teeui.lib bdecds.lib cds.lib dsnap.lib vcldbx.lib bdertl.lib \
    qrpt.lib adortl.lib dbrtl.lib vcldb.lib bcbsmp.lib vcl.lib rtl.lib
PACKAGES = rtl.bpi vcl.bpi vclx.bpi bcbsmp.bpi dclocx.bpi
SPARELIBS = rtl.lib vcl.lib bcbsmp.lib vcldb.lib dbrtl.lib adortl.lib qrpt.lib \
    bdertl.lib vcldbx.lib dsnap.lib cds.lib bdecds.lib teeui.lib teedb.lib \
    tee.lib teeqr.lib ibxpress.lib visualdbclx.lib xmlrtl.lib inet.lib \
    inetdbbde.lib inetdb.lib inetdbxpress.lib dbexpress.lib nmfast.lib \
    bcbie.lib soaprtl.lib dclocx.lib dbxcds.lib indy.lib bcb2kaxserver.lib
DEFFILE = 
OTHERFILES = 
# ---------------------------------------------------------------------------
DEBUGLIBPATH = $(BCB)\lib\debug
RELEASELIBPATH = $(BCB)\lib\release
USERDEFINES = karteneditor;HEXAGON;FREEMAPZOOM;_WIN32_;WIN32;_SDL_;_NOASM_;NEWKEYB;_DEBUG
SYSDEFINES = NO_STRICT;_NO_VCL
INCLUDEPATH = ..\..\..\libs\loki;..\..\..\..\..\..\Borland\CBuilder6\Source\Clx;..\..\..;..\..\..\..\..\sdl\src\main\win32;..\..\..\sdl;$(BCB)\include;$(BCB)\include\vcl;..\..\..\..\..\sdl\include;..\..;..\..\..\LIBS\getopt;..\..\..\..\..\sdl_mixer;..\..\..\..\..\SDLmm\src;..\..\..\..\..\SDL_image;..\..\..\..\..\paragui\include;..\..\..\..\..\freetype2\include;..\..\..\libs\libsigc++
LIBPATH = ..\..\..\libs\loki;..\..\..\..\..\..\Borland\CBuilder6\Source\Clx;..\..\..;..\..\..\LIBS\getopt;..\..\..\..\..\sdl\src\main\win32;..\..\..\sdl;$(BCB)\lib\obj;$(BCB)\lib
WARNINGS= -w-pck -w-par -w-8027 -w-8026 -w-csu -w-big
PATHCPP = .;..\..\..\..\..\sdl\src\main\win32;..\..\..;..\..\..\LIBS\getopt;..\..\..\sdl
PATHASM = .;
PATHPAS = .;
PATHRC = .;
PATHOBJ = .;$(LIBPATH)
# ---------------------------------------------------------------------------
CFLAG1 = -Od -Q -Vx -Ve -X- -r- -a1 -5 -b -k -y -v -vi- -tW -tWM -c -K
IDLCFLAGS = 
PFLAGS = -N2obj -N0obj -$Y+ -$W -$O- -$A8 -v -JPHNE -M
RFLAGS = 
AFLAGS = /mx /w2 /zd
LFLAGS = -Iobj -D"" -aa -Tpe -x -Gn -v
# ---------------------------------------------------------------------------
ALLOBJ = c0w32.obj $(OBJFILES)
ALLRES = $(RESFILES)
ALLLIB = $(LIBFILES) import32.lib cw32mt.lib
# ---------------------------------------------------------------------------
!ifdef IDEOPTIONS

[Version Info]
IncludeVerInfo=0
AutoIncBuild=0
MajorVer=1
MinorVer=0
Release=0
Build=0
Debug=0
PreRelease=0
Special=0
Private=0
DLL=0

[Version Info Keys]
CompanyName=
FileDescription=
FileVersion=1.0.0.0
InternalName=
LegalCopyright=
LegalTrademarks=
OriginalFilename=
ProductName=
ProductVersion=1.0.0.0
Comments=

[Debugging]
DebugSourceDirs=$(BCB)\source\vcl

!endif





# ---------------------------------------------------------------------------
# MAKE SECTION
# ---------------------------------------------------------------------------
# This section of the project file is not used by the BCB IDE.  It is for
# the benefit of building from the command-line using the MAKE utility.
# ---------------------------------------------------------------------------

.autodepend
# ---------------------------------------------------------------------------
!if "$(USERDEFINES)" != ""
AUSERDEFINES = -d$(USERDEFINES:;= -d)
!else
AUSERDEFINES =
!endif

!if !$d(BCC32)
BCC32 = bcc32
!endif

!if !$d(CPP32)
CPP32 = cpp32
!endif

!if !$d(DCC32)
DCC32 = dcc32
!endif

!if !$d(TASM32)
TASM32 = tasm32
!endif

!if !$d(LINKER)
LINKER = ilink32
!endif

!if !$d(BRCC32)
BRCC32 = brcc32
!endif


# ---------------------------------------------------------------------------
!if $d(PATHCPP)
.PATH.CPP = $(PATHCPP)
.PATH.C   = $(PATHCPP)
!endif

!if $d(PATHPAS)
.PATH.PAS = $(PATHPAS)
!endif

!if $d(PATHASM)
.PATH.ASM = $(PATHASM)
!endif

!if $d(PATHRC)
.PATH.RC  = $(PATHRC)
!endif

!if $d(PATHOBJ)
.PATH.OBJ  = $(PATHOBJ)
!endif
# ---------------------------------------------------------------------------
$(PROJECT): $(OTHERFILES) $(IDLGENFILES) $(OBJFILES) $(RESDEPEN) $(DEFFILE)
    $(BCB)\BIN\$(LINKER) @&&!
    $(LFLAGS) -L$(LIBPATH) +
    $(ALLOBJ), +
    $(PROJECT),, +
    $(ALLLIB), +
    $(DEFFILE), +
    $(ALLRES)
!
# ---------------------------------------------------------------------------
.pas.hpp:
    $(BCB)\BIN\$(DCC32) $(PFLAGS) -U$(INCLUDEPATH) -D$(USERDEFINES);$(SYSDEFINES) -O$(INCLUDEPATH) --BCB {$< }

.pas.obj:
    $(BCB)\BIN\$(DCC32) $(PFLAGS) -U$(INCLUDEPATH) -D$(USERDEFINES);$(SYSDEFINES) -O$(INCLUDEPATH) --BCB {$< }

.cpp.obj:
    $(BCB)\BIN\$(BCC32) $(CFLAG1) $(WARNINGS) -I$(INCLUDEPATH) -D$(USERDEFINES);$(SYSDEFINES) -n$(@D) {$< }

.c.obj:
    $(BCB)\BIN\$(BCC32) $(CFLAG1) $(WARNINGS) -I$(INCLUDEPATH) -D$(USERDEFINES);$(SYSDEFINES) -n$(@D) {$< }

.c.i:
    $(BCB)\BIN\$(CPP32) $(CFLAG1) $(WARNINGS) -I$(INCLUDEPATH) -D$(USERDEFINES);$(SYSDEFINES) -n. {$< }

.cpp.i:
    $(BCB)\BIN\$(CPP32) $(CFLAG1) $(WARNINGS) -I$(INCLUDEPATH) -D$(USERDEFINES);$(SYSDEFINES) -n. {$< }

.asm.obj:
    $(BCB)\BIN\$(TASM32) $(AFLAGS) -i$(INCLUDEPATH:;= -i) $(AUSERDEFINES) -d$(SYSDEFINES:;= -d) $<, $@

.rc.res:
    $(BCB)\BIN\$(BRCC32) $(RFLAGS) -I$(INCLUDEPATH) -D$(USERDEFINES);$(SYSDEFINES) -fo$@ $<



# ---------------------------------------------------------------------------




