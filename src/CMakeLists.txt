set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
add_definitions( -DBOOST_ALL_NO_LIB )
set(BOOST_ROOT "C:\\libdev\\boost")
set(BOOST_LIBRARYDIR "stage64")

find_package(Boost COMPONENTS date_time)
set(Boost_USE_STATIC_LIBS OFF)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(rsim main.cpp)

target_link_libraries(rsim ${Boost_LIBRARIES})
message(${Boost_LIBRARIES})