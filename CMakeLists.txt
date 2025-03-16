cmake_minimum_required(VERSION 3.20)
include(FetchContent)
cmake_policy(SET CMP0135 NEW) # To make warning go away
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(cpp-game-systems)

FetchContent_Declare( # GLM Math Lib
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	bf71a834948186f4097caa076cd2663c69a10e1e #refs/tags/1.0.1
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare( # Google Test
  googletest
  URL https://github.com/google/googletest/archive/7927f8e93d2cc7788229ab446179fdfda15e75e4.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Locate SDL3 using the provided SDL3Config.cmake
set(SDL3_DIR "${CMAKE_SOURCE_DIR}/deps/SDL3-3.2.0/cmake")
find_package(SDL3 REQUIRED)

# Locate SDL3 using the provided SDL3Config.cmake
set(SDL3_image_DIR "${CMAKE_SOURCE_DIR}/deps/SDL3_image-3.2.0/cmake")
find_package(SDL3_image REQUIRED)

# Recursively find all .cpp
file(GLOB_RECURSE SOURCES "src/*.cpp")
list(FILTER SOURCES EXCLUDE REGEX ".*/main.cpp$")  # Exclude main.cpp

add_library(sdlgame_lib STATIC ${SOURCES})
# Exposes import paths as seen in /src in /test
target_include_directories(sdlgame_lib PUBLIC ${CMAKE_SOURCE_DIR}/src)
# Link libraries to the game library
target_link_libraries(
  sdlgame_lib 
    PRIVATE 
      SDL3::SDL3 
      glm::glm 
      SDL3_image::SDL3_image
)

# Create the executable and add all sources
add_executable(sdlgame src/main.cpp)
target_link_libraries(
  sdlgame 
    PRIVATE 
      sdlgame_lib 
      SDL3::SDL3 
      glm::glm 
      SDL3_image::SDL3_image
)

# Add static linking option (if needed)
target_link_options(sdlgame PRIVATE -static)

# Include the src directory and SDL3 headers
target_include_directories(sdlgame PRIVATE ${CMAKE_SOURCE_DIR}/src ${SDL3_INCLUDE_DIRS})

# Copy SDL3.dll to the output directory
add_custom_command(TARGET sdlgame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/deps/SDL3-3.2.0/x86_64-w64-mingw32/bin/SDL3.dll"
    $<TARGET_FILE_DIR:sdlgame>
)

# TESTING

enable_testing()

# Recursively find all test files
file(GLOB_RECURSE TEST_SOURCES "test/*.cpp")

add_executable(
  sdlgame_test ${TEST_SOURCES}
)

target_link_libraries(
  sdlgame_test
  GTest::gtest_main
  sdlgame_lib
  glm::glm
  SDL3::SDL3
)

include(GoogleTest)
gtest_discover_tests(sdlgame_test)
