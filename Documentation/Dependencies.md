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
- (Platform) - x86 for 32-bit builds, x64 for 64-bit builds
- (Configuration) - Debug, RelWithDebInfo, MinSizeRel, or Release 

Each library is accompanied by a Find***.cmake CMake module that is responsible for finding the library. These modules are located under `Source/CMake/Modules`. They follow the rules described above, but if you are unsure where library outputs should be placed you can look at the source code for those modules to find their search paths.

Additionally, if the dependency structure still isn't clear, download one of the pre-compiled dependency packages to see an example.  

## Library list 

**freeimg**
- FreeImage 3.17
- http://freeimage.sourceforge.net
- Required by TeFreeImgImporter
- Compile as a static library

**Assimp**
- assimp 3.1.1
- http://www.assimp.org/
- Required by TeObjectImpoter