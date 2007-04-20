; example2.nsi
;
; This script is based on example1.nsi, but it remember the directory, 
; has uninstall support and (optionally) installs start menu shortcuts.
;
; It will install example2.nsi into a directory that the user selects,

;--------------------------------

; The name of the installer
Name "ASC for Project Battle Planets"

; The file to write
OutFile "ASC-1.99.92.77-Install.exe"

; The default installation directory
InstallDir $PROGRAMFILES\ASC

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\Advanced Strategic Command" "Orig_Install_Dir"

;--------------------------------

; Pages

Page license
Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

LicenseData licdata.txt


; The stuff to install
Section "ASC main program (required)"

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Executables
  File "/ASC2.exe"
  File "/mapeditor2.exe"
  
  ; DLLs
  File "/dbghelp.dll"
  File "/jpeg.dll"
  File "/msvcp71.dll"
  File "/msvcr71.dll"
  File "/ogg.dll"
  File "/SDL_mixer.dll"
  File "/smpeg.dll"
  File "/zlib1.dll"
  File "/expat.dll"
  File "/libpng13.dll"
  File "/sdl.dll"
  File "/vorbis.dll"
  File "/in_flac.dll"
  File "/SDL_image.dll"
  File "/sdl_sound.dll"
  File "/vorbisfile.dll"
  
  ; Debug DLLs
  File "/sdl_sound_d.dll"
  File "/msvcr71d.dll"
  File "/msvcp71d.dll"

  ; Data Files
  File "../data/dialog/asc2_dlg.zip"
  File "../data/units/MK1/mk1.con"
  File "../data/units/MK3/units-mk3.con"
  File "../data/main.con"
  File "../data/asc.gfx"
  File "../data/trrobj2/trrobj2.con"
  File "../data/buildings/buildings.con"
  File "../data/trrobj/trrobj.con"

  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\NSIS_Example2 "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Advanced Strategic Command" "DisplayName" "Advanced Strategic Command 2"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Advanced Strategic Command" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Advanced Strategic Command" "URLInfoAbout" '"http://www.asc-hq.org"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Advanced Strategic Command" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Advanced Strategic Command" "NoRepair" 1
  WriteUninstaller "uninstall.exe"

 ; start menu entries


  CreateDirectory "$SMPROGRAMS\Advanced Strategic Command"
  ClearErrors
  CreateShortCut "$SMPROGRAMS\Advanced Strategic Command\Advanced Strategic Command.lnk" "$INSTDIR\asc2.exe" 
  CreateShortCut "$SMPROGRAMS\Advanced Strategic Command\Map Editor.lnk" "$INSTDIR\mapeditor2.exe" 
  WriteINIStr    "$SMPROGRAMS\Advanced Strategic Command\ASC Website.url" "InternetShortcut" "URL" "http://www.asc-hq.org/"


 ; add file association

!define Index "Line${__LINE__}"
  ReadRegStr $1 HKCR ".ascpbm" ""
  StrCmp $1 "" "${Index}-NoBackup"
    StrCmp $1 "AdvancedStrategicCommand.PBEMfile" "${Index}-NoBackup"
    WriteRegStr HKCR ".ascpbm" "backup_val" $1
"${Index}-NoBackup:"
  WriteRegStr HKCR ".ascpbm" "" "AdvancedStrategicCommand.PBEMfile"
  ReadRegStr $0 HKCR "AdvancedStrategicCommand.PBEMfile" ""
  StrCmp $0 "" 0 "${Index}-Skip"
	WriteRegStr HKCR "AdvancedStrategicCommand.PBEMfile" "" "ASC Mail Game"
	WriteRegStr HKCR "AdvancedStrategicCommand.PBEMfile\shell" "" "open"
	WriteRegStr HKCR "AdvancedStrategicCommand.PBEMfile\DefaultIcon" "" "$INSTDIR\asc2.exe,0"
"${Index}-Skip:"
  WriteRegStr HKCR "AdvancedStrategicCommand.PBEMfile\shell\open\command" "" '$INSTDIR\asc2.exe -l "%1"'
!undef Index
  
SectionEnd

Section "Tools"
   SetOutPath "$INSTDIR\tools"
   File "data/building-template.png"
   File "data/hexfield.png"
   File "/weaponguide.exe"
   File "/demount.exe"
   File "/mount.exe"
 ;  File "/makegfx.exe"
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Example2"
  DeleteRegKey HKLM SOFTWARE\NSIS_Example2

  ; Remove files and uninstaller
  Delete $INSTDIR\*.exe
  Delete $INSTDIR\*.dll
  Delete $INSTDIR\*.con
  Delete $INSTDIR\*.cache
  Delete $INSTDIR\*.gfx

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Advanced Strategic Command\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\Advanced Strategic Command"
  ; RMDir "$INSTDIR"


!define Index "Line${__LINE__}"
  ReadRegStr $1 HKCR ".ascpbm" ""
  StrCmp $1 "AdvancedStrategicCommand.PBEMfile" 0 "${Index}-NoOwn" ; only do this if we own it
    ReadRegStr $1 HKCR ".ascpbm" "backup_val"
    StrCmp $1 "" 0 "${Index}-Restore" ; if backup="" then delete the whole key
      DeleteRegKey HKCR ".ascpbm"
    Goto "${Index}-NoOwn"
"${Index}-Restore:"
      WriteRegStr HKCR ".ascpbm" "" $1
      DeleteRegValue HKCR ".ascpbm" "backup_val"
   
    DeleteRegKey HKCR "AdvancedStrategicCommand.PBEMfile" ;Delete key with association settings
 
"${Index}-NoOwn:"
!undef Index

SectionEnd
