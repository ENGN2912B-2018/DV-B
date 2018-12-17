cmake_minimum_required(VERSION 2.8)
project(DV_B)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -p -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")

message(STATUS "Running on CCV")


find_package(VTK REQUIRED)

include(${VTK_USE_FILE})

add_executable(ConvertFile_vtp ConvertFile_vtp.cxx)
target_link_libraries(ConvertFile_vtp ${VTK_LIBRARIES})

add_executable(ConvertFile_vtu ConvertFile_vtu.cxx)
target_link_libraries(ConvertFile_vtu ${VTK_LIBRARIES})

