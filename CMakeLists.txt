cmake_minimum_required(VERSION 3.16)
project(imnes CXX)
include(cmake/PreventInSourceBuilds.cmake)

# We set project options and warnings as INTERFACE libraries
# Libraries that have no generated object files
# This lets us choose whether to include the options/warnings on a per-target basis
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)


set(WARNINGS_AS_ERRORS FALSE)

add_library(project_warnings INTERFACE)
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# enable doxygen
include(cmake/Doxygen.cmake)
enable_doxygen()

# allow for static analysis options
include(cmake/StaticAnalyzers.cmake)

# Todo. Add this testing
option(ENABLE_TESTING "Enable Test Builds" OFF)
if(ENABLE_TESTING)
    message("Building Tests")
    add_subdirectory(test)
endif()


add_subdirectory(src)