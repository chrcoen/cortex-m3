#
# CMakeLists.txt
#
# Copyright (C) 2015, GreenSocs ltd.
#
# Developed by Guillaume Delbergue <guillaume.delbergue@greensocs.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or (at
# your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/licenses>.
#
# Linking GreenSocs code, statically or dynamically with other modules
# is making a combined work based on GreenSocs code. Thus, the terms and
# conditions of the GNU General Public License cover the whole
# combination.
#
# In addition, as a special exception, the copyright holders, GreenSocs
# Ltd, give you permission to combine GreenSocs code with free software
# programs or libraries that are released under the GNU LGPL, under the
# OSCI license, under the OCP TLM Kit Research License Agreement or
# under the OVP evaluation license.You may copy and distribute such a
# system following the terms of the GNU GPL and the licenses of the
# other code concerned.
#
# Note that people who make modified versions of GreenSocs code are not
# obligated to grant this special exception for their modified versions;
# it is their choice whether to do so. The GNU General Public License
# gives permission to release a modified version without this exception;
# this exception also makes it possible to release a modified version
# which carries forward this exception.

cmake_minimum_required(VERSION 2.8)
include(ExternalProject)

project(M3-Platform)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

include_directories(include)

set(CMAKE_CXX_FLAGS "-ggdb -DSC_INCLUDE_DYNAMIC_PROCESSES -Wall -Werror")
if(MINGW)
endif(MINGW)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if (WIN32 AND CMAKE_SYSTEM_VERSION)
    set(ver ${CMAKE_SYSTEM_VERSION})
    string(REPLACE "." "" ver ${ver})
    string(REGEX REPLACE "([0-9])" "0\\1" ver ${ver})
    set(version "0x${ver}")
#https://msdn.microsoft.com/en-us/library/windows/desktop/aa383745(v=vs.85).aspx
    add_definitions(-D_WIN32_WINNT=${version})
endif()

#REQUIRED LIBRARIES.
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

find_package(Lua REQUIRED)
if(LUA_FOUND)
    include_directories(${LUA_INCLUDE_DIR})
else()
    message( FATAL_ERROR "LUA library not found." )
endif()

