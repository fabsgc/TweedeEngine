# Find freeimg dependency
#
# This module defines
#  freeimg_INCLUDE_DIRS
#  freeimg_LIBRARIES
#  freeimg_FOUND

start_find_package (freeimg)

set (freeimg_INSTALL_DIR ${APP_ROOT_DIR}/Dependencies/freeimg CACHE PATH "")
gen_default_lib_search_dirs (freeimg)

find_imported_includes (freeimg FreeImage.h)
find_imported_library_shared (freeimg freeimage)

install_dependency_binaries (freeimg)

end_find_package (freeimg freeimage)