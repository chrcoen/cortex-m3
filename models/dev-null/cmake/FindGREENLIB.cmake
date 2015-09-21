# - Find GreenLib
# This module finds if GreenLib is installed and determines where the
# include files and libraries are.
#

#=============================================================================
# Copyright 2014 GreenSocs
#
# KONRAD Frederic <fred.konrad@greensocs.com>
#=============================================================================

message(STATUS "Searching for GreenLib")

# The HINTS option should only be used for values computed from the system.
set(_GREENLIB_HINTS
    ${GREENLIB_PREFIX}/include
    ${GREENLIB_PREFIX}/lib
    ${GREENLIB_PREFIX}/lib-linux
    ${GREENLIB_PREFIX}/lib-linux64
    ${GREENLIB_PREFIX}/lib-macos
    ${GREENLIB_PREFIX}/lib-macosx64
    ${GREENLIB_PREFIX}/lib-mingw
    ${GREENLIB_PREFIX}/lib-mingw64
    $ENV{GREENLIB_PREFIX}/include
    $ENV{GREENLIB_PREFIX}/lib
    $ENV{GREENLIB_PREFIX}/lib-linux
    $ENV{GREENLIB_PREFIX}/lib-linux64
    $ENV{GREENLIB_PREFIX}/lib-macos
    $ENV{GREENLIB_PREFIX}/lib-macosx64
    $ENV{GREENLIB_PREFIX}/lib-mingw
    $ENV{GREENLIB_PREFIX}/lib-mingw64
    ${CMAKE_INSTALL_PREFIX}/include
    ${CMAKE_INSTALL_PREFIX}/lib
    ${CMAKE_INSTALL_PREFIX}/lib-linux
    ${CMAKE_INSTALL_PREFIX}/lib-linux64
    ${CMAKE_INSTALL_PREFIX}/lib-macos
    ${CMAKE_INSTALL_PREFIX}/lib-macosx64
    ${CMAKE_INSTALL_PREFIX}/lib-mingw
    ${CMAKE_INSTALL_PREFIX}/lib-mingw64
)

# Hard-coded guesses should still go in PATHS. This ensures that the user
# environment can always override hard guesses.
set(_GREENLIB_PATHS
    /usr/include
    /usr/lib
    /usr/lib-linux
    /usr/lib-linux64
    /usr/lib-macos
    /usr/lib-macosx64
    /usr/lib-mingw
    /usr/lib-mingw64
    /usr/local/lib
    /usr/local/lib-linux
    /usr/local/lib-linux64
    /usr/local/lib-macos
    /usr/local/lib-macosx64
    /usr/local/lib-mingw
    /usr/local/lib-mingw64
)

find_path(GREENLIB_INCLUDE_DIRS
    NAMES greencontrol/config.h
    HINTS ${_GREENLIB_HINTS}
    PATHS ${_GREENLIB_PATHS}
)

find_library(_GREENLIB_GREENREG
    NAMES greenreg
    HINTS ${_GREENLIB_HINTS}
    PATHS ${_GREENLIB_PATHS}
)

find_library(_GREENLIB_GREENSCRIPT
    NAMES greenscript
    HINTS ${_GREENLIB_HINTS}
    PATHS ${_GREENLIB_PATHS}
)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

if("${GREENLIB_INCLUDE_DIRS}" MATCHES "GREENLIB_INCLUDE_DIRS-NOTFOUND")
    set(GREENLIB_FOUND FALSE)
else("${GREENLIB_INCLUDE_DIRS}" MATCHES "GREENLIB_INCLUDE_DIRS-NOTFOUND")
    set(GREENLIB_FOUND TRUE)
    list(APPEND GREENLIB_LIBRARIES ${_GREENLIB_GREENREG})
    list(APPEND GREENLIB_LIBRARIES ${_GREENLIB_GREENSCRIPT})
    message(STATUS "GreenLib include directory = ${GREENLIB_INCLUDE_DIRS}")
    message(STATUS "GreenLib libraries = ${GREENLIB_LIBRARIES}")
endif("${GREENLIB_INCLUDE_DIRS}" MATCHES "GREENLIB_INCLUDE_DIRS-NOTFOUND")

