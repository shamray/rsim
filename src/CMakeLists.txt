# External dependencies
find_package(Boost COMPONENTS date_time)

# Target definition
add_executable(rsim main.cpp utils/random.cpp distribution/birth.cpp)

# Compile-time dependencies
include_directories(${Rsim_SOURCE_DIR}/include)
include_directories(${Boost_INCLUDE_DIRS})

# Link-time dependencies
target_link_libraries(rsim ${Boost_LIBRARIES})

