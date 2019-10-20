# Compiling ASC for Windows


## Introduction

This document was written some time ago... Somewhere around 2008. That's the time when I setup my Windows Development Environment.
Since then, things have improved for Windows Developer with technologies like NuGet. But my ancient build environment still works, and this is the description how I set it up.
So far, nobody other than me has ever managed to compile ASC on windows. 

## Original Introduction

To summarize this whole tutorial: software development for Windows simply sucks. It's a pain in the ass. If you want to enjoy developing software, go to Linux.

But unfortunately, there are still lots of users out there who want to player ASC - and we can't deprive them of ASC, can we? So I put together this document in the hope that maybe someone apart from me will manage to compile ASC...

If you want to give it a try, please make sure you have
* plenty of time
* plenty of free hard disk capacity (5 GB should be sufficient)
* plenty of bandwidth
* plenty of patience 

### General principals

#### Library location publishing

ASC uses two different mechanisms to locate libraries it needs. Libraries need two kinds of directoriese to be made known to the application that uses the library:

* the INCLUDE directory with the header files
* the LIB directory with the compiled libraries against which the application is linked. 

In some cases the ASC project references an environment variable that has to be set to the library directory. For other libs the installation directory must be registered in the global VisualStudio directories, so that VisualStudio automatically searches that directory for files. This is done in Tools -> Options ->Projects & Solutions / VC++ Directories / Include Files or Library Files, respectively.

The environment variable system is usually used when

* different projects may need different versions of a library
* debug and release library names have the same name, but are located in different directories
* the had been the need to debug the libraries during ASC development 

#### Directory names

It usually does not matter where you install your libraries. In this tutorial, I'm using fixed directories so that I can easily reference them.

In this article I assume that we are installing all stuff into a folder ascdev. It may be c:\program files\ascdev , it may be somewhere else. It doesn't matter. Whenever the directory ascdev is mentioned, please use the fully qualified path, like c:\program files\ascdev

Several directory names contain version numbers. If you are downloading newer versions of the libraries, you may of course adjust the version numbers accordingly.

### Setting up the IDE

1. Get and Install Microsoft Visual C++ Express
1. Download the DirectX SDK from Microsoft
1. Install it into ascdev\directX-sdk 

### Setting up the libraries

Setup DLL target dir

1. Create a directory ascdev\dlls where all created dlls will be placed
1. Add this directory to Windows' path environment variable 

#### Boost

1. Download Boost installer from http://www.boostpro.com/products/free (this includes the precompiled libs for MSVC, which the official distribution from http://www.boost.org lacks)
1. Install everything (you may skip the single-threaded libs) into ascdev\boost_1_35_0
1. In Visual Studio, set `ascdev\boost_1_35_0` as global include directory ...
1. ... and `ascdev\boost_1_35_0\lib` as global library dirctory. 

#### SDL

1. Download SDL 1.2 from [www.libsdl.org]
1. Unzip the archive into ascdev (which will result in a directory ascdev\SDL-1.2.13)
1. Unzip ascdev\SDL-1.2.13\VisualC.zip into the ascdev\SDL-1.2.13 directory (which result in a directory ascdev\SDL-1.2.13\VisualC)
1. Open ascdev\SDL-1.2.13\VisualC\SDL.sln in Visual Studio
1. Build everything ( Build -> Batch Build -> Select All )
1. Go to Tools -> Options ->Projects & Solutions / VC++ Directories / Include Files and add the ascdev\SDL-1.2.13\include directory there.
1. In the same dialog, add ascdev\SDL-1.2.13\VisualC\SDL\Release and ascdev\SDL-1.2.13\VisualC\SDLmain\Release_NOSTDIO to the library directories
1. Copy ascdev\SDL-1.2.13\VisualC\SDL\Release\SDL.dll into ascdev\dlls
1. In Windows, go to Control Panel -> System -> Advanced -> Environment Variables and add a variable SDL with the value ascdev\SDL-1.2.13\VisualC\SDL (this is to provide the paths to the libraries. We are not doig this through a setting in Visual Studio because we want to access the release build and debug builds individually, which both have the same name) 

#### SDL-mixer

1. Download SDL-Mixer from http://www.libsdl.org/projects/SDL_mixer/
1. Unzip it into the ascdev Folder (which will result in a directory ascdev\SDL_mixer-1.2.8)
1. Unzip ascdev\SDL_mixer-1.2.8\VisualC.zip into the ascdev\SDL_mixer-1.2.8 directory (which will result in a directory ascdev\SDL_mixer-1.2.8\VisualC)
1. Open ascdev\SDL_mixer-1.2.8\VisualC\SDL_mixer.sln in Visual Studio
1. Build everything ( Build -> Batch Build -> Select All )
1. There will be a few errors because the file afxres.h is not found. Replace afxres.h by winresrc.h in the source file
1. Build again everything
1. copy ascdev\SDL_mixer-1.2.8\VisualC\Release\SDL_mixer.dll into ascdev\dlls
1. add ascdev\SDL_mixer-1.2.8 to Visual C++ include dir
1. add ascdev\SDL_mixer-1.2.8\VisualC\Release to Visual C++ library dir 

