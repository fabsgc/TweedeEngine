cmake_minimum_required (VERSION 3.10.0)

project (TweedeEngineFrameworkRedux)

set (TE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/Source)
set (APP_ROOT_DIR ${PROJECT_SOURCE_DIR})
set (CXX_COMPILER_PATH ${CMAKE_CXX_COMPILER})

if (MSVC)
    get_filename_component(MY_COMPILER_DIR
        ${CMAKE_CXX_COMPILER} DIRECTORY)
    find_file(MY_VCVARSALL_BAT vcvarsall.bat
        "${MY_COMPILER_DIR}/.." "${MY_COMPILER_DIR}/../..")

        message (STATUS ${MY_VCVARSALL_BAT})

    set (MSVC_VCVARS ${MY_VCVARSALL_BAT})
endif ()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    if (LINUX)
        set(CMAKE_INSTALL_PREFIX /usr/local/opt/tef CACHE PATH "..." FORCE)
    endif () 
endif ()

include (Source/CMake/Properties.cmake)
include (Source/CMake/HelperMethods.cmake)

add_subdirectory (Source)
