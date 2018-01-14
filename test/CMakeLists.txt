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

# Project name
set(PROJECT_NAME "unit_tests")

# Source files
set(SOURCE "test.cpp")

# Enable unity build
enable_unity_build(${PROJECT_NAME} SOURCE)

# The executable
add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCE})

# Set properties
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD_REQUIRED 17)

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC "../lib/acutest/include")

# Enable unit testing
enable_testing()

# Add test
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

# Compile & link options for the project
if(MSVC)
    # TODO: I know next to nothing about MSVC at the moment! Maybe one day I'll come back to this...
else()
    target_compile_options(${PROJECT_NAME} PUBLIC
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
        "-Wdouble-promotion"
        "-Wno-missing-braces")
    link_libraries(
        "-Wl,-z,relro,-z,now"
        "-fstack-protector-strong"
        "-Wstack-protector"
        "-feliminate-unused-debug-types")
endif()

# Add test command
add_custom_command(TARGET ${PROJECT_NAME} COMMENT "Run tests" POST_BUILD COMMAND ${PROJECT_NAME})

##----------------------------------------------------------------------------------------------------------------------
