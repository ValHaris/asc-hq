#
# spec file for package asc (Version 2.1.0.0)
#
# Copyright (c) 2008 SUSE LINUX Products GmbH, Nuernberg, Germany.
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#

# norootforbuild


Name:           asc
BuildRequires:  SDL-devel SDL_image-devel SDL_mixer-devel SDL_sound-devel SDLmm-devel gcc-c++ libsigc++12-devel physfs-devel
BuildRequires:  boost-devel freetype2-devel libexpat-devel libjpeg-devel pkg-config update-desktop-files zip lua-devel wxGTK-devel 
Summary:        Advanced Strategic Command--Turn-Based Strategy Game
Version:        2.4.102.0
Release:        1.1
License:        GPL v2 or later
Group:          Amusements/Games/Strategy/Turn Based
Source:         %{name}-%{version}.tar.bz2
Source1:        %{name}.desktop
Source2:        %{name}-music.tar.gz
Url:            http://www.asc-hq.org/
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description
ASC is a free, turn-based strategy game in the tradition of Battle
Isle. It features both single player game play against an AI as well as
extensive play-by-e-mail multiplayer functionality.



Authors:
--------
    Martin Bickel <bickel@asc-hq.org>

%package music
License:        GPL v2 or later
Summary:        Music for Advanced Strategic Command--Turn-Based Strategy Game
Group:          Amusements/Games/Strategy/Turn Based
Requires:       %{name}

%description music
ASC is a free, turn-based strategy game in the tradition of Battle
Isle. It features both single player game play against an AI as well as
extensive play-by-e-mail multiplayer functionality.



Authors:
--------
    Martin Bickel <bickel@asc-hq.org>

