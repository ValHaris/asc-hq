cmake_minimum_required(VERSION 3.16)

project(ASC VERSION 2.8.6.5 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Platform-specific settings
if(MSVC)
    # Visual Studio specific settings
    add_compile_options(/W3 /MP /wd4100 /wd4244 /wd4996)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_WIN32 -DWIN32)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
else()
    # Linux/GCC/Clang settings
    # add_compile_options(-Wall -Wextra -Wpedantic)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options(-Wno-deprecated-declarations -DSIZE_T_not_identical_to_INT)
    endif()
endif()


# Dependencies
if(WIN32)
    find_package(fmt CONFIG REQUIRED)
    
    # Windows: Use vcpkg config package; do not request components here because
    # static triplets can expose only SDL2::SDL2-static.
    find_package(SDL2 CONFIG REQUIRED)
    find_package(SDL2_image CONFIG REQUIRED)
    find_package(SDL2_mixer CONFIG REQUIRED)
    find_package(Lua REQUIRED)
    # find_package(libsigcpp REQUIRED)
    # find_package(sigc++-2 REQUIRED) 
    find_package(Freetype REQUIRED)
    find_package(Boost CONFIG REQUIRED COMPONENTS regex filesystem system)
    find_package(EXPAT REQUIRED)
    find_package(BZip2 REQUIRED)
    find_package(PNG REQUIRED)
    find_package(PhysFS REQUIRED)

    if(NOT DEFINED VCPKG_TARGET_TRIPLET)
        message(FATAL_ERROR "VCPKG_TARGET_TRIPLET must be set for Windows builds")
    endif()

    set(ASC_VCPKG_INSTALLED_DIR "${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}")
    find_library(SIGC_LIBRARY_RELEASE NAMES sigc-2.0
      PATHS "${ASC_VCPKG_INSTALLED_DIR}/lib"
      NO_DEFAULT_PATH
      REQUIRED)
    find_library(SIGC_LIBRARY_DEBUG NAMES sigc-2.0
      PATHS "${ASC_VCPKG_INSTALLED_DIR}/debug/lib"
      NO_DEFAULT_PATH
      REQUIRED)

    include_directories(SDL2::SDL2)
    # Set variables for compatibility with Linux pkg-config style
    set(SDL2_LIBRARIES $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static> $<IF:$<TARGET_EXISTS:SDL2::SDL2main>,SDL2::SDL2main,SDL2::SDL2main-static>)
    set(SDL2IMAGE_LIBRARIES $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>)
    set(SDL2MIXER_LIBRARIES $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>)
    set(SIGC_LIBRARIES
      $<$<CONFIG:Debug>:${SIGC_LIBRARY_DEBUG}>
      $<$<CONFIG:RelWithDebInfo>:${SIGC_LIBRARY_RELEASE}>
      $<$<CONFIG:Release>:${SIGC_LIBRARY_RELEASE}>
      $<$<CONFIG:MinSizeRel>:${SIGC_LIBRARY_RELEASE}>)
    set(LUA_LIBRARIES ${LUA_LIBRARIES})
    # set(PHYSFS_LIBRARIES physfs::physfs)
    
    # Include directories
    set(SDL2_INCLUDE_DIRS ${SDL_INCLUDE_DIR})
    set(SDL2IMAGE_INCLUDE_DIRS "")
    set(SDL2MIXER_INCLUDE_DIRS "")
    set(SIGC_INCLUDE_DIRS "${ASC_VCPKG_INSTALLED_DIR}/include")
    set(LUA_INCLUDE_DIRS ${LUA_INCLUDE_DIR})
else()
    # Linux/Unix: Use pkg-config
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2>=2.25.0)
    pkg_check_modules(SDL2IMAGE REQUIRED SDL2_image)
    pkg_check_modules(SDL2MIXER REQUIRED SDL2_mixer)
    pkg_check_modules(LUA REQUIRED lua5.1)
    pkg_check_modules(SIGC REQUIRED sigc++-2.0)
    find_package(Freetype REQUIRED)
    find_package(Boost CONFIG REQUIRED COMPONENTS regex filesystem system)
    find_package(EXPAT REQUIRED)
    find_package(BZip2 REQUIRED)
    find_package(PNG REQUIRED)
    pkg_check_modules(PHYSFS REQUIRED physfs)
endif()

# Expat target name differs between CMake module and vcpkg config package.
if(TARGET EXPAT::EXPAT)
  set(ASC_EXPAT_TARGET EXPAT::EXPAT)
