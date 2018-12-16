
cmake_minimum_required(VERSION 2.8)

PROJECT(RenderWindowNoUiFile)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_library(data_load data_load.h data_load.cpp)
target_link_libraries(data_load ${VTK_LIBRARIES})

add_executable(RenderWindowNoUiFile MACOSX_BUNDLE RenderWindowNoUiFile.cxx )

target_link_libraries(RenderWindowNoUiFile ${VTK_LIBRARIES} data_load)