#### SDL-Image

1. Download SDL-Image from http://www.libsdl.org/projects/SDL_image/
1. Unzip it into the ascdev Folder (which will result in a directory ascdev\SDL_image-1.2.6)
1. Unzip ascdev\SDL_image-1.2.6\VisualC.zip into the ascdev\SDL_image-1.2.6 directory (which will result in a directory ascdev\SDL_image-1.2.6\VisualC)
1. Open ascdev\SDL_image-1.2.6\VisualC\SDL_image.sln in Visual Studio
1. Build everything ( Build -> Batch Build -> Select All )
1. There will be a few errors because the file afxres.h is not found. Replace afxres.h by winresrc.h in the source file
1. Build again everything
1. Add ascdev\SDL_image-1.2.6 to Visual Studio include dir
1. Add ascdev\SDL_image-1.2.6\VisualC\Release to Visual Studio library dir
1. copy ascdev\SDL_image-1.2.6\VisualC\Release\SDL_image.dll into ascdev\dlls\
1. copy the following files from ascdev\SDL_image-1.2.6\VisualC\graphics\lib to ascdev\dlls :
   * jpeg.dll
   * libpng12-0.dll
   * libtiff-3.dll 

#### SMPEG

1. To get the SMPEG sources, you need the Subversion client. If you don't have it, download it from http://subversion.tigris.org
1. Go to the ascdev directory and run svn co svn://svn.icculus.org/smpeg/trunk smpeg (which will create a directory ascdev\smpeg)
1. In VisualStudio, add ascdev\smpeg to the global include dirs (see SDL section)
1. Unzip the VisualC.zip
1. Open ascdev\smpeg\VisualC\smpeg.dsw
1. Build the smpeg library for both debug and release
1. Add ascdev\smpeg\VisualC\Release to Visual Studio library dir
1. copy ascdev\smpeg\VisualC\Release\smpeg.dll into ascdev\dlls\ 

#### LibOgg, LibVorbis

1. Download libogg, libFlac and libvorbis from http://xiph.org/ and extract them
1. LibOgg and LibVorbis have each a win32 subdirectory with a .dsw file in. Open it in Visual Studio and build all targets.
1. For Libflac it's in the main directory. Build it too.
1. Add ascdev\libvorbis-1.2.0\include and ascdev\libogg-1.1.3\include to Visual Studio include dir.
1. Add ascdev\libvorbis-1.2.0\win32\Vorbis_Dynamic_Release, ascdev\libvorbis-1.2.0\win32\VorbisFile_Dynamic_Release and ascdev\libogg-1.1.3\win32\Dynamic_Release to Visual Studio lib dir
1. copy these files into ascdev\dlls\
   * ascdev\libvorbis-1.2.0\win32\VorbisFile_Dynamic_Release\vorbisfile.dll
   * ascdev\libvorbis-1.2.0\win32\Vorbis_Dynamic_Release\vorbis.dll
   * ascdev\libogg-1.1.3\win32\Dynamic_Release\ogg.dll 

#### LibFlac

1. Download libFlac from http://xiph.org/
1. Libflac requires nasmw.exe to be in your path. Download the latest NASM release from http://nasm.sourceforge.net/
   1. If it contains only a nasm.exe, rename it to nasmw.exe
   1. Put it into a directory that is in your windows path (or alternatively extend your path to the nasm directory) 
1. Build the libFLAC_dynamic and libFLAC_static projects for both release and debug
1. Add ascdev\flac-1.1.2\include to Visual Studio include dir.
1. Add ascdev\flac-1.1.2\obj\release\lib to library dir
1. copy ascdev\flac-1.1.2\obj\release\bin\libFLAC.dll into ascdev\dlls\ 

#### SDL-Sound

1. Download SDL-Sound from http://icculus.org/SDL_sound/
1. Unzip it into the ascdev Folder (which will result in a directory ascdev\SDL_sound-1.0.3)
1. Open ascdev\SDL_sound-1.0.3\VisualC\sdl_sound.dsw in Visual Studio
1. We are now building SDL_sound without Mikmod and Modplug to make life a bit easer.
   1. Right-click on sdl_sound_dll, select properties and go to configurion Properties -> C/C++ -> Preprocessor
   1. For both Release and Debug remove the entries SOUND_SUPPORTS_MODPLUG and SOUND_SUPPORTS_MIKMOD from preprocessor definitions
   1. Do the same for sdl_sound_static
   1. Remove the mikmod.lib and modplug.lib from the referenced libraries 