elseif(TARGET expat::expat)
  set(ASC_EXPAT_TARGET expat::expat)
else()
  message(FATAL_ERROR "Expat target not found after find_package(EXPAT REQUIRED)")
endif()


# Installation directories (can be overridden by user)
include(GNUInstallDirs)
if(WIN32)
  include(InstallRequiredSystemLibraries)
    set(GAME_DATADIR "${CMAKE_INSTALL_PREFIX}/data"
        CACHE PATH "Directory where game data files are installed")
else()
    set(GAME_DATADIR "${CMAKE_INSTALL_FULL_DATADIR}/games/${PROJECT_NAME}"
        CACHE PATH "Directory where game data files are installed")
endif()

# Generate config.h from template
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/config.h"
    @ONLY
)


# Global include dirs used across many targets
set(ASC_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source)
set(ASC_PARAGUI_INCLUDE ${ASC_SRC_DIR}/libs/paragui/include)
set(ASC_LOKI_INCLUDE ${ASC_SRC_DIR}/libs/loki-0.1.6/include)

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}  # For generated config.h
  ${ASC_SRC_DIR}
  ${ASC_PARAGUI_INCLUDE}
  ${ASC_LOKI_INCLUDE}
  ${SDL2_INCLUDE_DIRS}
  ${SDL2IMAGE_INCLUDE_DIRS}
  ${SDL2MIXER_INCLUDE_DIRS}
  ${SIGC_INCLUDE_DIRS}
  ${FREETYPE_INCLUDE_DIRS}
  ${LUA_INCLUDE_DIRS}
  ${EXPAT_INCLUDE_DIRS}
)

if(WIN32)
    add_compile_definitions(_WIN32_ _SDL_ NOMINMAX HAVE_LIMITS)
else()
    add_compile_definitions(_UNIX_ _SDL_)
endif()


if(HAVE_SDL_SOUND)
  message(STATUS "Using system SDL_sound")
  set(SDL_SOUND_LIBRARIES ${SDL_SOUND_LIBRARIES})
  target_link_libraries(sdlsnd PUBLIC ${SDL2_LIBRARIES} ${SDL2MIXER_LIBRARIES} ${SDL_SOUND_LIBRARIES} Boost::regex)
  target_include_directories(sdlsnd PRIVATE ${ASC_SRC_DIR} ${SDL_SOUND_INCLUDE_DIRS})
