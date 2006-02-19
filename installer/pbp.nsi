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
OutFile "PBP-Install.exe"

; The default installation directory
InstallDir $PROGRAMFILES\ASC

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\Advanced Strategic Command" "Orig_Install_Dir"

;--------------------------------

; Pages

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "ASC main program (required)"

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File "win32/asc.exe"
  File "win32/mapeditor.exe"
  File "win32/dlls/*.dll"
  File "../data/dialog/asc_dlg.zip"
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
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Advanced Strategic Command" "DisplayName" "Advanced Strategic Command"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Advanced Strategic Command" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Advanced Strategic Command" "URLInfoAbout" '"http://www.asc-hq.org"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Advanced Strategic Command" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Advanced Strategic Command" "NoRepair" 1
  WriteUninstaller "uninstall.exe"

 ; start menu entries


CreateDirectory "$SMPROGRAMS\Advanced Strategic Command"
ClearErrors
CreateShortCut "$SMPROGRAMS\Advanced Strategic Command\Advanced Strategic Command.lnk" "$INSTDIR\asc.exe" 
CreateShortCut "$SMPROGRAMS\Advanced Strategic Command\Map Editor.lnk" "$INSTDIR\mapeditor.exe" 
WriteINIStr    "$SMPROGRAMS\Advanced Strategic Command\ASC Website.url" "InternetShortcut" "URL" "http://www.asc-hq.org/"


 ; add file association

!define Index "Line${__LINE__}"
  ReadRegStr $1 HKCR ".asc" ""
  StrCmp $1 "" "${Index}-NoBackup"
    StrCmp $1 "AdvancedStrategicCommand.PBEMfile" "${Index}-NoBackup"
    WriteRegStr HKCR ".asc" "backup_val" $1
"${Index}-NoBackup:"
  WriteRegStr HKCR ".asc" "" "AdvancedStrategicCommand.PBEMfile"
  ReadRegStr $0 HKCR "AdvancedStrategicCommand.PBEMfile" ""
  StrCmp $0 "" 0 "${Index}-Skip"
	WriteRegStr HKCR "AdvancedStrategicCommand.PBEMfile" "" "ASC Mail Game"
	WriteRegStr HKCR "AdvancedStrategicCommand.PBEMfile\shell" "" "open"
	WriteRegStr HKCR "AdvancedStrategicCommand.PBEMfile\DefaultIcon" "" "$INSTDIR\asc.exe,0"
"${Index}-Skip:"
  WriteRegStr HKCR "AdvancedStrategicCommand.PBEMfile\shell\open\command" "" '$INSTDIR\asc.exe -l "%1"'
!undef Index
  
SectionEnd

Section "PBP components"

  File "win32/mount.exe"
  File "pbp/*"

WriteINIStr    "$SMPROGRAMS\Advanced Strategic Command\PBP Website.url" "InternetShortcut" "URL" "http://www.battle-planet.de/"
  
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
  ReadRegStr $1 HKCR ".asc" ""
  StrCmp $1 "AdvancedStrategicCommand.PBEMfile" 0 "${Index}-NoOwn" ; only do this if we own it
    ReadRegStr $1 HKCR ".asc" "backup_val"
    StrCmp $1 "" 0 "${Index}-Restore" ; if backup="" then delete the whole key
      DeleteRegKey HKCR ".asc"
    Goto "${Index}-NoOwn"
"${Index}-Restore:"
      WriteRegStr HKCR ".asc" "" $1
      DeleteRegValue HKCR ".asc" "backup_val"
   
    DeleteRegKey HKCR "AdvancedStrategicCommand.PBEMfile" ;Delete key with association settings
 
"${Index}-NoOwn:"
!undef Index

SectionEnd
