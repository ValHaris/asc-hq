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
OutFile "ASC-1.99.93.93-Install.exe"

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
  File "/mnt/win2000/Programme/ascdev/asc-newgraph//ASC2.exe"
  File "/mnt/win2000/Programme/ascdev/asc-newgraph//mapeditor2.exe"
  
  ; DLLs
  File "/mnt/win2000/Programme/libs//dbghelp.dll"
  File "/mnt/win2000/Programme/libs//jpeg.dll"
  File "/mnt/win2000/Programme/libs//msvcp71.dll"
  File "/mnt/win2000/Programme/libs//msvcr71.dll"
  File "/mnt/win2000/Programme/libs//ogg.dll"
  File "/mnt/win2000/Programme/libs//SDL_mixer.dll"
  File "/mnt/win2000/Programme/libs//smpeg.dll"
  File "/mnt/win2000/Programme/libs//zlib1.dll"
  File "/mnt/win2000/Programme/libs//expat.dll"
  File "/mnt/win2000/Programme/libs//libpng13.dll"
  File "/mnt/win2000/Programme/libs//sdl.dll"
  File "/mnt/win2000/Programme/libs//vorbis.dll"
  File "/mnt/win2000/Programme/libs//in_flac.dll"
  File "/mnt/win2000/Programme/libs//SDL_image.dll"
  File "/mnt/win2000/Programme/libs//sdl_sound.dll"
  File "/mnt/win2000/Programme/libs//vorbisfile.dll"
  
  ; Debug DLLs
  File "/mnt/win2000/Programme/libs//sdl_sound_d.dll"
  File "/mnt/win2000/Programme/libs//msvcr71d.dll"
  File "/mnt/win2000/Programme/libs//msvcp71d.dll"

  ; Data Files
  File "../data/dialog/asc2_dlg.zip"
  File "../data/main.ascdat"

  ; Write the installation path into the registry
  WriteRegStr HKLM "SOFTWARE\Advanced Strategic Command" "InstallDir2" "$INSTDIR"
  
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
   File "/mnt/win2000/Programme/ascdev/asc-newgraph//weaponguide.exe"
   File "/mnt/win2000/Programme/ascdev/asc-newgraph//demount.exe"
   File "/mnt/win2000/Programme/ascdev/asc-newgraph//mount.exe"
 ;  File "/mnt/win2000/Programme/ascdev/asc-newgraph//makegfx.exe"
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
