@echo off

if exist asc.bpg goto start
goto error

:start

cd SOURCE\LIBS\BZLIB\WIN
make -fbzlib.mak
cd ..\..\..\..

cd SOURCE\LIBS\JPEG-6B\
make -flibjpeg.mak
cd ..\..\..

cd SOURCE\LIBS\TRIANGUL\WIN32\
make -ftriangulation.mak
cd ..\..\..\..

cd SOURCE\TOOLS\WIN\
make -ftools.mak
make -fviewid.mak
make -fmakegfx.mak
make -fgfx2pcx.mak
make -fdemount.mak
make -fweaponguide.mak
make -fmount.mak
make -fbin2text.mak
cd ..\..\..

cd SOURCE\WIN32\BORLAND\mapeditor\
make -fmapeditor.mak
cd ..\..\..\..

cd SOURCE\AI\
make -fai.mak
cd ..\..

cd SOURCE\WIN32\BORLAND\
make -fasc.mak
cd ..\..\..

md game

if exist game\main.con goto skipmount
cd DATA
..\source\tools\win\mount *.* ..\game\main.con
cd ..
:skipmount

cd game
rem md doc
copy ..\source\win32\borland\*.exe .
copy ..\..\sdl\bin\sdl.dll .
rem xcopy ..\doc\*.* .\doc /e
echo Executables have been copied into subdirectory GAME.
cd ..

goto end

:error
echo Please change to the ASC directory before executing this file !

:end