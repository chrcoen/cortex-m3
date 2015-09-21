#
# CMakeLists.txt
#
# Copyright (C) 2015, GreenSocs ltd.
#
# Developed Guillaume Delbergue <guillaume.delbergue@greensocs.com>
#

project(DevNull)

#########################
###       CMAKE       ###
#########################

cmake_minimum_required(VERSION 2.8)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

##########################
### REQUIRED LIBRARIES ###
##########################

find_package(SystemC)
if(SystemC_FOUND)
    include_directories(${SystemC_INCLUDE_DIRS})
else()
    message( FATAL_ERROR "SystemC library not found." )
endif()

find_package(TLM)
if(TLM_FOUND)
    include_directories(${TLM_INCLUDE_DIRS})
else()
    message( FATAL_ERROR "TLM library not found." )
endif()

find_package(GREENLIB)
if(GREENLIB_FOUND)
    include_directories(${GREENLIB_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "GreenLib not found.")
endif()

##########################
###       FLAGS        ###
##########################

set(CMAKE_CXX_FLAGS "-Wall -Werror -DSC_INCLUDE_DYNAMIC_PROCESSES")

##########################
###   APB Dual Timer   ###
##########################

set(INCLUDE_DIRS ${INCLUDE_DIRS} include)

include_directories(${INCLUDE_DIRS})

if(MINGW)
    add_library(devnull STATIC src/DevNull.cpp)
else()
    add_library(devnull SHARED src/DevNull.cpp)
endif()    

target_link_libraries(devnull ${SystemC_LIBRARIES})

# Installation paths
install(DIRECTORY include/DevNull DESTINATION include)

# Not sure it's the right destination? Maybe we want lib64 in case of 64bits?
install(TARGETS devnull DESTINATION lib)
