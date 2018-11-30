cmake_minimum_required(VERSION 2.8)
project(DV_B)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -p -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")

message(STATUS "Running on CCV")


find_package(VTK REQUIRED)

include(${VTK_USE_FILE})


add_executable(test_io test_load_data.cpp)
target_link_libraries(test_io read_data ${VTK_LIBRARIES})

add_executable(watch_data watch_data.cpp)
target_link_libraries(watch_data read_data ${VTK_LIBRARIES})
