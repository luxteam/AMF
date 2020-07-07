cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_SKIP_RULE_DEPENDENCY TRUE)

enable_language(CXX)

project(open_amf_test1)
set(AMF_NAME amf)
ADD_DEFINITIONS(-DAMF_CORE_STATIC)
set(OpenCL_INCLUDE_DIR "../../../../../../CL_Headers")
ADD_DEFINITIONS(-DCL_TARGET_OPENCL_VERSION=120)
# Local header files here ONLY
SET(HEADERS

   )

# Local source files here
SET(SOURCES
    main_metal.cpp
    )

#include_directories(..)
add_subdirectory(../cmake ${AMF_NAME})

#add_executable(${PROJECT_NAME} ${SOURCES})
add_custom_command(OUTPUT default.air
    COMMAND xcrun -sdk macosx metal -c ../process.metal -o default.air
)
add_custom_command(OUTPUT default.metallib
    COMMAND xcrun -sdk macosx metallib default.air -o default.metallib
    DEPENDS default.air
)

add_executable(
    ${PROJECT_NAME}
    MACOSX_BUNDLE
    ${SOURCES}
#    ${HEADER_FILES}
    process.metal
    default.metallib
)

set_source_files_properties(
    default.metallib
    PROPERTIES
    MACOSX_PACKAGE_LOCATION
    Resources
)

get_filename_component(Path_To_OpenCL ${OpenCL_INCLUDE_DIR} ABSOLUTE)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${Path_To_OpenCL}/lib/x86_64/opencl.lib)
target_link_libraries(${PROJECT_NAME} ${AMF_NAME})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} \
    -framework AppKit \
    -framework Metal \
    -framework MetalKit"
    )