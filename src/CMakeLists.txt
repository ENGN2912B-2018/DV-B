cmake_minimum_required(VERSION 2.8)


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

include_directories(vtk_io)
add_subdirectory(vtk_io)
