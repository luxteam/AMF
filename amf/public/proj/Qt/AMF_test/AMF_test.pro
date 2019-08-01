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

LIBS += -L$${DESTDIR} -lamf

QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += AMF_CORE_STATIC

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ../../../common/AMFFactory.cpp \
        ../../../common/Windows/ThreadWindows.cpp \
        main.cpp

HEADERS += \
    ../../../common/AMFFactory.h