%prep
%setup -q
chmod ugo-x doc/manpages/* doc/unitguide/output/*.gif doc/unitguide/output/*.html doc/unitguide/output/*.txt
tar -C data/music -xzf %{S:2}

%build
export PATH=$PATH:.
# _ISOC99_SOURCE is defined for log2. It compiles without it, but does not work
export CFLAGS="$RPM_OPT_FLAGS -DUSE_HOME_DIRECTORY=1 -D_ISOC99_SOURCE -D_GNU_SOURCE"
export CXXFLAGS="$CFLAGS"
%configure \
    --disable-sdltest \
    --disable-vorbistest
make %{?jobs:-j%jobs}

%install
make DESTDIR=$RPM_BUILD_ROOT install
%suse_update_desktop_file -i %{name} Game ArcadeGame

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc AUTHORS COPYING ChangeLog README TODO
%doc %{_mandir}/man6/*
%{_prefix}/bin/*
%dir %{_datadir}/games/asc
%{_datadir}/games/asc/asc2_dlg.zip
%{_datadir}/games/asc/main.ascdat
%{_datadir}/applications/*.desktop

%files music
%defattr(-,root,root)
%dir %{_datadir}/games/asc/music
%{_datadir}/games/asc/music/*

%changelog
* Mon Nov 10 2008 prusnak@suse.cz
- fix wrong delete usage (delete.patch) [bnc#442935]
* Thu Apr 24 2008 prusnak@suse.cz
- updated to 2.1.0.0
  o Major new features:
  * new campaign: Kamor
  * replays can be saved as MPEG4 movies
  * building that harvest objects (e.g. forest, crystals)
  * units may now internally produce other units
  * some units are equipped with a self-destruct device
  o Changes to game mechanics:
  * attack bonus of terrain now also used for ranged attack
  o Updated data:
  * few snow graphics overhauled
  * replaced weather graphics with overlays
  * unified properties of terrain
  * TOW, Wiesel Bugfix category
  * LR5, Service now also on ground level and deep submerged;
    has external fuel transfer
  * Adjusted unit pictures for consistent contrast and color
  * Bradley techbugfix
  * Skjold, Abilities Bugfix
  * Elch and Ant can't build strong_pipeline any longer
  * Forrester can harvest oil plants
  * MBV can now build farm, lumbercamp, crystal processing and spaceport
  * wheeled vehicles have increased movement
  * default radar vehicle 66 movement/tracked - * 80 movement/wheeled
  * oracle is now a tracked vehicle
  * Judy construction bugfix, can now harvest minerals
  * Shuttle weight is now 310
  * Spacestation has a reduced weight of 400
  * Booster can transport Spacelab into orbit
  * Fix has no internal fueltransfer any more
  * changed punch for planes when on ground level
  * changed properties of cruise missile weapons
  * standardized ship weapons
  * new unit 'Panzerzug' for MK3 set
  * Elite Pioneers can now build all kinds of foundations
  * Wachturm weight is 6000
  * Dora/K5 require Heavy Gun technology
  * new Cliff objects
  * new linkable mountains object
  o Bugfixes:
  * crash when moving units into screen
  * units with truecolor images are now always displayed in correct color
  * Mining Station analysis shows current production in absolute numbers.
  * Messages can now be saved with ctrl-s
  * Fixed: dump vehicle now gives correct output with features/abilities
  * Fixed: weather images for buildings not correctly saved to cache
  * automatic resetting of player data when player is terminated
  * Added email editing in admin-game dialog
  * Fixed replay error: remove production lines
  * Now also viewing cargo indicator for other players' units if they
    contain own cargo
  * Applied patch to copyArea function in mapeditor
  * Mouse zoom now centers on mouse cursor
  * Level of height display in dashboard shows color of unit
  * Fixed hotkeys in mapeditor (cargo editor)
  * converted last maps to new file format, so they can be loaded on any
    platform
  * Fixed: production unit didn't show units that couldn't be produced because
    o of lacking resources if "only produce researched stuff" was enabled
  * Fixed replay bug when constructing turrets
  * Fixed endless loop when searching for map by password
  * Unit summary should now also count units in allied transports
  * The gui buttons for mine placement are no longer visible when a mine has
    been layed.
  * fixed various problems on campaign maps, including translation
  * Fixed crashes on startup when using certain versions of PhysFS
  * ASC now compiles with gcc 4.3 without errors or warnings
- dropped obsolete patches:
  * includes.patch (included in update)
* Tue Nov 20 2007 prusnak@suse.cz
- added missing includes (includes.patch)
* Tue Nov  6 2007 prusnak@suse.cz
- update to 2.0.1.0
  * completely rewritten user interface and new graphics engine
  * added lots of cool new features, too numerous to list them here
* Wed May 23 2007 prusnak@suse.cz
- removed executable attribute from GIF files and manpages
- removed redundant prefix line in spec file
- removed Requires
* Fri Apr 13 2007 ro@suse.de
- added libmikmod-devel to buildreq
* Tue Feb 27 2007 prusnak@suse.cz
- added physfs to Requires and physfs-devel to BuildRequires
* Thu Feb 22 2007 prusnak@suse.cz
- updated to 1.16.4.0
  * reactionfire is now correctly recorded in Replay
  * fixed a replay crash
  * fixed crash when administrating maps
  * fixed crash in AmmoDialog with many weapons
  * units with wait-for-attack can do immediate RF now
  * increased general robustness and error handling
  * wreckag objects have a much lower movemalus than before
  * added maptransformation for buildings in map editor
  * mount can now collect files from different directories
* Fri Oct 20 2006 nadvornik@suse.cz
- updated to bugfix release 1.16.3.0
* Wed Jan 25 2006 mls@suse.de
- converted neededforbuild to BuildRequires
* Thu Nov  3 2005 nadvornik@suse.cz
- fixed to build
* Mon Aug  1 2005 nadvornik@suse.cz
- updated to 1.16.1.0
* Thu Jul  7 2005 mmj@suse.de
- add missing decls
* Wed May 18 2005 yxu@suse.de
- fixed serious compiler warnings: uninitialized variables
* Tue Apr 19 2005 yxu@suse.de
- fixed GCC4 problems
* Thu Feb 10 2005 sbrabec@suse.cz
- Require libsigc++12 in neededforbuild.
* Thu Feb  3 2005 nadvornik@suse.cz
- updated to 1.15.2.1
  * updated campaign
* Sun Aug  8 2004 sndirsch@suse.de
- updated to 1.15.0
  * added manual pages (new)
* Fri Apr 30 2004 nadvornik@suse.cz
- fixed dangerous compiler warnings
* Tue Feb 17 2004 nadvornik@suse.cz
- updated to 1.14.0
  - updated and extended campaign
- added desktop file
* Sat Jan 10 2004 adrian@suse.de
- build as user
* Thu Jul 31 2003 uli@suse.de
- fixed to build on BE platforms by skipping unimplemented float
  conversion
* Mon Jul 28 2003 nadvornik@suse.cz
- updated to 1.13.7
* Wed Apr  9 2003 schwab@suse.de
- Fix broken used of #pragma pack.
* Mon Feb 10 2003 ro@suse.de
- don't run paragui test in configure
* Mon Feb 10 2003 ro@suse.de
- refine last change
* Thu Feb  6 2003 ro@suse.de
- output config.log in case of failure
* Tue Nov 19 2002 ro@suse.de
- fix configure.in for latest autoconf
* Tue Jul 16 2002 nadvornik@suse.cz
- updated to 1.10.2
* Wed Apr 17 2002 nadvornik@suse.cz
- fixed to compile with new gcc
* Fri Feb  1 2002 ro@suse.de
- changed neededforbuild <libpng> to <libpng-devel-packages>
* Mon Nov 26 2001 nadvornik@suse.cz
- update to 1.9.5:
  - working AI and singleplayer game
* Fri Oct 26 2001 ro@suse.de
- use neededforbuild aliases: SDL_devel-pakages, SDL_mixer-packages
* Tue Aug 14 2001 nadvornik@suse.cz
- update to 1.5.9
* Wed Aug  8 2001 ro@suse.de
- changed neededforbuild <kdelibs kdelibs-devel> to <kdelibs-artsd>
* Wed Aug  8 2001 ro@suse.de
- changed neededforbuild <sdl> to <SDL>
- changed neededforbuild <sdl-devel> to <SDL-devel>
* Wed Jun 20 2001 nadvornik@suse.cz
- added kdelibs and kdelibs-devel to neededforbuild
* Mon Mar 26 2001 ro@suse.de
- changed neededforbuild <sdl> to <sdl sdl-devel>
* Thu Mar 22 2001 nadvornik@suse.cz
- update to 1.3.12
* Wed Feb 21 2001 uli@suse.de
- added alsa to neededforbuild (reqd. by new SDL)
* Fri Feb  9 2001 nadvornik@suse.cz
- fixed to compile
* Mon Dec  4 2000 uli@suse.de
- fixed va_arg type violation in edglobal.cpp
* Fri Dec  1 2000 nadvornik@suse.cz
- new package
