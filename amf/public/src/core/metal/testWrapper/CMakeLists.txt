cmake_minimum_required(VERSION 3.0)
project(Test)
set(NAME Test)
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -x objective-c")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_SKIP_RULE_DEPENDENCY TRUE)

enable_language(CXX)

set(HEADER_FILES

)
set(SOURCE_FILES
    main_metal.cpp
)
add_subdirectory(.. AMF_Metal)

add_custom_command(OUTPUT default.air
    COMMAND xcrun -sdk macosx metal -c ../process.metal -o default.air
)
add_custom_command(OUTPUT default.metallib
    COMMAND xcrun -sdk macosx metallib default.air -o default.metallib
    DEPENDS default.air
)


add_executable(
    ${NAME}
    MACOSX_BUNDLE
    ${SOURCE_FILES}
    ${HEADER_FILES}
    process.metal
    default.metallib
)


target_link_libraries(${NAME} AMF_Metal)

set_source_files_properties(
    default.metallib
    PROPERTIES
    MACOSX_PACKAGE_LOCATION
    Resources
)

# set_target_properties(
#     ${NAME}
#     PROPERTIES
#     MACOSX_BUNDLE_INFO_PLIST
#     ${CMAKE_CURRENT_LIST_DIR}/plist.in
# )

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} \
    -framework AppKit \
    -framework Metal \
    -framework MetalKit"
    )
