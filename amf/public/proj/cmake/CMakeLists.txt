cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_SKIP_RULE_DEPENDENCY TRUE)

enable_language(CXX)

message("Prepare open AMF project")

# detects demanded name (previosly set in setup_amf.cmake)
get_property(AMF_NAME GLOBAL PROPERTY AMF_NAME)

if(NOT AMF_NAME)
  #set_property(GLOBAL PROPERTY AMF_NAME open_amf)
  message(FATAL_ERROR "Error: open AMF project name not set!")
endif()

# library name
project(${AMF_NAME})
include_directories(../../..)

#ADD_DEFINITIONS(-DAMF_CORE_STATIC)

# sources
set(
  SOURCE_LIB

  ../../common/AMFFactory.cpp
  ../../common/AMFSTL.cpp
  ../../common/CurrentTimeImpl.cpp
  ../../common/DataStreamFactory.cpp
  ../../common/DataStreamFile.cpp
  ../../common/DataStreamMemory.cpp
  ../../common/IOCapsImpl.cpp
  ../../common/Linux/ThreadLinux.cpp
  ../../common/PropertyStorageExImpl.cpp
  ../../common/Thread.cpp
  ../../common/TraceAdapter.cpp
  ../../common/Windows/ThreadWindows.cpp

  ../../src/core/BufferImpl.cpp
  ../../src/core/ComputeOCL.cpp
  ../../src/core/ContextImpl.cpp
  ../../src/core/DataImpl.cpp
  ../../src/core/DebugImpl.cpp
  ../../src/core/DeviceHostImpl.cpp
  ../../src/core/DeviceImpl.cpp
  ../../src/core/DeviceOCLImpl.cpp
  ../../src/core/FactoryImpl.cpp
  ../../src/core/PlaneImpl.cpp
  ../../src/core/ProgramsImpl.cpp
  ../../src/core/SurfaceImpl.cpp
  ../../src/core/TraceImpl.cpp
)

if(WIN32)
  list(APPEND SOURCE_LIB ../../common/Windows/ThreadWindows.cpp)
else()
  list(APPEND SOURCE_LIB ../../common/Linux/ThreadLinux.cpp)
endif()

set(
  HEADER_LIB

  ../../common/AMFMath.h
  ../../common/AMFSTL.h
  ../../common/ByteArray.h
  ../../common/CurrentTime.h
  ../../common/CurrentTimeImpl.h
  ../../common/DataStream.h
  ../../common/DataStreamFile.h
  ../../common/DataStreamMemory.h
  ../../common/IOCapsImpl.h
  ../../common/InterfaceImpl.h
  ../../common/ObservableImpl.h
  ../../common/PropertyStorageExImpl.h
  ../../common/PropertyStorageImpl.h
  ../../common/Thread.h
  ../../common/TraceAdapter.h
  ../../include/components/Ambisonic2SRenderer.h
  ../../include/components/AudioCapture.h
  ../../include/components/ColorSpace.h
  ../../include/components/Component.h
  ../../include/components/ComponentCaps.h
  ../../include/components/DisplayCapture.h
  ../../include/components/FFMPEGAudioConverter.h
  ../../include/components/FFMPEGAudioDecoder.h
  ../../include/components/FFMPEGAudioEncoder.h
  ../../include/components/FFMPEGComponents.h
  ../../include/components/FFMPEGFileDemuxer.h
  ../../include/components/FFMPEGFileMuxer.h
  ../../include/components/FFMPEGVideoDecoder.h
  ../../include/components/MediaSource.h
  ../../include/components/VideoCapture.h
  ../../include/components/VideoConverter.h
  ../../include/components/VideoDecoderUVD.h
  ../../include/components/VideoEncoderHEVC.h
  ../../include/components/VideoEncoderVCE.h
  ../../include/components/VideoStitch.h
  ../../include/components/ZCamLiveStream.h
  ../../include/core/AudioBuffer.h
  ../../include/core/Buffer.h
  ../../include/core/Compute.h
  ../../include/core/ComputeFactory.h
  ../../include/core/Context.h
  ../../include/core/Data.h
  ../../include/core/Debug.h
  ../../include/core/Dump.h
  ../../include/core/Factory.h
  ../../include/core/Interface.h
  ../../include/core/Plane.h
  ../../include/core/Platform.h
  ../../include/core/PropertyStorage.h
  ../../include/core/PropertyStorageEx.h
  ../../include/core/Result.h
  ../../include/core/Surface.h
  ../../include/core/Trace.h
  ../../include/core/Variant.h
  ../../include/core/Version.h
  ../../src/core/BufferImpl.h
  ../../src/core/ComputeOCL.h
  ../../src/core/ContextImpl.h
  ../../src/core/DataImpl.h
  ../../src/core/DebugImpl.h
  ../../src/core/Device.h
  ../../src/core/DeviceHostImpl.h
  ../../src/core/DeviceImpl.h
  ../../src/core/DeviceOCLImpl.h
  ../../src/core/FactoryImpl.h
  ../../src/core/PlaneImpl.h
  ../../src/core/ProgramsImpl.h
  ../../src/core/SurfaceImpl.h
  ../../src/core/TraceImpl.h
  )


# declare static library creation
add_library(
  ${AMF_NAME}
  STATIC
  ${SOURCE_LIB}
  ${HEADER_LIB}
  )

set_property(TARGET ${AMF_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

if(NOT WIN32)
  target_compile_options(${AMF_NAME} PUBLIC -mavx2)
  target_compile_options(${AMF_NAME} PUBLIC -mfma)
  target_compile_options(${AMF_NAME} PUBLIC -msse4.2)
  target_compile_options(${AMF_NAME} PUBLIC -g) #todo: to Debug only
  
  if(NOT APPLE)
    target_compile_options(${AMF_NAME} PUBLIC -Wpsabi)
  endif()
endif()