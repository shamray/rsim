set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
set(BOOST_ROOT "C:\\libdev4_x64\\boost")
set(BOOST_LIBRARYDIR "stage64")

find_package(Boost COMPONENTS date_time)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(rsim main.cpp)

target_link_libraries(rsim ${Boost_LIBRARIES})
message(${Boost_LIBRARIES})