@echo off
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

