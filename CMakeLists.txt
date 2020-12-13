cmake_minimum_required (VERSION 3.10.0)

project (TweedeEngineFrameworkRedux)

set (TE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/Source)
set (APP_ROOT_DIR ${PROJECT_SOURCE_DIR})
set (CXX_COMPILER_PATH ${CMAKE_CXX_COMPILER})

if (MSVC)
    get_filename_component(COMPILER_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)
    file(GLOB_RECURSE VCVARSALL_LIST_BAT "${COMPILER_DIR}/../../../../../../*.bat")

    foreach (VCVARSALL_ITEM ${VCVARSALL_LIST_BAT})
        if (VCVARSALL_ITEM MATCHES "^(.*)(vcvarsall\.bat)(.*)$")
            set (MSVC_VCVARS ${VCVARSALL_ITEM})
        endif ()
    endforeach ()
endif ()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    if (LINUX)
        set(CMAKE_INSTALL_PREFIX /usr/local/opt/tef CACHE PATH "..." FORCE)
    endif () 
endif ()

include (Source/CMake/Properties.cmake)
include (Source/CMake/HelperMethods.cmake)

add_subdirectory (Source)
