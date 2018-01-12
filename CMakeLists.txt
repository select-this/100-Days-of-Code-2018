##----------------------------------------------------------------------------------------------------------------------
##
## Copyright (c) 2018 Darren Paul Ferrie
##
## Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
## documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
## rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
## permit persons to whom the Software is furnished to do so, subject to the following conditions:
##
## The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
## Software.
##
## THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
## WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
## COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
## OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
##
##----------------------------------------------------------------------------------------------------------------------

# Set CMake minimum version
cmake_minimum_required(VERSION 3.10)

# Use CCache if it's available
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif()

# Set the configuration types
set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE TYPE INTERNAL FORCE)

if(MSVC)
    # TODO: I know next to nothing about MSVC at the moment! Maybe one day I'll come back to this...
else()
    add_compile_options(
        "-MMD"
        "-MP"
        "-g"
        "-Wall"
        "-Wextra"
        "-Werror"
        "-Wuninitialized"
        "-Wformat"
        "-Wformat-security"
        "-Werror=format-security"
        "-pthread"
        "-fstack-protector-strong"
        "-Wstack-protector"
        "-Wdate-time"
        "-feliminate-unused-debug-types"
        "-Wuseless-cast"
        "-Wdouble-promotion"
        "-Wno-missing-braces")
    link_libraries(
        "-Wl,-z,relro,-z,now"
        "-fstack-protector-strong"
        "-Wstack-protector"
        "-feliminate-unused-debug-types")
endif()

# Set a default build type if one isn't specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: Debug Release" FORCE)
endif(NOT CMAKE_BUILD_TYPE)

# Configuration flag to enable Unity builds
option(UNITY_BUILDS "Whether to build using unity builds" OFF)

# Function to enable Unity builds
function(enable_unity_build UNITY_BUILD_SUFFIX SOURCE_VARIABLE_NAME)
    if(UNITY_BUILDS)
        message(STATUS "Enabling unity build for ${UNITY_BUILD_SUFFIX}")
        set(UNITY_FILES ${${SOURCE_VARIABLE_NAME}})
        # Generate a unique filename for the unity build translation unit
        set(UNITY_BUILD_FILE ${CMAKE_CURRENT_BINARY_DIR}/unity_${UNITY_BUILD_SUFFIX}.cpp)
        # Exclude all translation units from compilation
        set_source_files_properties(${UNITY_FILES} PROPERTIES HEADER_FILE_ONLY true)
        # Open the ub file
        FILE(WRITE ${UNITY_BUILD_FILE} "// Unity Build\n")
        # Add include statement for each translation unit
        foreach(UNITY_SOURCE_FILE ${UNITY_FILES})
            FILE(APPEND ${UNITY_BUILD_FILE} "#include <${CMAKE_CURRENT_SOURCE_DIR}/${UNITY_SOURCE_FILE}>\n")
        endforeach(UNITY_SOURCE_FILE)
        # Complement list of translation units with the name of ub
        set(${SOURCE_VARIABLE_NAME} ${${SOURCE_VARIABLE_NAME}} ${UNITY_BUILD_FILE} PARENT_SCOPE)
    endif()
endfunction(enable_unity_build)

# Function to add FILE_PATH definition
function(define_file_source_paths PROJECT_SOURCE_FILE_TARGET)
    set(PATH_EXC_TRAIL "${CMAKE_SOURCE_DIR}")
    set(PATH_INC_TRAIL "${CMAKE_SOURCE_DIR}/")
    set(PATH_REPLACE_CH " ")
    get_target_property(PROJECT_SOURCE_FILES "${PROJECT_SOURCE_FILE_TARGET}" SOURCES)
    foreach(PROJECT_SOURCE_FILE ${PROJECT_SOURCE_FILES})
        get_property(PROJECT_SOURCE_FILE_DEFINITIONS SOURCE "${PROJECT_SOURCE_FILE}" PROPERTY COMPILE_DEFINITIONS)
        get_filename_component(PROJECT_SOURCE_FILE_NAME "${PROJECT_SOURCE_FILE}" REALPATH)
        string(REPLACE ${PATH_INC_TRAIL} ${PATH_REPLACE_CH} PROJECT_SOURCE_FILE_NAME ${PROJECT_SOURCE_FILE_NAME})
        string(REPLACE ${PATH_EXC_TRAIL} ${PATH_REPLACE_CH} PROJECT_SOURCE_FILE_NAME ${PROJECT_SOURCE_FILE_NAME})
        string(STRIP ${PROJECT_SOURCE_FILE_NAME} PROJECT_SOURCE_FILE_NAME)
        list(APPEND PROJECT_SOURCE_FILE_DEFINITIONS "FILE_PATH=\"${PROJECT_SOURCE_FILE_NAME}\"")
        set_property(SOURCE "${PROJECT_SOURCE_FILE}" PROPERTY COMPILE_DEFINITIONS ${PROJECT_SOURCE_FILE_DEFINITIONS})
    endforeach()
endfunction()

# Project name
set(PROJECT_NAME "100-Days-of-Code-2018")

# Set project
project(${PROJECT_NAME})

# Headers for this project
#set(HEADERS "include/tmp.hpp")

# Source files
set(SOURCE "src/main.cpp")

# Enable unity build
enable_unity_build(${PROJECT_NAME} SOURCE)

# The executable
add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCE})

# Set properties
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD_REQUIRED 17)

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC "include")

# Add the CMake module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/")

# Find dependencies
find_package(SDL2_ttf REQUIRED)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

# Add dependencies
target_link_libraries(${PROJECT_NAME} ${SDL2TTF_LIBRARY} ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES})

# Add file source path defines
define_file_source_paths(${PROJECT_NAME})

# Add subdirectores
add_subdirectory(test)

##----------------------------------------------------------------------------------------------------------------------
