; example2.nsi
;
; This script is based on example1.nsi, but it remember the directory, 
; has uninstall support and (optionally) installs start menu shortcuts.
;
; It will install example2.nsi into a directory that the user selects,

;--------------------------------

; The name of the installer
Name "Advanced Strategic Command 2.4.100.103"

; The file to write
OutFile "ASC-2.4.100.103-Install.exe"

; The default installation directory
InstallDir $PROGRAMFILES\ASC

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\Advanced Strategic Command" "Orig_Install_Dir"

!include "LogicLib.nsh"
!include "Sections.nsh"

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

Icon "../source/win32/msvc/ASC/icon2.ico"
UninstallIcon "../source/win32/msvc/ASC/icon2.ico"

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
  File "/msvcp90.dll"
  File "/msvcr90.dll"
  File "/ogg.dll"
  File "/SDL_mixer.dll"
  File "/smpeg.dll"
  File "/zlib1.dll"
  File "/libexpat.dll"
  File "/libpng13.dll"
  File "/libpng12-0.dll"
  File "/libtiff-3.dll"
  File "/sdl.dll"
  File "/README-SDL.txt"
  File "/vorbis.dll"
  File "/libFLAC.dll"
  File "/SDL_image.dll"
  File "/sdl_sound.dll"
  File "/vorbisfile.dll"
  File "/libcurl.dll"
  File "/libeay32.dll" 
  File "/ssleay32.dll"
  File "/xvidcore.dll"
  File "/Microsoft.VC90.CRT.manifest"


  ; Debug DLLs
  ; File "/sdl_sound_d.dll"
  ; File "/msvcr71d.dll"
  ; File "/msvcp71d.dll"

  ; Data Files
  File "../data/dialog/asc2_dlg.zip"
  File "../data/main.ascdat"
  File "../COPYING"

  call registerASC
  
SectionEnd

Section /o "USB Stick Mode" USB
  SetOutPath $INSTDIR
  File "data/asc2.ini"
SectionEnd


Section "Music Download"
   SetOutPath "$INSTDIR\music"
   IfFileExists "$INSTDIR\music\frontiers.mp3" +6 0
   IfFileExists "$INSTDIR\music\frontiers.ogg" +5 0
   NSISdl::download http://www.asc-hq.org/music/frontiers.ogg frontiers.ogg
   Pop $R0 ;Get the return value
       StrCmp $R0 "success" +2
          MessageBox MB_OK "Download failed: $R0"

   IfFileExists "$INSTDIR\music\machine_wars.mp3" +6 0
   IfFileExists "$INSTDIR\music\machine_wars.ogg" +5 0
   NSISdl::download http://www.asc-hq.org/music/machine_wars.ogg machine_wars.ogg
   Pop $R0 ;Get the return value
       StrCmp $R0 "success" +2
          MessageBox MB_OK "Download failed: $R0"

   IfFileExists "$INSTDIR\music\time_to_strike.mp3" +6 0
   IfFileExists "$INSTDIR\music\time_to_strike.ogg" +5 0
   NSISdl::download http://www.asc-hq.org/music/time_to_strike.ogg time_to_strike.ogg
   Pop $R0 ;Get the return value
       StrCmp $R0 "success" +2
          MessageBox MB_OK "Download failed: $R0"
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
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Advanced Strategic Command"

  ; Remove files and uninstaller
  Delete $INSTDIR\*.exe
  Delete $INSTDIR\*.dll
  Delete $INSTDIR\*.ascdat
  Delete $INSTDIR\*.cache
  Delete $INSTDIR\*.gfx
  Delete $INSTDIR\tools\*.exe
  Delete $INSTDIR\tools\building-template.png 
  Delete $INSTDIR\tools\hexfield.png 
  Delete $INSTDIR\music\*.ogg
  Delete $INSTDIR\asc2_dlg.zip
  Delete $INSTDIR\README-SDL
  Delete $INSTDIR\Microsoft.VC90.CRT.manifest
  Delete $INSTDIR\README-SDL.txt"
  Delete $INSTDIR\COPYING"

  Delete $DOCUMENTS\ASC\ASC2.cache
  Delete $DOCUMENTS\ASC\progress.dat


  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Advanced Strategic Command\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\Advanced Strategic Command"
  RMDir "$INSTDIR\music"
  RMDir "$INSTDIR\tools"
  RMDir "$INSTDIR"


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

Function registerASC
  SectionGetFlags ${USB} $0
  IntOp $0 $0 & 1
  ${If} $0 == ${SF_SELECTED}
      MessageBox MB_OK 'USB Mode selected, ASC will not be registered in Windows'
  ${Else}

      ; Write the installation path into the registry - not used any more, since we store in AppData
      ; WriteRegStr HKLM "SOFTWARE\Advanced Strategic Command" "InstallDir2" "$INSTDIR"
      
      ; Write the uninstall keys for Windows
      WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Advanced Strategic Command" "DisplayName" "Advanced Strategic Command 2.4.100.103"
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
      WriteINIStr    "$SMPROGRAMS\Advanced Strategic Command\ASC Forum.url" "InternetShortcut" "URL" "http://battle-planet.de/pbp/board/index.php"
      WriteINIStr    "$SMPROGRAMS\Advanced Strategic Command\More Maps.url" "InternetShortcut" "URL" "http://battle-planet.de/pbp/board/viewforum.php?f=23"
      
      
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
  ${EndIf}  




FunctionEnd
