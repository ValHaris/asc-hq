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
PROJECT = libjpeg.lib
OBJFILES = jutils.obj jcapistd.obj jccoefct.obj jccolor.obj jcdctmgr.obj jchuff.obj \
    jcinit.obj jcmainct.obj jcmarker.obj jcmaster.obj jcomapi.obj jcparam.obj \
    jcphuff.obj jcprepct.obj jcsample.obj jctrans.obj jdapimin.obj \
    jdapistd.obj jdatadst.obj jdatasrc.obj jdcoefct.obj jdcolor.obj \
    jddctmgr.obj jdhuff.obj jdinput.obj jdmainct.obj jdmarker.obj jdmaster.obj \
    jdmerge.obj jdphuff.obj jdpostct.obj jdsample.obj jdtrans.obj jerror.obj \
    jfdctflt.obj jfdctfst.obj jfdctint.obj jidctflt.obj jidctfst.obj \
    jidctint.obj jidctred.obj jmemmgr.obj jmemnobs.obj jquant1.obj jquant2.obj \
    jcapimin.obj
RESFILES = 
MAINSOURCE = libjpeg.bpf
RESDEPEN = $(RESFILES)
LIBFILES = 
IDLFILES = 
IDLGENFILES = 
LIBRARIES = 
PACKAGES = 
SPARELIBS = 
DEFFILE = 
# ---------------------------------------------------------------------------
PATHCPP = .;
PATHASM = .;
PATHPAS = .;
PATHRC = .;
LINKER = TLib
DEBUGLIBPATH = 
RELEASELIBPATH = 
USERDEFINES = _DEBUG
SYSDEFINES = _RTLDLL;NO_STRICT
INCLUDEPATH = ..\..\sdl;$(BCB)\include;$(BCB)\include\vcl
LIBPATH = ..\..\sdl;$(BCB)\lib\obj;$(BCB)\lib
WARNINGS = -w-par
LISTFILE = 
# ---------------------------------------------------------------------------
CFLAG1 = -Od -Q -Vx -Ve -X- -r- -a1 -b -k -y -v -vi- -c -tW -tWM
IDLCFLAGS = 
PFLAGS = -$YD -$W -$O- -v -JPHNE -M
RFLAGS = 
AFLAGS = /mx /w2 /zd
LFLAGS = 
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




