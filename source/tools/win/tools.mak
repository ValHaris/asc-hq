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

VERSION = BCB.05.03
# ---------------------------------------------------------------------------
PROJECT = tools.lib
OBJFILES = obj\basegfx.obj obj\basestrm.obj obj\buildingtype.obj obj\gameoptions.obj \
    obj\misc.obj obj\Named.obj obj\palette.obj obj\sgstream.obj obj\typen.obj \
    obj\vehicletype.obj obj\graphicset.obj obj\ascstring.obj \
    obj\graphicselector.obj obj\loadpcxc.obj obj\newfont.obj obj\CLoadable.obj \
    obj\Property.obj obj\PropertyGroup.obj obj\graphics.obj obj\getopt1.obj \
    obj\getopt.obj obj\strtmesg.obj
RESFILES = 
MAINSOURCE = tools.bpf
RESDEPEN = $(RESFILES)
LIBFILES = ..\..\libs\bzlib\win\bzlib.lib ..\..\..\..\sdl\bin\sdl.lib
IDLFILES = 
IDLGENFILES = 
LIBRARIES = 
PACKAGES = 
SPARELIBS = 
DEFFILE = 
# ---------------------------------------------------------------------------
PATHCPP = .;..\..\..\source;..\..\sdl;..\..\LIBS\getopt
PATHASM = .;
PATHPAS = .;
PATHRC = .;
LINKER = TLib
DEBUGLIBPATH = 
RELEASELIBPATH = 
USERDEFINES = _DEBUG;HEXAGON;_WIN32_;WIN32;converter;_NOASM_;_SDL_
SYSDEFINES = _RTLDLL;NO_STRICT
INCLUDEPATH = ..\..\LIBS\getopt;..\..\sdl;..\..;..\..\..\source;$(BCB)\include;$(BCB)\include\vcl;..\..\win32;..\..\..\..\SDL\include
LIBPATH = ..\..\LIBS\getopt;..\..\sdl;..\..;..\..\..\source;$(BCB)\lib\obj;$(BCB)\lib
WARNINGS = -w-par -w-csu
LISTFILE = 
# ---------------------------------------------------------------------------
CFLAG1 = -Od -Q -Vx -Ve -X- -a1 -b -k -y -v -vi- -c -tW -tWM -K
IDLCFLAGS = 
PFLAGS = -N2obj -N0obj -$YD -$W -$O- -v -JPHNE -M
RFLAGS = 
AFLAGS = /mx /w2 /zd
LFLAGS = /P64 /0 /E
# ---------------------------------------------------------------------------
ALLOBJ = $(OBJFILES)
ALLRES = 
ALLLIB = 
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
LINKER = TLib
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
# ---------------------------------------------------------------------------
!if "$(LISTFILE)" ==  ""
COMMA =
!else
COMMA = ,
!endif

$(PROJECT): $(IDLGENFILES) $(OBJFILES) $(RESDEPEN) $(DEFFILE)
    $(BCB)\BIN\$(LINKER) /u $@ @&&!
    $(LFLAGS) $? $(COMMA) $(LISTFILE)

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




