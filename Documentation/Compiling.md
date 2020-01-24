# Compiling

## Quick start

- Install git (https://git-scm.com) and CMake 3.9.0 or higher (https://cmake.org)
  - Ensure they are added to your *PATH* environment variable
- Install other dependencies
  - See [here](#otherDeps)
- Run the following commands in the terminal/command line:
  - `git clone https://github.com/fabsgc/TweedeFrameworkRedux.git`
  - `cd TweedeFrameworkRedux`
  - `mkdir Build`
  - `cd Build`
  - `cmake -G "$generator$" ..` 
    - Where *$generator$* should be replaced with any of the supported generators. Some common ones:
    - `Visual Studio 16 2019` - Visual Studio 2019 (64-bit build)
    - `Visual Studio 15 2017 Win64` - Visual Studio 2017 (64-bit build)
    - `Visual Studio 16 2019` - Visual Studio 2019
    - `Unix Makefiles`
    - `Ninja`
    - See all valid generators: [cmake-generators](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html)
  - Optionally provide the `CMAKE_INSTALL_PREFIX` to override the default install path:
    - `cmake -G "$generator$" -DCMAKE_INSTALL_PREFIX=/path/to/install ..`
  - `cmake --build . --config Release`
    - Alternatively you can also run your build system manually (depending on your choice of generator):
  - `cmake --build . --config Release --target install`
  - Alternatively you can run the install target in your chosen build tool
    - Note that files install to the default install folder, unless you have overriden it as specified above
   
## Customizing the build

Additional variables allow you to pick between the render API (DirectX, OpenGL). Run *CMake* to see all options. Note that non-default *CMake* options might require additional dependencies to be installed, see [here](#otherDeps).

Modify *CMAKE_INSTALL_PREFIX* to choose where the library gets installed after the *install* target is ran (e.g. `make install`, or running the *INSTALL* target in Visual Studio/XCode).

Modify *CMAKE_BUILD_TYPE* to pick what kind of a build you want. Note that this is ignored for workflow generators like Visual Studio or XCode, in which you can pick this within the IDE itself. Supported values are:
 - *Debug* - Builds and unoptimized version with debug symbols. This is generally what you want for development.
 - *RelWithDebInfo* - Builds an optimized version with debug symbols and various debug checks enabled.
 - *MinSizeRel* - Builds an optimized version attempting to keep the resultant binary as small as possible. Debug checks are disabled.
 - *Release* - Builds an optimized version with no debug checks.
 
You can choose to use a different *CMake* generator than those specified above, as long as the platform/compiler is supported:  
  - Supported platforms:
    - Windows 7, 8, 10
    - Linux
  - Supported compilers:
    - MSVC++ 15.0 (Visual Studio 2017) (or newer)
    - GCC 7.0 (or newer)
    - Clang 5.0 (or newer)
  
### <a name="dependencies"></a>Third party dependencies
The Tweede Framework relies on a variety of third party dependencies. A set of pre-compiled dependencies are provided for every supported platform/compiler and these will be fetched automatically by the build process. If required, the dependencies can also be compiled manually by following [this guide](dependencies.md). This can be required if the pre-compiled dependencies don't work with your platform (e.g. unsupported Linux distro) or if you wish to update to a newer dependency version. Note that prebuilt dependencies only come as 64-bit.

## <a name="otherDeps"></a>Other dependencies
The following dependencies will need to be installed manually. Which ones are required depend on the selected *CMake* options and your usage of bs::framework (check text of each entry below).

**Windows**
  - **DirectX SDK** (Required by default on Windows 7 or earlier)
  - Optional if you have choosen a different RenderAPI in *CMake* options
    - Set up DXSDK_DIR environment variable pointing to the DirectX instalation
  - **Windows SDK** (Required by default on Windows 8 or later)
  - Optional if you have choosen a different RenderAPI in *CMake* options
  - **DirectX Debug Layer** (Required by default on Windows 10)
    - Optional if you have choosen a different RenderAPI in *CMake* options
    - Go to Settings panel (type "Settings" in Start)->System->Apps & features->Manage optional Features->Add a feature->Select "Graphics Tools"
 
**Linux**
  - **OpenGL**
    - Required by default, but optional if you have chosen a different RenderAPI in *CMake* options
    - Debian/Ubuntu: `apt-get install libgl1-mesa-dev libglu1-mesa-dev mesa-common-dev`
  - **X11**
    - Debian/Ubuntu: `apt-get install libx11-dev libxcursor-dev libxrandr-dev libxi-dev`
  - **LibUUID**
    - Debian/Ubuntu: `apt-get install uuid-dev`
  - **LibICU**
    - Debian/Ubuntu: `apt-get install libicu-dev`
  - (Or equivalent packages for your distribution)

**All OS**
  - **Assimp**
  - **Free Image**
