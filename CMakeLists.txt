cmake_minimum_required(VERSION 3.16.2)

project(cpp 
        VERSION 1.0.0.0
        DESCRIPTION "My C++ Project"
        HOMEPAGE_URL http://123.com
        LANGUAGES CXX)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/flags.cmake)

option(CPP_WITH_TESTS "Compile and (for jsoncpp_check) run JsonCpp test executables" ON)
# option(CPP_WITH_POST_BUILD_UNITTEST "Automatically run unit-tests as a post build step" ON)
# option(CPP_WITH_WARNING_AS_ERROR "Force compilation to fail if a warning occurs" OFF)
# option(CPP_WITH_STRICT_ISO "Issue all the warnings demanded by strict ISO C and ISO C++" ON)
# option(CPP_WITH_PKGCONFIG_SUPPORT "Generate and install .pc files" ON)
# option(CPP_WITH_CMAKE_PACKAGE "Generate and install cmake package files" ON)
# option(CPP_WITH_EXAMPLE "Compile JsonCpp example" OFF)
# option(CPP_STATIC_WINDOWS_RUNTIME "Use static (MT/MTd) Windows runtime" OFF)
# option(BUILD_SHARED_LIBS "Build jsoncpp_lib as a shared library." ON)
# option(BUILD_STATIC_LIBS "Build jsoncpp_lib as a static library." ON)
# option(BUILD_OBJECT_LIBS "Build jsoncpp_lib as a object library." ON)

add_subdirectory(app)

#install the example
if(CPP_WITH_TESTS)
    add_subdirectory(test)
endif()