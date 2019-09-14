# This module looks for CCE4510 module and determines where the header files
# and libraries are.

#=============================================================================
# Copyright 2014 GreenSocs
#
# KONRAD Frederic <fred.konrad@greensocs.com>
#
#=============================================================================

MESSAGE(STATUS "Searching for CCE4510 model.")

# The HINTS option should only be used for values computed from the system.
SET(_CCE4510_HINTS
  ${CCE4510_PREFIX}/include
  ${CCE4510_PREFIX}/lib
  ${CCE4510_PREFIX}/lib64
  $ENV{CCE4510_PREFIX}/include
  $ENV{CCE4510_PREFIX}/lib
  $ENV{CCE4510_PREFIX}/lib64
  ${CMAKE_INSTALL_PREFIX}/include
  ${CMAKE_INSTALL_PREFIX}/lib
  ${CMAKE_INSTALL_PREFIX}/lib64
  )
# Hard-coded guesses should still go in PATHS. This ensures that the user
# environment can always override hard guesses.
SET(_CCE4510_PATHS
  /usr/include
  /usr/lib
  /usr/lib64
  /usr/local/lib
  /usr/local/lib64
  )

FIND_PATH(CCE4510_INCLUDE_DIRS
  NAMES CCE4510/CCE4510.h
  HINTS ${_CCE4510_HINTS}
  PATHS ${_CCE4510_PATHS}
)

FIND_PATH(CCE4510_LIBRARY_DIRS
  NAMES libCCE4510.so
  HINTS ${_CCE4510_HINTS}
  PATHS ${_CCE4510_PATHS}
)

if("${CCE4510_INCLUDE_DIRS}" MATCHES "CCE4510_INCLUDE_DIRS-NOTFOUND")
    SET(CCE4510_FOUND FALSE)
else("${CCE4510_INCLUDE_DIRS}" MATCHES "CCE4510_INCLUDE_DIRS-NOTFOUND")
    SET(CCE4510_FOUND TRUE)
    MESSAGE(STATUS "CCE4510 include directory = ${CCE4510_INCLUDE_DIRS}")
    MESSAGE(STATUS "CCE4510 library directory = ${CCE4510_LIBRARY_DIRS}")
    SET(CCE4510_LIBRARIES CCE4510)
endif("${CCE4510_INCLUDE_DIRS}" MATCHES "CCE4510_INCLUDE_DIRS-NOTFOUND")

