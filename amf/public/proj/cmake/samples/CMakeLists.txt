cmake_minimum_required(VERSION 2.8) 
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_SKIP_RULE_DEPENDENCY TRUE)

enable_language(CXX)

if(NOT AMF_NAME)
  set_property(GLOBAL PROPERTY AMF_NAME amf)
  get_property(AMF_NAME GLOBAL PROPERTY AMF_NAME)

  message(WARNING "Open AMF will be built with the default library name: " ${AMF_NAME})
endif()

if(OpenCL_INCLUDE_DIR)
  get_filename_component(Path_To_OpenCL ${OpenCL_INCLUDE_DIR} ABSOLUTE)
  message(WARNING "OpenCL location was set manually")
  include_directories(${Path_To_OpenCL})
  set_property(GLOBAL PROPERTY OpenCL_LIBRARIES ${Path_To_OpenCL}/lib/x86_64/opencl.lib)
  get_property(OpenCL_LIBRARIES GLOBAL PROPERTY OpenCL_LIBRARIES)
  message("OpenCL_INCLUDE_DIRS: " ${Path_To_OpenCL})
  message("OpenCL_LIBRARIES: " ${OpenCL_LIBRARIES})
else()
  find_package(OpenCL REQUIRED)
  
  if(NOT OpenCL_FOUND)
    message("")
    message(SEND_ERROR "Error: Failed to find OpenCL!")
    return()
  else()
    message("OpenCL found: " ${OpenCL_FOUND})
    message("OpenCL_INCLUDE_DIRS: " ${OpenCL_INCLUDE_DIRS})
    message("OpenCL_LIBRARIES: " ${OpenCL_LIBRARIES})
    message("OpenCL_VERSION_STRING: " ${OpenCL_VERSION_STRING})
    message("OpenCL_VERSION_MAJOR: " ${OpenCL_VERSION_MAJOR})
    message("OpenCL_VERSION_MINOR: " ${OpenCL_VERSION_MINOR})

    message("OpenCL_INCLUDE_DIR: ${OpenCL_INCLUDE_DIR}")
    message("OpenCL_LIBRARY: ${OpenCL_LIBRARY}")

    include_directories(${OpenCL_INCLUDE_DIR})
    #ADD_DEFINITIONS(-DCL_TARGET_OPENCL_VERSION=120)
  endif()  
endif()

project(open_amf_test1)	

set(SOURCE_EXE ../../Qt/AMF_test/main.cpp)

include_directories(..)	
add_subdirectory(.. ${AMF_NAME})

add_executable(test1 ${SOURCE_EXE})

if(AMF_CORE_STATIC)
  target_link_libraries(test1 ${AMF_NAME} ${OpenCL_LIBRARIES})
else()
  #target_link_libraries(test1 ${AMF_NAME}) 
endif()