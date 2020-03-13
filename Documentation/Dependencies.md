# Compiling dependencies manually

Below you will find a list of dependencies that the framework relies on, as well as links to their source code and/or binaries. If a dependency isn't provided in binary form you will have to manually compile it (this is the case for the large majority or them). Make sure to compile the exact version of the dependency listed below. Newer versions *might* work, but haven't been tested. 

Once you have the dependency development files (headers and binaries) you will need to make sure they placed in the appropriate folders so the framework can find them during the build process. 

The dependencies are searched for in these locations:
- The `/Dependency` folder within source. See below for the exact breakdown of how this folder is supposed to look. Usually you want to put all your dependencies here.
- If dependency cannot be found in the `/Dependency` folder, its default install path is searched for instead. For example `usr/local` on Linux or default install path if the dependency comes with an installer. 

Note that on Windows most dependencies do not have default install paths and should therefore be placed in the `/Dependency` folder. In order to avoid problems with dependency versions this should be the preferred behaviour on Linux as well. 

`/Dependency` folder breakdown:
- Static & shared libraries (.lib, .a, .so): 
  - Pick one of:
    - (TeSource)/Dependencies/(DepName)/lib
    - (TeSource)/Dependencies/(DepName)/lib/(Platform)
    - (TeSource)/Dependencies/(DepName)/lib/(Platform)/(Configuration)
    - (TeSource)/Dependencies/(DepName)/lib/(Configuration)
- Dynamic libraries (.dll)
  - Place in (TeSource)/bin/(Platform)/(Configuration)
- Includes
  - Place in (TeSource)/Dependencies/(DepName)/include  

Legend:
- (TeSource) - root directory of the framework
- (DepName) - name of the dependency (title of each dependency shown below)
- (Platform) - x64 for 64-bit builds
- (Configuration) - Debug, RelWithDebInfo, MinSizeRel, or Release 

Each library is accompanied by a Find***.cmake CMake module that is responsible for finding the library. These modules are located under `Source/CMake/Modules`. They follow the rules described above, but if you are unsure where library outputs should be placed you can look at the source code for those modules to find their search paths.

Additionally, if the dependency structure still isn't clear, download one of the pre-compiled dependency packages to see an example.  

## Library list 

**freeimg**
- FreeImage 3.17
- http://freeimage.sourceforge.net
- Required by TeFreeImgImporter

**Assimp**
- assimp 3.1.1
- http://www.assimp.org/
- Required by TeObjectImpoter

**NVIDIA Texture Tools**
- NVIDIA Texture Tools 2.0
- https://github.com/BearishSun/nvidia-texture-tools
- Required by tef

**LibICU**
- http://site.icu-project.org/
- Only required for Linux builds
- Required by bsfCore
- Compile as a static library
 - Make sure to provide `-fPIC` as an explicit flag
 - You *can* use system version of the ICU library, but your binaries will then only be compatible with Linux distros using the exact same API version (which changes often). It's safest to link ICU statically.

**freetype**
- Freetype 2.3.5
- https://github.com/BearishSun/freetype (branch *banshee*)
- Required by bsfFontImporter
- Compile as a static library

**OpenAL**
- OpenAL Soft 1.17.2
- https://github.com/kcat/openal-soft
- Required by bsfOpenAudio
- **Linux only**
  - Make sure to get audio backend libraries before compiling: PulseAudio, OSS, ALSA and JACK
  - On Debian/Ubuntu run: `apt-get install libpulse libasound2-dev libjack-dev`
- Compile as a dynamic library on Windows/Linux (default), static library on macOS
  - Use `LIBTYPE=STATIC` CMake flag to force it to compile as a static library

**libogg**
- libogg v1.3.2
- https://xiph.org/downloads/
- Required by bsfOpenAudio and bsfFMOD
- Compile as a static library
  - Switch runtime library to dynamic to avoid linker warnings when adding it to bs::f
  - This is also required when compiling libvorbis and libflac (below). See readme files included with those libraries.
  
**libvorbis**
- libvorbis commit:8a8f8589e19c5016f6548d877a8fda231fce4f93
- https://git.xiph.org/?p=vorbis.git
- Required by bsfOpenAudio and bsfFMOD
- Compile as a dynamic library on Windows, static library on Linux/macOS (default)
  - Requires libogg, as described in its readme file.
  - When compiling as static library on Linux, make sure to specify `-DCMAKE_POSITION_INDEPENDENT_CODE=ON` to CMake, otherwise it will fail to link
   
**libFLAC**
- libflac commit: f7cd466c24fb5d1966943f3ea36a1f4a37858597
- https://git.xiph.org/?p=flac.git
- Required by bsfOpenAudio
- Compile as a dynamic library on Windows (default), static library on Linux/macOS
  - Provide `--disable-shared --enable-static` flags to `configure` to force it to compile as a static library
  - Requires libogg, as described in its readme file.