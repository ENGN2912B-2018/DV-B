cmake_minimum_required(VERSION 2.8)
project(DV_B)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -p -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")

message(STATUS "Running on CCV")


find_package(VTK REQUIRED)

include(${VTK_USE_FILE})

add_executable(DumpXMLFile DumpXMLFile.cxx)
target_link_libraries(DumpXMLFile ${VTK_LIBRARIES})

add_executable(test_io test_load_data.cpp)
target_link_libraries(test_io read_data ${VTK_LIBRARIES})

add_executable(watch_data watch_data.cpp)
target_link_libraries(watch_data read_data ${VTK_LIBRARIES})

add_executable(watch_blade watch_blade.cpp)
target_link_libraries(watch_blade read_data ${VTK_LIBRARIES})

add_executable(blades blades.cpp)
target_link_libraries(blades read_data ${VTK_LIBRARIES})

add_executable(clip clip.cpp)
target_link_libraries(clip read_data ${VTK_LIBRARIES})

add_executable(watch_all watch_all.cpp)
target_link_libraries(watch_all read_data ${VTK_LIBRARIES})

add_executable(q_watch q_watch.cpp)
target_link_libraries(q_watch read_data ${VTK_LIBRARIES})

add_executable(get_points get_points.cpp)
target_link_libraries(get_points read_data ${VTK_LIBRARIES})

