cmake_minimum_required(VERSION 2.8)
project(vtk_io)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -p -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")

message(STATUS "Running on CCV")

find_package(VTK REQUIRED)
if (vtkIOExport_LOADED AND vtkIOImport_LOADED)
vtk_module_config(VTK
  vtkCommonCore
  vtkIOLegacy
  vtkIOXML
  vtkIOGeometry
  vtkInteractionStyle
  vtkIOImport
  vtkIOExport
  vtksys
)
else()
vtk_module_config(VTK
  vtkCommonCore
  vtkIOLegacy
  vtkIOXML
  vtkIOGeometry
  vtkInteractionStyle
  vtkIOImport
  vtksys
)
endif()
include(${VTK_USE_FILE})

##add_executable(data_load data_load.h data_load.cpp)
add_library(vtk_io data_load.h data_load.cpp)
set_target_properties(vtk_io PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(vtk_io ${VTK_LIBRARIES})
