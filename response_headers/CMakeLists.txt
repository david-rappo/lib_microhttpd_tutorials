cmake_minimum_required(VERSION 3.13.4)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

# x64
project(response_headers)

# SYSTEM - instructs the compiler to not generate warnings for these header files.
include_directories(SYSTEM $ENV{HOME}/code/repo/other/GSL/include)

# Required when building on Linux for PVS-Studio in cmake-gui
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Inside build directory
# -DCMAKE_EXPORT_COMPILE_COMMANDS=On : Required for PVS-Studio
# Use Clang
# cmake -G "Unix Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=On -DCMAKE_C_COMPILER=clang-3.8 -DCMAKE_CXX_COMPILER=clang++-3.8 -DCMAKE_BUILD_TYPE=Debug path/to/source
# Use GCC
# cmake -G "Unix Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=On -DCMAKE_BUILD_TYPE=Debug path/to/source

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(proj_srcs file_utility.h file_utility.cpp
              main.cpp)

add_executable(${PROJECT_NAME} ${proj_srcs})

target_link_libraries(${PROJECT_NAME} microhttpd)

include($ENV{HOME}/code/repo/other/pvs-studio-cmake-examples/PVS-Studio.cmake)

# ALL: Perform the project analysis using the Build All command.
# COMPILE_COMMANDS: Use compile_commands.json. Setting CMAKE_EXPORT_COMPILE_COMMANDS to On creates compile_commands.json.
# ARGS: Additional pvs-studio-analyzer/CompilerCommandsAnalyzer.exe flags
# -e: Ignore files in the specified directory.
# See PVS-Studio.cmake for further information about the pvs_studio_add_target command.
pvs_studio_add_target(TARGET "pvs_${PROJECT_NAME}" ALL COMPILE_COMMANDS OUTPUT FORMAT errorfile ARGS
	-e $ENV{HOME}/code/repo/other/GSL
	-e $ENV{HOME}/code/repo/other/googletest)