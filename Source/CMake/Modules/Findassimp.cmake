# Find Assimp dependency
#
# This module defines
#  assimp_INCLUDE_DIRS
#  assimp_LIBRARIES
#  assimp_FOUND

start_find_package (assimp)

set (assimp_INSTALL_DIR ${TE_SOURCE_DIR}/../Dependencies/assimp CACHE PATH "")
gen_default_lib_search_dirs (assimp)

find_imported_includes (assimp assimp/Importer.hpp)
find_imported_library_shared (assimp assimp)

install_dependency_binaries (assimp)

end_find_package (assimp assimp)