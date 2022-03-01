# Find nvtt dependency
#
# This module defines
#  nvtt_INCLUDE_DIRS
#  nvtt_LIBRARIES
#  nvtt_FOUND

start_find_package(nvtt)

# Use bundled library, as there is no common packaging for nvtt
set (nvtt_INSTALL_DIR ${APP_ROOT_DIR}/Dependencies/${PLATFORM_OS}.${PLATFORM_COMPILER}/nvtt CACHE PATH "")
gen_default_lib_search_dirs(nvtt) 

find_imported_includes(nvtt nvtt.h)
find_imported_library(nvtt nvtt)
find_imported_library(nvtt nvimage)
find_imported_library(nvtt bc6h)
find_imported_library(nvtt bc7)
find_imported_library(nvtt nvcore)
find_imported_library(nvtt nvmath)
find_imported_library(nvtt nvthread)
find_imported_library(nvtt squish)

end_find_package(nvtt nvtt)
end_find_package(nvtt nvimage)
end_find_package(nvtt bc6h)
end_find_package(nvtt bc7)
end_find_package(nvtt nvcore)
end_find_package(nvtt nvmath)
end_find_package(nvtt nvthread)
end_find_package(nvtt squish)