1. Build the sdl_sound_* projects for debug and release
   1. If it fails to compile, you may need to add the file decoders\timidity\instrum_dls.c to the project 
1. Add ascdev\SDL_sound-1.0.3 to Visual Studio include directory
1. Add ascdev\SDL_sound-1.0.3\VisualC\win32lib to library directory
1. copy ascdev\SDL_sound-1.0.3\VisualC\win32lib\sdl_sound*.dll into ascdev\dlls\ 

#### LibSigC++

1. Install LibSigC++ 2.0 
1. Extract it to a directory ascdev\libsigc++
1. compile and build it
1. Set an environment variable libsigcpp=ascdev\libsigc++ 

#### Freetype2

1. Get it from http://www.freetype.org and install it in ascdev\freetype2
1. Open the solution [ascdev\freetype-2.3.6\builds\win32\visualc\freetype.sln] and build all targets
1. Go to ascdev\freetype-2.3.6\objs and copy freetype236.lib to freetype.lib and freetype236_D.lib to freetype_D.lib. This is to avoid having ASC reference a specific version of Freetype.
1. Add ascdev\freetype-2.3.6\include as global include dir in VicualC++
1. Add ascdev\freetype-2.3.6\objs as global library dir in VisualC++ 

#### Expat

1. Get it from http://expat.sourceforge.net/
1. Install it into ascdev\Expat 2.0.1
1. Open ascdev\Expat 2.0.1\Source\expat.dsw in Visual Studio and build all targets
1. Add ascdev\Expat 2.0.1\Source\lib\ to Visual Studio include dir
1. Add ascdev\Expat 2.0.1\Source\win32\bin to Visual Studio lib dir
1. copy ascdev\Expat 2.0.1\Source\win32\bin\Release\libexpat.dll to ascdev\dlls\ 

#### Physfs

1. Install and build Physfs in ascdev\physfs-1.0.0
1. Set a globabl environment variable `physfs=ascdev\physfs-1.0.0 

#### LibPNG

1. Get it from http://www.libpng.org/pub/png/libpng.html and install it
1. Open the solution in (C:\Programme\ascdev\libpng-1.2.29\projects\visualc71\libpng.sln) and build all targets without ASM in their name. You can of course build all targets too, but the ASM targets will fail and we don't need them anyway.
1. Set an environment variable libpng=ascdev\libpng-1.2.29
1. Set a global include dir in Visual Studio to $(libpng)
1. Copy the following files from ascdev\libpng-1.2.29\projects\visualc71\Win32_DLL_Release\ into ascdev\dlls\
   * libpng13.dll
   * libpng13d.dll
   * ZLib\zlib1.dll
   * ZLib\zlib1d.dll 

#### ZLib

1. Get it from http://www.zlib.net/ and install it (ascdev\zlib-1.2.3)
1. Open the project ( ascdev\zlib-1.2.3\projects\visualc6\zlib.dsw) in Visual Studio and compile all targets without ASM in their name (same as LibPNG)
1. Add ascdev\zlib-1.2.3 to Visual Studio's global include directory
1. Add ascdev\zlib-1.2.3\projects\visualc6\Win32_DLL_Releaseto Visual Sutdio's global library directory 

#### Curl

1. Curl doesn't provide usable build files for Visual Studio, so I recommend downloading a prebuild Curl package: [http://my.guardpuppy.com/libcurl-7.15.1-msvc-win32-ssl-0.9.8a-zlib-1.2.3.zip]
1. Extract the package into ascdev\curl
1. Add ascdev\curl\include to the Visual Studio include dir
1. Add ascdev\curl\ to library dir
1. copy the file libcurl.dll libeay32.dll ssleay32.dll from ascdev\curl\ to ascdev\dlls\ 

#### Revel

1. Get it from http://revel.sourceforge.net/ (type: precompiled binaries) and install it to ascdev\revel-1.1.0-win32
1. Add ascdev\revel-1.1.0-win32\lib to VS library dir
1. Add revel-1.1.0-win32\include to VS include dir 

### Compiling ASC

Finally, everything should be ready to build ASC itself!

1. Get the latest [Source Code], either packaged or through CVS. If you have got this far, I recommend using CVS because it's easier to incorporate update and to make diffs in case you make changes and want to have them incorporated.
1. If you get the source by CVS, you will end up with ascdev\games\asc. You can move the asc directory to ascdev\asc and then delete games
1. Open the project file ascdev\asc\source\win32\msvc\ASC.sln in Visual Studio
1. Build all targets, which may take some time...
1. If everything went smooth, you will find the executables in ascdev\asc\source\win32\msvc\release\bin and ascdev\asc\source\win32\msvc\debug\bin respectively 

Congratulations! You now have mastered the ASC build process on Windows! I would appreciate if you give some feedback in the Forum, so that we know somebody really managed it 
