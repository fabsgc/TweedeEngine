# Find nvtt dependency
#
# This module defines
#  nvtt_INCLUDE_DIRS
#  nvtt_LIBRARIES
#  nvtt_FOUND

start_find_package(nvtt)

# Use bundled library, as there is no common packaging for nvtt
set (nvtt_INSTALL_DIR ${TE_SOURCE_DIR}/../Dependencies/nvtt CACHE PATH "")
gen_default_lib_search_dirs(nvtt)

find_imported_includes(nvtt nvtt.h)
find_imported_library(nvtt nvtt)

end_find_package(nvtt nvtt)