else()
  message(STATUS "Building internal SDL_sound")
  file(GLOB SDL_SOUND_INT_SRCS CONFIGURE_DEPENDS
    ${ASC_SRC_DIR}/libs/SDL_sound-2.0.4/src/*.c 
    ${ASC_SRC_DIR}/libs/SDL_sound-2.0.4/src/timidity/*.c 
    ${ASC_SRC_DIR}/libs/SDL_sound-2.0.4/src/libmodplug/*.c 
    )

  list(REMOVE_ITEM SDL_SOUND_INT_SRCS 
    ${ASC_SRC_DIR}/libs/SDL_sound-2.0.4/src/SDL_sound_skeleton.c
    )
  add_library(SDL_sound_int STATIC ${SDL_SOUND_INT_SRCS})
  target_compile_definitions(SDL_sound_int PRIVATE SOUND_SUPPORTS_OGG SOUND_SUPPORTS_WAV SOUND_SUPPORTS_MP3)
  target_include_directories(SDL_sound_int PUBLIC ${ASC_SRC_DIR}/libs/SDL_sound-2.0.4 ${SDL2_INCLUDE_DIRS})
  if(TARGET SDL2::SDL2)
    target_include_directories(SDL_sound_int PUBLIC $<TARGET_PROPERTY:SDL2::SDL2,INTERFACE_INCLUDE_DIRECTORIES>)
  endif()
  # Add SDL_sound include directory globally so sound.cpp can find SDL_sound.h
  include_directories(${ASC_SRC_DIR}/libs/SDL_sound-2.0.4/src)
endif()

# Common compile flags for executables
set(ASC_COMMON_DEFS sgmain SOUND_SUPPORTS_MP3 SOUND_SUPPORTS_OGG SOUND_SUPPORTS_WAV)

# ParaGUI sources
file(GLOB PARAGUI_SOURCES CONFIGURE_DEPENDS
  ${ASC_SRC_DIR}/libs/paragui/src/core/*.cpp
  ${ASC_SRC_DIR}/libs/paragui/src/draw/*.cpp
  ${ASC_SRC_DIR}/libs/paragui/src/font/*.cpp
  ${ASC_SRC_DIR}/libs/paragui/src/themes/*.cpp
  ${ASC_SRC_DIR}/libs/paragui/src/widgets/*.cpp
)

# Common sources used by asc and unittester
file(GLOB COMMON_GAME_SOURCES CONFIGURE_DEPENDS
  ${ASC_SRC_DIR}/soundList.cpp
  ${ASC_SRC_DIR}/typen.cpp
  ${ASC_SRC_DIR}/applicationstarter.cpp
  ${ASC_SRC_DIR}/strtmesg.cpp
  ${ASC_SRC_DIR}/stack.cpp
  ${ASC_SRC_DIR}/spfst.cpp
  ${ASC_SRC_DIR}/sgstream.cpp
  ${ASC_SRC_DIR}/palette.cpp
  ${ASC_SRC_DIR}/newfont.cpp
  ${ASC_SRC_DIR}/misc.cpp
  ${ASC_SRC_DIR}/loadpcxc.cpp
  ${ASC_SRC_DIR}/loaders.cpp
  ${ASC_SRC_DIR}/tos.cpp
  ${ASC_SRC_DIR}/loadbi3.cpp
  ${ASC_SRC_DIR}/gamedlg.cpp
  ${ASC_SRC_DIR}/dlg_box.cpp
  ${ASC_SRC_DIR}/dialog.cpp
  ${ASC_SRC_DIR}/controls.cpp
  ${ASC_SRC_DIR}/basestrm.cpp
  ${ASC_SRC_DIR}/basegfx.cpp
  ${ASC_SRC_DIR}/attack.cpp
  ${ASC_SRC_DIR}/replaymapdisplay.cpp
  ${ASC_SRC_DIR}/memory-measurement.cpp
  ${ASC_SRC_DIR}/gameoptions.cpp
  ${ASC_SRC_DIR}/astar2.cpp
  ${ASC_SRC_DIR}/vehicletype.cpp
  ${ASC_SRC_DIR}/objects.cpp
  ${ASC_SRC_DIR}/stdio-errorhandler.cpp
  ${ASC_SRC_DIR}/buildingtype.cpp
  ${ASC_SRC_DIR}/containerbase.cpp
  ${ASC_SRC_DIR}/mapalgorithms.cpp
  ${ASC_SRC_DIR}/viewcalculation.cpp
  ${ASC_SRC_DIR}/gamemap.cpp
  ${ASC_SRC_DIR}/password.cpp
  ${ASC_SRC_DIR}/replay.cpp
  ${ASC_SRC_DIR}/research.cpp
  ${ASC_SRC_DIR}/fieldimageloader.cpp
  ${ASC_SRC_DIR}/mapdisplay.cpp
  ${ASC_SRC_DIR}/resourcenet.cpp
  ${ASC_SRC_DIR}/ascstring.cpp
  ${ASC_SRC_DIR}/graphicset.cpp
  ${ASC_SRC_DIR}/vehicle.cpp
  ${ASC_SRC_DIR}/buildings.cpp
  ${ASC_SRC_DIR}/networkinterface.cpp
  ${ASC_SRC_DIR}/textfileparser.cpp
  ${ASC_SRC_DIR}/terraintype.cpp
  ${ASC_SRC_DIR}/objecttype.cpp
  ${ASC_SRC_DIR}/textfiletags.cpp
  ${ASC_SRC_DIR}/itemrepository.cpp
  ${ASC_SRC_DIR}/stringtokenizer.cpp
  ${ASC_SRC_DIR}/music.cpp
  ${ASC_SRC_DIR}/messages.cpp
  ${ASC_SRC_DIR}/paradialog.cpp
  ${ASC_SRC_DIR}/textfile_evaluation.cpp
  ${ASC_SRC_DIR}/containerbasetype.cpp
  ${ASC_SRC_DIR}/messagedlg.cpp
  ${ASC_SRC_DIR}/simplestream.cpp
  ${ASC_SRC_DIR}/mappolygons.cpp
  ${ASC_SRC_DIR}/prehistoricevents.cpp
  ${ASC_SRC_DIR}/gameevents.cpp
  ${ASC_SRC_DIR}/gameeventsystem.cpp
  ${ASC_SRC_DIR}/gameevent_dialogs.cpp
  ${ASC_SRC_DIR}/statisticdialog.cpp
  ${ASC_SRC_DIR}/clipboard.cpp
  ${ASC_SRC_DIR}/geometry.cpp
  ${ASC_SRC_DIR}/guidimension.cpp
  ${ASC_SRC_DIR}/guiiconhandler.cpp
  ${ASC_SRC_DIR}/guifunctions.cpp
  ${ASC_SRC_DIR}/iconrepository.cpp
  ${ASC_SRC_DIR}/overviewmapimage.cpp
  ${ASC_SRC_DIR}/mapfield.cpp
  ${ASC_SRC_DIR}/overviewmappanel.cpp
  ${ASC_SRC_DIR}/dashboard.cpp
  ${ASC_SRC_DIR}/asc-mainscreen.cpp
  ${ASC_SRC_DIR}/explosivemines.cpp
  ${ASC_SRC_DIR}/mainscreenwidget.cpp
  ${ASC_SRC_DIR}/networksupervisor.cpp
  ${ASC_SRC_DIR}/statistics.cpp
  ${ASC_SRC_DIR}/legacynetworkloaders.cpp
  ${ASC_SRC_DIR}/player.cpp
  ${ASC_SRC_DIR}/cannedmessages.cpp
  ${ASC_SRC_DIR}/reactionfire.cpp
  ${ASC_SRC_DIR}/mapimageexport.cpp
  ${ASC_SRC_DIR}/containercontrols.cpp
  ${ASC_SRC_DIR}/containerbase-functions.cpp
  ${ASC_SRC_DIR}/windowing.cpp
  ${ASC_SRC_DIR}/turncontrol.cpp
  ${ASC_SRC_DIR}/unitset.cpp
  ${ASC_SRC_DIR}/playercolor.cpp
  ${ASC_SRC_DIR}/pg_mapdisplay.cpp
  ${ASC_SRC_DIR}/autotraining.cpp
  ${ASC_SRC_DIR}/researchexecution.cpp
  ${ASC_SRC_DIR}/luacommandwriter.cpp
  ${ASC_SRC_DIR}/campaignactionrecorder.cpp
  ${ASC_SRC_DIR}/loggingoutput.cpp
  ${ASC_SRC_DIR}/versionidentifier.cpp
  ${ASC_SRC_DIR}/package.cpp
  ${ASC_SRC_DIR}/packagemanager.cpp
  ${ASC_SRC_DIR}/packagerepository.cpp
  ${ASC_SRC_DIR}/dataversioncheck.cpp
  ${ASC_SRC_DIR}/i18n.cpp
  ${ASC_SRC_DIR}/accessconstraints-game.cpp
  ${ASC_SRC_DIR}/contextutils.cpp
  ${ASC_SRC_DIR}/unitcostcalculator-standard.cpp
  ${ASC_SRC_DIR}/weaponrangelayer.cpp
  ${ASC_SRC_DIR}/unitcostcalculator-pbp.cpp
  ${ASC_SRC_DIR}/dialogs/newgame.cpp
  ${ASC_SRC_DIR}/tasks/*.cpp
  ${ASC_SRC_DIR}/actions/*.cpp
  ${ASC_SRC_DIR}/sdl/sound.cpp
  ${ASC_SRC_DIR}/dialogs/soundsettings.cpp
)

file(GLOB ASC_EXTRA_SOURCES CONFIGURE_DEPENDS
  ${ASC_SRC_DIR}/ai/*.cpp
  ${ASC_SRC_DIR}/lua/commands.cpp
  ${ASC_SRC_DIR}/lua/commands_wrap.cxx
  ${ASC_SRC_DIR}/lua/luarunner.cpp
  ${ASC_SRC_DIR}/lua/luastate.cpp
  ${ASC_SRC_DIR}/lua/common.cpp
)

# used by editor and game
file(GLOB COMMON_SOURCES CONFIGURE_DEPENDS
  ${ASC_SRC_DIR}/libs/triangul/*.c
  ${ASC_SRC_DIR}/libs/loki-0.1.6/src/SmallObj.cpp 
  ${ASC_SRC_DIR}/libs/loki-0.1.6/src/Singleton.cpp 
  ${ASC_SRC_DIR}/libs/loki-0.1.6/src/SmartPtr.cpp
  ${ASC_SRC_DIR}/graphics/*.cpp
  ${ASC_SRC_DIR}/widgets/*.cpp
  ${ASC_SRC_DIR}/dialogs/editmapparam.cpp
  ${ASC_SRC_DIR}/dialogs/cargowidget.cpp
  ${ASC_SRC_DIR}/dialogs/selectionwindow.cpp
  ${ASC_SRC_DIR}/dialogs/fileselector.cpp
  ${ASC_SRC_DIR}/dialogs/playersetup.cpp
  ${ASC_SRC_DIR}/dialogs/alliancesetup.cpp
  ${ASC_SRC_DIR}/dialogs/emailsetup.cpp
  ${ASC_SRC_DIR}/dialogs/unitinfodialog.cpp
  ${ASC_SRC_DIR}/dialogs/editgameoptions.cpp
  ${ASC_SRC_DIR}/dialogs/vehicletypeselector.cpp
  ${ASC_SRC_DIR}/dialogs/fieldmarker.cpp
  ${ASC_SRC_DIR}/dialogs/admingame.cpp
  ${ASC_SRC_DIR}/dialogs/pwd_dlg.cpp
  ${ASC_SRC_DIR}/dialogs/terraininfo.cpp
  ${ASC_SRC_DIR}/dialogs/messagedialog.cpp
  ${ASC_SRC_DIR}/dialogs/locatefile.cpp
  ${ASC_SRC_DIR}/dialogs/unitguidedialog.cpp
  ${ASC_SRC_DIR}/libs/sdlmm/src/sdlmm_global.cpp
  ${ASC_SRC_DIR}/libs/sdlmm/src/sdlmm_basesurface.cpp
  ${ASC_SRC_DIR}/libs/sdlmm/src/sdlmm_surface.cpp
  ${ASC_SRC_DIR}/libs/sdlmm/src/sdlmm_pixelformat.cpp
  ${ASC_SRC_DIR}/libs/sdlmm/src/sdlmm_srect.cpp
  ${ASC_SRC_DIR}/libs/sdlmm/src/sdlmm_timer.cpp
  ${ASC_SRC_DIR}/sdl/events.cpp
  ${ASC_SRC_DIR}/sdl/graphics.cpp
  ${ASC_SRC_DIR}/util/messaginghub.cpp

  ${ASC_SRC_DIR}/actions/action.cpp
  ${ASC_SRC_DIR}/actions/command.cpp 
  ${ASC_SRC_DIR}/actions/context.cpp 
  ${ASC_SRC_DIR}/actions/actionresult.cpp 
  ${ASC_SRC_DIR}/actions/actioncontainer.cpp 
  ${ASC_SRC_DIR}/actions/containeraction.cpp
  ${ASC_SRC_DIR}/tasks/abstracttaskcontainer.cpp 
  ${ASC_SRC_DIR}/tasks/taskhibernatingcontainer.cpp
)

# asc executable
add_executable(asc
  ${COMMON_GAME_SOURCES}
  ${ASC_EXTRA_SOURCES}
  ${PARAGUI_SOURCES}
  ${COMMON_SOURCES}
  ${SDL_SOUND_INT_SRCS}
  ${ASC_SRC_DIR}/sg.cpp
  ${ASC_SRC_DIR}/gamedialog.cpp
  ${ASC_SRC_DIR}/network/simple_file_transfer.cpp
  ${ASC_SRC_DIR}/network/pbem-server-dummy.cpp
  ${ASC_SRC_DIR}/dialogs/cargodialog.cpp
  ${ASC_SRC_DIR}/dialogs/unitcounting.cpp
  ${ASC_SRC_DIR}/dialogs/cargowidget.cpp
  ${ASC_SRC_DIR}/dialogs/ammotransferdialog.cpp
  ${ASC_SRC_DIR}/dialogs/attackpanel.cpp
  ${ASC_SRC_DIR}/dialogs/vehicletypeselector.cpp
  ${ASC_SRC_DIR}/dialogs/nextcampaignmap.cpp
  ${ASC_SRC_DIR}/dialogs/editplayerdata.cpp
  ${ASC_SRC_DIR}/dialogs/buildingtypeselector.cpp
  ${ASC_SRC_DIR}/dialogs/choosetech.cpp
  ${ASC_SRC_DIR}/dialogs/productionanalysis.cpp
  ${ASC_SRC_DIR}/dialogs/internalAmmoTransferDialog.cpp
  ${ASC_SRC_DIR}/dialogs/replayrecorder.cpp
  ${ASC_SRC_DIR}/dialogs/mailoptionseditor.cpp
  ${ASC_SRC_DIR}/dialogs/vehicleproductionselection.cpp
  ${ASC_SRC_DIR}/dialogs/infodialogs.cpp
  ${ASC_SRC_DIR}/dialogs/actionmanager.cpp
  ${ASC_SRC_DIR}/dialogs/gotoposition.cpp
  ${ASC_SRC_DIR}/dialogs/taskmanager.cpp
  ${ASC_SRC_DIR}/dialogs/actionwidget.cpp
  ${ASC_SRC_DIR}/dialogs/unitnaming.cpp
  ${ASC_SRC_DIR}/dialogs/eventinfo.cpp
  ${ASC_SRC_DIR}/dialogs/researchinfo.cpp
  ${ASC_SRC_DIR}/libs/getopt/getopt.c
  ${ASC_SRC_DIR}/libs/getopt/getopt1.c
  )

if(WIN32)
  target_sources(asc PRIVATE
    ${ASC_SRC_DIR}/win32/msvc/mdump.cpp
    ${ASC_SRC_DIR}/win32/win32-errormsg.cpp
    ${ASC_SRC_DIR}/win32/msvc/ASC/ASC.rc
  )
endif()

target_compile_definitions(asc PRIVATE ${ASC_COMMON_DEFS})
target_include_directories(asc PRIVATE ${ASC_SRC_DIR} ${ASC_PARAGUI_INCLUDE} ${ASC_LOKI_INCLUDE} ${ASC_SRC_DIR}/libs/getopt)

target_link_libraries(asc PRIVATE
  ${SDL2_LIBRARIES}
  ${SDL2IMAGE_LIBRARIES}
  ${SDL2MIXER_LIBRARIES}
  ${SIGC_LIBRARIES}
  Freetype::Freetype
  ${LUA_LIBRARIES}
  Boost::regex
  Boost::filesystem
  Boost::system
  ${ASC_EXPAT_TARGET}
  PNG::PNG
  ${PHYSFS_LIBRARIES}
  BZip2::BZip2
  $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
  )


# Windows-specific settings
if(WIN32)
    set_target_properties(asc PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

# Map editor (optional)
function(asc_configure_mapeditor_target target_name)
  target_include_directories(${target_name} PRIVATE
    ${ASC_SRC_DIR}
    ${ASC_PARAGUI_INCLUDE}
    ${ASC_LOKI_INCLUDE}
    ${ASC_SRC_DIR}/libs/getopt)

  target_link_libraries(${target_name} PRIVATE
    ${SDL2_LIBRARIES}
    ${SDL2IMAGE_LIBRARIES}
    ${SDL2MIXER_LIBRARIES}
    ${SIGC_LIBRARIES}
    Freetype::Freetype
    ${LUA_LIBRARIES}
    Boost::regex
    Boost::filesystem
    ${ASC_EXPAT_TARGET}
    PNG::PNG
    ${PHYSFS_LIBRARIES}
    BZip2::BZip2
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>)

endfunction()

  set(MAPEDIT_SOURCES
    ${ASC_SRC_DIR}/edgen.cpp
    ${ASC_SRC_DIR}/edglobal.cpp
    ${ASC_SRC_DIR}/edmain.cpp
    ${ASC_SRC_DIR}/edselfnt.cpp
    ${ASC_SRC_DIR}/edmisc.cpp
    ${ASC_SRC_DIR}/typen.cpp
    ${ASC_SRC_DIR}/strtmesg.cpp
    ${ASC_SRC_DIR}/stack.cpp
    ${ASC_SRC_DIR}/spfst.cpp
    ${ASC_SRC_DIR}/sgstream.cpp
    ${ASC_SRC_DIR}/palette.cpp
    ${ASC_SRC_DIR}/newfont.cpp
    ${ASC_SRC_DIR}/misc.cpp
    ${ASC_SRC_DIR}/loadpcxc.cpp
    ${ASC_SRC_DIR}/loaders.cpp
    ${ASC_SRC_DIR}/loadbi3.cpp
    ${ASC_SRC_DIR}/dlg_box.cpp
    ${ASC_SRC_DIR}/dialog.cpp
    ${ASC_SRC_DIR}/basestrm.cpp
    ${ASC_SRC_DIR}/basegfx.cpp
    ${ASC_SRC_DIR}/attack.cpp
    ${ASC_SRC_DIR}/gameoptions.cpp
    ${ASC_SRC_DIR}/maped-mainscreen.cpp
    ${ASC_SRC_DIR}/stdio-errorhandler.cpp
    ${ASC_SRC_DIR}/buildingtype.cpp
    ${ASC_SRC_DIR}/vehicletype.cpp
    ${ASC_SRC_DIR}/containerbase.cpp
    ${ASC_SRC_DIR}/ed_mapcomponent.cpp
    ${ASC_SRC_DIR}/mapalgorithms.cpp
    ${ASC_SRC_DIR}/viewcalculation.cpp
    ${ASC_SRC_DIR}/gamemap.cpp
    ${ASC_SRC_DIR}/password.cpp
    ${ASC_SRC_DIR}/research.cpp
    ${ASC_SRC_DIR}/mapdisplay.cpp
    ${ASC_SRC_DIR}/ascstring.cpp
    ${ASC_SRC_DIR}/graphicset.cpp
    ${ASC_SRC_DIR}/vehicle.cpp
    ${ASC_SRC_DIR}/buildings.cpp
    ${ASC_SRC_DIR}/networkinterface.cpp
    ${ASC_SRC_DIR}/resourcenet.cpp
    ${ASC_SRC_DIR}/textfileparser.cpp
    ${ASC_SRC_DIR}/terraintype.cpp
    ${ASC_SRC_DIR}/objecttype.cpp
    ${ASC_SRC_DIR}/textfiletags.cpp
    ${ASC_SRC_DIR}/itemrepository.cpp
    ${ASC_SRC_DIR}/stringtokenizer.cpp
    ${ASC_SRC_DIR}/music.cpp
    ${ASC_SRC_DIR}/messages.cpp
    ${ASC_SRC_DIR}/paradialog.cpp
    ${ASC_SRC_DIR}/textfile_evaluation.cpp
    ${ASC_SRC_DIR}/containerbasetype.cpp
    ${ASC_SRC_DIR}/simplestream.cpp
    ${ASC_SRC_DIR}/mappolygons.cpp
    ${ASC_SRC_DIR}/prehistoricevents.cpp
    ${ASC_SRC_DIR}/gameevents.cpp
    ${ASC_SRC_DIR}/gameeventsystem.cpp
    ${ASC_SRC_DIR}/gameevent_dialogs.cpp
    ${ASC_SRC_DIR}/clipboard.cpp
    ${ASC_SRC_DIR}/resourceplacement.cpp
    ${ASC_SRC_DIR}/resourceplacementdialog.cpp
    ${ASC_SRC_DIR}/geometry.cpp
    ${ASC_SRC_DIR}/guidimension.cpp
    ${ASC_SRC_DIR}/unitset.cpp
    ${ASC_SRC_DIR}/fieldimageloader.cpp
    ${ASC_SRC_DIR}/iconrepository.cpp
    ${ASC_SRC_DIR}/overviewmapimage.cpp
    ${ASC_SRC_DIR}/mapfield.cpp
    ${ASC_SRC_DIR}/objects.cpp
    ${ASC_SRC_DIR}/overviewmappanel.cpp
    ${ASC_SRC_DIR}/explosivemines.cpp
    ${ASC_SRC_DIR}/legacynetworkloaders.cpp
    ${ASC_SRC_DIR}/statistics.cpp
    ${ASC_SRC_DIR}/player.cpp
    ${ASC_SRC_DIR}/cannedmessages.cpp
    ${ASC_SRC_DIR}/mapimageexport.cpp
    ${ASC_SRC_DIR}/mainscreenwidget.cpp
    ${ASC_SRC_DIR}/windowing.cpp
    ${ASC_SRC_DIR}/playercolor.cpp
    ${ASC_SRC_DIR}/campaignactionrecorder.cpp
    ${ASC_SRC_DIR}/luacommandwriter.cpp
    ${ASC_SRC_DIR}/versionidentifier.cpp
    ${ASC_SRC_DIR}/package.cpp
    ${ASC_SRC_DIR}/packagemanager.cpp
    ${ASC_SRC_DIR}/packagerepository.cpp
    ${ASC_SRC_DIR}/dataversioncheck.cpp
    ${ASC_SRC_DIR}/i18n.cpp
    ${ASC_SRC_DIR}/accessconstraints-editor.cpp
    ${ASC_SRC_DIR}/unitcostcalculator-standard.cpp
    ${ASC_SRC_DIR}/weaponrangelayer.cpp
    ${ASC_SRC_DIR}/unitcostcalculator-pbp.cpp
    ${ASC_SRC_DIR}/dialogs/cargoeditor.cpp
    ${ASC_SRC_DIR}/dialogs/cargowidget.cpp
    ${ASC_SRC_DIR}/dialogs/eventeditor.cpp
    ${ASC_SRC_DIR}/dialogs/newmap.cpp
    ${ASC_SRC_DIR}/dialogs/exchangegraphics.cpp
    ${ASC_SRC_DIR}/dialogs/importbi3map.cpp
    ${ASC_SRC_DIR}/dialogs/edittechadapter.cpp
    ${ASC_SRC_DIR}/dialogs/vehiclepropertyeditor.cpp
    ${ASC_SRC_DIR}/dialogs/buildingpropertyeditor.cpp
    ${ASC_SRC_DIR}/lua/mapedcommands.cpp 
    ${ASC_SRC_DIR}/lua/mapedcommands_wrap.cxx 
    ${ASC_SRC_DIR}/lua/luarunner.cpp 
    ${ASC_SRC_DIR}/lua/luastate.cpp 
    ${ASC_SRC_DIR}/lua/common.cpp
    ${ASC_SRC_DIR}/actions/spawnobject.cpp 
    ${ASC_SRC_DIR}/actions/spawnmine.cpp 
    ${ASC_SRC_DIR}/actions/changeview.cpp 
    ${ASC_SRC_DIR}/actions/removemine.cpp 
    ${ASC_SRC_DIR}/actions/removeobject.cpp 
    ${ASC_SRC_DIR}/actions/changeobjectproperty.cpp
    ${ASC_SRC_DIR}/actions/changeunitproperty.cpp
    ${ASC_SRC_DIR}/actions/unitaction.cpp
    ${ASC_SRC_DIR}/actions/inflictdamage.cpp
    ${ASC_SRC_DIR}/actions/consumeammo.cpp
    ${ASC_SRC_DIR}/actions/consumeresource.cpp
    ${ASC_SRC_DIR}/actions/unitfieldregistration.cpp
    ${ASC_SRC_DIR}/actions/viewregistration.cpp
    ${ASC_SRC_DIR}/actions/changeunitmovement.cpp
    ${ASC_SRC_DIR}/actions/registerunitrftarget.cpp
    ${ASC_SRC_DIR}/actions/destructcontainer.cpp
    ${ASC_SRC_DIR}/libs/getopt/getopt.c
    ${ASC_SRC_DIR}/libs/getopt/getopt1.c
  )

  add_executable(asc_mapedit 
    ${MAPEDIT_SOURCES} 
    ${PARAGUI_SOURCES}
    ${COMMON_SOURCES})

  add_executable(asc_pbpedit 
    ${MAPEDIT_SOURCES} 
    ${PARAGUI_SOURCES}
    ${COMMON_SOURCES})

  if(WIN32)
    target_sources(asc_mapedit PRIVATE ${ASC_SRC_DIR}/win32/msvc/mdump.cpp ${ASC_SRC_DIR}/win32/win32-errormsg.cpp ${ASC_SRC_DIR}/win32/msvc/mapeditor/mapeditor.rc)
    target_sources(asc_pbpedit PRIVATE ${ASC_SRC_DIR}/win32/msvc/mdump.cpp ${ASC_SRC_DIR}/win32/win32-errormsg.cpp ${ASC_SRC_DIR}/win32/msvc/mapeditor/mapeditor.rc)    
  endif()
  target_compile_definitions(asc_mapedit PRIVATE karteneditor)
  target_compile_definitions(asc_pbpedit PRIVATE karteneditor pbpeditor)
  target_sources(asc_pbpedit PRIVATE ${ASC_SRC_DIR}/network/simple_file_transfer.cpp)

  target_include_directories(asc_pbpedit PRIVATE ${ASC_SRC_DIR}/unix/pbpedit)

  asc_configure_mapeditor_target(asc_mapedit)
  asc_configure_mapeditor_target(asc_pbpedit)

# Windows-specific settings for map editor
if(WIN32)
  set_target_properties(asc PROPERTIES OUTPUT_NAME "asc2")
  set_target_properties(asc_mapedit PROPERTIES WIN32_EXECUTABLE TRUE OUTPUT_NAME "mapeditor2")
  set_target_properties(asc_pbpedit PROPERTIES WIN32_EXECUTABLE TRUE OUTPUT_NAME "pbpeditor2")
else()
  set_target_properties(asc PROPERTIES OUTPUT_NAME "asc")
  set_target_properties(asc_mapedit PROPERTIES OUTPUT_NAME "asc_mapedit")
  set_target_properties(asc_pbpedit PROPERTIES OUTPUT_NAME "asc_pbpedit")
endif()

install(TARGETS asc asc_mapedit asc_pbpedit
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(WIN32 AND CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
  install(PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    OPTIONAL
  )
endif()

message(STATUS "\nConfiguration summary:")
message(STATUS "  SDL2 include: ${SDL2_INCLUDE_DIRS}")
message(STATUS "  Lua include: ${LUA_INCLUDE_DIRS}")
message(STATUS "  Using SDL_sound: ${HAVE_SDL_SOUND}")
