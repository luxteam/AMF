cmake_minimum_required(VERSION 3.10)
project(AMF_Metal)
set(NAME AMF_Metal)
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -x objective-c")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_SKIP_RULE_DEPENDENCY TRUE)

enable_language(CXX)

set(HEADER_FILES
    #MetalDevice.h
    #MetalCompute.h
    #MetalComputeKernel.h
    AMFDeviceMetal.h
    AmfComputeKernelMetal.h
    ../../../common/AMFMath.h
    ../../../common/AMFSTL.h
    ../../../common/ByteArray.h
    ../../../common/CurrentTime.h
    #../../../common/CurrentTimeImpl.h
    ../../../common/DataStream.h
    ../../../common/DataStreamFile.h
    ../../../common/DataStreamMemory.h
    ../../../common/IOCapsImpl.h
    ../../../common/InterfaceImpl.h
    ../../../common/ObservableImpl.h
    ../../../common/PropertyStorageExImpl.h
    ../../../common/PropertyStorageImpl.h
    ../../../common/Thread.h
    ../../../common/TraceAdapter.h
    ../../../include/core/AudioBuffer.h
    ../../../include/core/Buffer.h
    ../../../include/core/Compute.h
    ../../../include/core/ComputeFactory.h
    ../../../include/core/Context.h
    ../../../include/core/Data.h
    ../../../include/core/Debug.h
    ../../../include/core/Dump.h
    ../../../include/core/Factory.h
    ../../../include/core/Interface.h
    ../../../include/core/Plane.h
    ../../../include/core/Platform.h
    ../../../include/core/PropertyStorage.h
    ../../../include/core/PropertyStorageEx.h
    ../../../include/core/Result.h
    ../../../include/core/Surface.h
    ../../../include/core/Trace.h
    ../../../include/core/Variant.h
    ../../../include/core/Version.h
    ../../../src/core/DebugImpl.h
    ../../../src/core/DeviceImpl.h
    ../../../src/core/FactoryImpl.h
    ../../../src/core/ProgramsImpl.h
    ../../../src/core/TraceImpl.h
    ../../../src/core/AMFFileTraceWriter.h
    ../../../src/core/AMFConsoleTraceWriter.h
    ../../../src/core/ContextImpl.h
)
set(SOURCE_FILES
    MetalDevice.mm
    MetalCompute.mm
    MetalComputeKernel.mm
    AMFDeviceMetal.mm
    AmfComputeKernelMetal.mm
    #../../../common/CurrentTimeImpl.cpp
    ../../../common/DataStreamFactory.cpp
    ../../../common/AMFSTL.cpp
    ../../../common/DataStreamFile.cpp
    ../../../common/DataStreamMemory.cpp
    ../../../common/IOCapsImpl.cpp
    ../../../common/PropertyStorageExImpl.cpp
    ../../../common/Thread.cpp
    ../../../common/TraceAdapter.cpp

    ../../../src/core/DataImpl.cpp
    ../../../src/core/FactoryImpl.cpp
    ../../../src/core/DebugImpl.cpp
    ../../../src/core/DeviceImpl.cpp
    ../../../src/core/ProgramsImpl.cpp
    ../../../src/core/TraceImpl.cpp
    ../../../src/core/AMFFileTraceWriter.cpp
    ../../../src/core/AMFConsoleTraceWriter.cpp
    ../../../common/Linux/ThreadLinux.cpp
    ../../../src/core/ContextImpl.cpp
)

#list(APPEND SOURCE_FILES ../../../common/Linux/ThreadLinux.cpp)

add_library(
    ${NAME}
    #STATIC
    SHARED
    ${SOURCE_FILES}
    ${HEADER_FILES}
    process.metal
)

set_source_files_properties(
    process.metal
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
    -framework MetalKit \
    -framework ModelIO \
    -framework CoreGraphics"
    )