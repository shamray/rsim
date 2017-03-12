cmake_minimum_required(VERSION 3.0)
project(Rsim)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
add_definitions(-D_HAS_AUTO_PTR_ETC=1)

# Boost
set(BOOST_ROOT $ENV{BOOST_ROOT})
set(BOOST_LIBRARYDIR "stage64")

set(Boost_USE_STATIC_RUNTIME    OFF)
set(Boost_USE_STATIC_LIBS       OFF)
set(Boost_USE_MULTITHREADED     ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

add_definitions( -DBOOST_ALL_NO_LIB )

# Project sources
add_subdirectory(src)