find_package(Boost 1.40.0 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_subdirectory(libs/greenlib)
include_directories(libs/greenlib/greencontrol/include
                    libs/greenlib/greenmemory/include
                    libs/greenlib/greenscript/include
                    libs/greenlib/greensignalbus/include
                    libs/greenlib/greensocket/include
                    libs/greenlib/greenmessage/include
                    libs/greenlib/greenrouter/include
                    libs/greenlib/greenserialsocket/include
                    libs/greenlib/greensignalsocket/include
                    libs/greenlib/gsgpsocket/include
                    libs/greenlib/greenreg/include
                    libs/greenlib/greenthreads/include
                    libs/greenlib/include
)
link_directories(${CMAKE_SOURCE_DIR}/libs/greenlib/greenreg/lib
                 ${CMAKE_SOURCE_DIR}/libs/greenlib/greencontrol/lib
                 ${CMAKE_SOURCE_DIR}/libs/greenlib/greenmemory/lib
                 ${CMAKE_SOURCE_DIR}/libs/greenlib/greenscript/lib
                 ${CMAKE_SOURCE_DIR}/libs/greenlib/greensignalbus/lib
                 ${CMAKE_SOURCE_DIR}/libs/greenlib/greensocket/lib
                 ${CMAKE_SOURCE_DIR}/libs/greenlib/greenmessage/lib
                 ${CMAKE_SOURCE_DIR}/libs/greenlib/greenrouter/lib
                 ${CMAKE_SOURCE_DIR}/libs/greenlib/greenserialsocket/lib
                 ${CMAKE_SOURCE_DIR}/libs/greenlib/greensignalsocket/lib
                 ${CMAKE_SOURCE_DIR}/libs/greenlib/gsgpsocket/lib
)

set(GREENLIB_FOUND TRUE)
set(GREENLIB_LIBRARY_DIRS ${CMAKE_SOURCE_DIR}/libs/greenlib/greenreg/lib)
set(GREENLIB_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/libs/greenlib/greencontrol/include)
set(GREENLIB_GREENREG greenreg)
set(_GREENLIB_GREENREG greenreg)
if(MINGW)
    set(_GREENLIB_GREENSCRIPT "")
    set(GREENLIB_GREENSCRIPT "")
else()
    set(_GREENLIB_GREENSCRIPT greenscript)
    set(GREENLIB_GREENSCRIPT greenscript)
endif()

add_subdirectory(libs/tlm2c libs/tlm2c.build/lib)
include_directories(libs/tlm2c/include)
link_directories(${CMAKE_SOURCE_DIR}/libs/tlm2c.build)
file(COPY libs/tlm2c/include DESTINATION libs/tlm2c.build/)

set(TLM2C_FOUND TRUE)
set(TLM2C_LIBRARY_DIRS ${CMAKE_SOURCE_DIR}/libs/tlm2c.build)
set(TLM2C_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/libs/tlm2c/include)

add_subdirectory(models/cpu models/cpu.build)
include_directories(models/cpu/include)
link_directories(${CMAKE_SOURCE_DIR}/models/cpu.build)

add_subdirectory(models/memory models/memory.build)
include_directories(models/memory/include)
link_directories(${CMAKE_SOURCE_DIR}/models/memory.build)

add_subdirectory(backends/tcp-serial backends/tcp-serial.build)
include_directories(backends/tcp-serial/include)
link_directories(${CMAKE_SOURCE_DIR}/backends/tcp-serial.build)

add_subdirectory(models/apb_uart models/apb_uart.build)
include_directories(models/apb_uart/include)
link_directories(${CMAKE_SOURCE_DIR}/models/apb_uart.build)

add_subdirectory(models/cce4510 models/cce4510.build)
include_directories(models/cce4510/include)
link_directories(${CMAKE_SOURCE_DIR}/models/cce4510.build)

add_subdirectory(models/pl022 models/pl022.build)
include_directories(models/pl022/include)
link_directories(${CMAKE_SOURCE_DIR}/models/pl022.build)

add_subdirectory(models/apb-timer models/apb-timer.build)
include_directories(models/apb-timer/include)
link_directories(${CMAKE_SOURCE_DIR}/models/apb-timer.build)

add_subdirectory(models/apb-dual-timer models/apb-dual-timer.build)
include_directories(models/apb-dual-timer/include)
link_directories(${CMAKE_SOURCE_DIR}/models/apb-dual-timer.build)

add_subdirectory(models/dev-null models/dev-null.build)
include_directories(models/dev-null/include)
link_directories(${CMAKE_SOURCE_DIR}/models/dev-null.build)

add_subdirectory(models/ahb-gpio models/ahb-gpio.build)
include_directories(models/ahb-gpio/include)
link_directories(${CMAKE_SOURCE_DIR}/models/ahb-gpio.build)

add_subdirectory(models/apb-watchdog models/apb-watchdog.build)
include_directories(models/apb-watchdog/include)
link_directories(${CMAKE_SOURCE_DIR}/models/apb-watchdog.build)

add_subdirectory(models/systemtest models/systemtest.build)
include_directories(models/systemtest/include)
link_directories(${CMAKE_SOURCE_DIR}/models/systemtest.build)

### QBOX ###
if(MINGW)
    set(QBOX_CONFIGURE_COMMAND ${CMAKE_SOURCE_DIR}/libs/qbox/configure --target-list=arm-softmmu --library --qbox --with-tlm2c=${CMAKE_SOURCE_DIR}/libs/tlm2c.build --prefix=${CMAKE_SOURCE_DIR}/libs/qbox.build/install --disable-pie --extra-cflags=-ftls-model=initial-exec\ -static-libgcc\ -static-libstdc++)
else()
    set(QBOX_CONFIGURE_COMMAND ${CMAKE_SOURCE_DIR}/libs/qbox/configure --target-list=arm-softmmu --library --qbox --with-tlm2c=${CMAKE_SOURCE_DIR}/libs/tlm2c.build --prefix=${CMAKE_SOURCE_DIR}/libs/qbox.build/install --disable-pie --extra-cflags=-fPIC\ -fpic\ -ftls-model=initial-exec)
endif()
ExternalProject_Add(qbox SOURCE_DIR ${CMAKE_SOURCE_DIR}/libs/qbox
                         INSTALL_DIR ${CMAKE_SOURCE_DIR}/libs/qbox.build/install
                         BINARY_DIR ${CMAKE_SOURCE_DIR}/libs/qbox.build
                         TMP_DIR ${CMAKE_SOURCE_DIR}/libs/qbox.build/tmp
                         STAMP_DIR ${CMAKE_SOURCE_DIR}/libs/qbox.build/stampdir
                         CONFIGURE_COMMAND ${QBOX_CONFIGURE_COMMAND}
                         PREFIX ${CMAKE_SOURCE_DIR}/libs/qbox.build
                         BUILD_COMMAND $(MAKE))
ExternalProject_Add_Step(qbox force-build DEPENDEES update
                                          DEPENDERS build
                                          ALWAYS 1)
add_dependencies(qbox tlm2c)
link_directories(${CMAKE_SOURCE_DIR}/libs/qbox.build/arm-softmmu)
install(FILES ${CMAKE_SOURCE_DIR}/libs/qbox.build/arm-softmmu/libqbox-cortex-m3${CMAKE_SHARED_LIBRARY_SUFFIX} DESTINATION lib)

### Toplevel ###
add_executable(toplevel src/toplevel.cpp)
add_dependencies(toplevel
                 simplecpu
                 cce4510
                 apb_uart
                 qbox)

set(TOPLEVEL_LINK_LIBRARIES ${SystemC_LIBRARIES}
                            ${LUA_LIBRARIES}
                            simplecpu
                            apb_uart
                            cce4510
                            pl022
                            apbtimer
                            apbdualtimer
                            ahbgpio
                            apbwatchdog
                            devnull
                            tcpserial
                            systemtest
			    greenthreads
)
enable_testing()
add_test(write_reg_test_success toplevel --conf conf/cortex-m3-write_reg_test_success.lua)
SET_TESTS_PROPERTIES(write_reg_test_success PROPERTIES LABELS regression )
add_test(write_reg_test_fail toplevel --conf conf/cortex-m3-write_reg_test_fail.lua)
SET_TESTS_PROPERTIES(write_reg_test_fail PROPERTIES LABELS regression WILL_FAIL true )
 
if(MINGW)
    list(APPEND TOPLEVEL_LINK_LIBRARIES mman)
endif(MINGW)

target_link_libraries(toplevel ${TOPLEVEL_LINK_LIBRARIES})

install(TARGETS toplevel DESTINATION bin)
