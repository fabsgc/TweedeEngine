# Find Assimp dependency
#
# This module defines
#  assimp_INCLUDE_DIRS
#  assimp_LIBRARIES
#  assimp_FOUND

start_find_package (assimp)

set (assimp_INSTALL_DIR ${APP_ROOT_DIR}/Dependencies/${PLATFORM_OS}.${PLATFORM_COMPILER}/assimp CACHE PATH "")
gen_default_lib_search_dirs (assimp)

find_imported_includes (assimp assimp/Importer.hpp)


if(WIN32)
    set(assimp_LIBNAME assimp-vc142-mt)
else()
    set(assimp_LIBNAME assimp)
endif()

if(WIN32)
    find_imported_library_shared (assimp assimp-vc142-mt)
else()
    find_imported_library_shared (assimp assimp)
endif()

install_dependency_binaries(assimp)

if(WIN32)
    end_find_package (assimp assimp-vc142-mt)
else()
    end_find_package (assimp assimp)
endif()
