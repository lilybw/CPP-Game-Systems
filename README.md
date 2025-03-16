# Build
Check "deps" directory, for each dependency, a keepme file specifies where to find it and version

Run the commands located in root
config.bat, then build.bat (or their contents if not on windows)

The executable is located in dist as "app.exe"

# Sources
Sources are automatically loaded on running ./config, however not automatically updated.
I.e. to include any new source file, ./config has to be rerun.

# Testing
This project uses GoogleTest see: https://github.com/google/googletest?tab=readme-ov-file

The test executable is build automatically when the project is and can be found in "dist/sdlgame_test.exe"


# Structure
Project structure for reference

 - deps // location of all dependencies not automatically fetched
 - - SDL3-3.2.0
 - dist // output location
 - src
 - - collisions
 - - - collider.h
 - - - resolver.h
 - - entities
 - - - components.h
 - - - entity.h
 - - input
 - - - input.cpp
 - - - input.h
 - - meta
 - - - ApplicationContext.cpp
 - - - ApplicationContext.h
 - - player
 - - scene
 - - - MenuScreen.cpp
 - - - scene.h
 - - types // utility structures and the like
 - - - types.h
 - - main.cpp
 - test
 - - entities
 - - - entity_test.cpp
 - - meta
 - main_test.cpp
