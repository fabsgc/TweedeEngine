# Source files and their filters
include(CMakeSources.cmake)

message("-- TeFontImporter Initialization...")

# Target
add_library(TeFontImporter SHARED ${TE_FONTIMPORTER_SRC})

# Includes
target_include_directories(TeFontImporter PRIVATE "./")

# Defines
target_compile_definitions(TeFontImporter PRIVATE -DTE_FONT_EXPORTS -DTE_ENGINE_BUILD -DUSE_FREETYPE2_STATIC)

# Libraries
## External lib: FreeType
target_link_libraries(TeFontImporter ${freetype_LIBRARIES})

## Local libs
target_link_libraries(TeFontImporter tef)

# IDE specific
set_property(TARGET TeFontImporter PROPERTY FOLDER Plugins)

# Install
install_tef_target(TeFontImporter)

message("-- ...TeFontImporter OK.")
