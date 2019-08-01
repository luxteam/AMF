####### common block to be used in all

contains(QT_ARCH, x86_64){
    BUILD_ARCH=x64
} else {
    BUILD_ARCH=x86
}
exists($$_PRO_FILE_PWD_/make.pri) PROJECT_BUILD_ROOT=$$_PRO_FILE_PWD_
exists($$_PRO_FILE_PWD_/../make.pri) PROJECT_BUILD_ROOT=$$_PRO_FILE_PWD_/../
exists($$_PRO_FILE_PWD_/../../make.pri) PROJECT_BUILD_ROOT=$$_PRO_FILE_PWD_/../../
exists($$_PRO_FILE_PWD_/../../../make.pri) PROJECT_BUILD_ROOT=$$_PRO_FILE_PWD_/../../../
exists($$_PRO_FILE_PWD_/../../../../make.pri) PROJECT_BUILD_ROOT=$$_PRO_FILE_PWD_/../../../../
exists($$_PRO_FILE_PWD_/../../../../../make.pri) PROJECT_BUILD_ROOT=$$_PRO_FILE_PWD_/../../../../../


CONFIG(debug, debug|release) {
    BUILD_MODE = debug
} else {
    BUILD_MODE = release
}

win32:BUILD_FOLDER = win_$${BUILD_ARCH}/$${BUILD_MODE}
mingw:BUILD_FOLDER = mingw_$${BUILD_ARCH}/$${BUILD_MODE}
unix:BUILD_FOLDER  = linux_$${BUILD_ARCH}/$${BUILD_MODE}
mac:BUILD_FOLDER   = osx_$${BUILD_ARCH}/$${BUILD_MODE}


DESTDIR      = $${PROJECT_BUILD_ROOT}/bin/$${BUILD_FOLDER}

####### end common block

#-------------------------------------------------
#
# Project created by QtCreator 2019-07-31T11:11:40
#
#-------------------------------------------------

QT       -= core gui

TARGET = AMF
TEMPLATE = lib
DEFINES += AMF_LIBRARY

INCLUDEPATH += $${PWD}/../../../../../Thirdparty/OCL_SDK_Light/include
LIBS += -L$${PWD}/../../../../../Thirdparty/OCL_SDK_Light/lib/x86_64/ -lopencl

message($$INCLUDEPATH)
message($$LIBS)

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += AMF_CORE_EXPORTS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32:!win32-g++: LIBS += -lAdvapi32 -lOle32 -lwinspool -lshell32


SOURCES += \
    ../../../common/AMFSTL.cpp \
    ../../../common/CurrentTimeImpl.cpp \
    ../../../common/DataStreamFactory.cpp \
    ../../../common/DataStreamFile.cpp \
    ../../../common/DataStreamMemory.cpp \
    ../../../common/IOCapsImpl.cpp \
    ../../../common/Linux/ThreadLinux.cpp \
    ../../../common/PropertyStorageExImpl.cpp \
    ../../../common/Thread.cpp \
    ../../../common/TraceAdapter.cpp \
    ../../../common/Windows/ThreadWindows.cpp \
    ../../../src/core/BufferImpl.cpp \
    ../../../src/core/ComputeOCL.cpp \
    ../../../src/core/ContextImpl.cpp \
    ../../../src/core/DataImpl.cpp \
    ../../../src/core/DebugImpl.cpp \
    ../../../src/core/DeviceHostImpl.cpp \
    ../../../src/core/DeviceImpl.cpp \
    ../../../src/core/DeviceOCLImpl.cpp \
    ../../../src/core/FactoryImpl.cpp \
    ../../../src/core/PlaneImpl.cpp \
    ../../../src/core/SurfaceImpl.cpp \
    ../../../src/core/TraceImpl.cpp

HEADERS += \
    ../../../common/AMFMath.h \
    ../../../common/AMFSTL.h \
    ../../../common/ByteArray.h \
    ../../../common/CurrentTime.h \
    ../../../common/CurrentTimeImpl.h \
    ../../../common/DataStream.h \
    ../../../common/DataStreamFile.h \
    ../../../common/DataStreamMemory.h \
    ../../../common/IOCapsImpl.h \
    ../../../common/InterfaceImpl.h \
    ../../../common/ObservableImpl.h \
    ../../../common/PropertyStorageExImpl.h \
    ../../../common/PropertyStorageImpl.h \
    ../../../common/Thread.h \
    ../../../common/TraceAdapter.h \
    ../../../include/components/Ambisonic2SRenderer.h \
    ../../../include/components/AudioCapture.h \
    ../../../include/components/ColorSpace.h \
    ../../../include/components/Component.h \
    ../../../include/components/ComponentCaps.h \
    ../../../include/components/DisplayCapture.h \
    ../../../include/components/FFMPEGAudioConverter.h \
    ../../../include/components/FFMPEGAudioDecoder.h \
    ../../../include/components/FFMPEGAudioEncoder.h \
    ../../../include/components/FFMPEGComponents.h \
    ../../../include/components/FFMPEGFileDemuxer.h \
    ../../../include/components/FFMPEGFileMuxer.h \
    ../../../include/components/FFMPEGVideoDecoder.h \
    ../../../include/components/MediaSource.h \
    ../../../include/components/VideoCapture.h \
    ../../../include/components/VideoConverter.h \
    ../../../include/components/VideoDecoderUVD.h \
    ../../../include/components/VideoEncoderHEVC.h \
    ../../../include/components/VideoEncoderVCE.h \
    ../../../include/components/VideoStitch.h \
    ../../../include/components/ZCamLiveStream.h \
    ../../../include/core/AudioBuffer.h \
    ../../../include/core/Buffer.h \
    ../../../include/core/Compute.h \
    ../../../include/core/ComputeFactory.h \
    ../../../include/core/Context.h \
    ../../../include/core/Data.h \
    ../../../include/core/Debug.h \
    ../../../include/core/Dump.h \
    ../../../include/core/Factory.h \
    ../../../include/core/Interface.h \
    ../../../include/core/Plane.h \
    ../../../include/core/Platform.h \
    ../../../include/core/PropertyStorage.h \
    ../../../include/core/PropertyStorageEx.h \
    ../../../include/core/Result.h \
    ../../../include/core/Surface.h \
    ../../../include/core/Trace.h \
    ../../../include/core/Variant.h \
    ../../../include/core/Version.h \ \
    ../../../src/core/BufferImpl.h \
    ../../../src/core/ComputeOCL.h \
    ../../../src/core/ContextImpl.h \
    ../../../src/core/DataImpl.h \
    ../../../src/core/DebugImpl.h \
    ../../../src/core/Device.h \
    ../../../src/core/DeviceHostImpl.h \
    ../../../src/core/DeviceImpl.h \
    ../../../src/core/DeviceOCLImpl.h \
    ../../../src/core/FactoryImpl.h \
    ../../../src/core/PlaneImpl.h \
    ../../../src/core/SurfaceImpl.h \
    ../../../src/core/TraceImpl.h

