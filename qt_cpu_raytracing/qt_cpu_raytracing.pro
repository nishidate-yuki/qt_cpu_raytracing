#-------------------------------------------------
#
# Project created by QtCreator 2020-04-17T19:35:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qt_cpu_raytracing
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17

SOURCES += \
        bvh.cpp \
        coordinateconverter.cpp \
        main.cpp \
        mainwindow.cpp \
        material.cpp \
        mesh.cpp \
        object.cpp \
        ray.cpp \
        renderer.cpp \
        renderview.cpp \
        sky.cpp \
        sphere.cpp \
        triangle.cpp \
        utils.cpp

HEADERS += \
        bvh.h \
        coordinateconverter.h \
        mainwindow.h \
        material.h \
        mesh.h \
        object.h \
        ray.h \
        renderer.h \
        renderview.h \
        sky.h \
        sphere.h \
        stb_image.h \
        triangle.h \
        utils.h

FORMS += \
        mainwindow.ui


# MinGW----------------------
#QMAKE_LFLAGS += -fopenmp
#QMAKE_CXXFLAGS += -fopenmp
#LIBS += -fopenmp
#QMAKE_CXXFLAGS += -fopenmp -O3 -mtune=native -march=native
#QMAKE_CXXFLAGS += -O3

# MSVC-----------------------
QMAKE_LFLAGS += -openmp
QMAKE_CXXFLAGS += -openmp
#LIBS += -fopenmp



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Support FBX
win32:CONFIG(release, debug|release): LIBS += -L'C:/Program Files/Autodesk/FBX/FBX SDK/2020.0.1/lib/vs2017/x64/release/' -llibfbxsdk
else:win32:CONFIG(debug, debug|release): LIBS += -L'C:/Program Files/Autodesk/FBX/FBX SDK/2020.0.1/lib/vs2017/x64/debug/' -llibfbxsdk

INCLUDEPATH += 'C:/Program Files/Autodesk/FBX/FBX SDK/2020.0.1/include'
DEPENDPATH += 'C:/Program Files/Autodesk/FBX/FBX SDK/2020.0.1/include'

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += 'C:/Program Files/Autodesk/FBX/FBX SDK/2020.0.1/lib/vs2017/x64/release/liblibfbxsdk.a'
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += 'C:/Program Files/Autodesk/FBX/FBX SDK/2020.0.1/lib/vs2017/x64/debug/liblibfbxsdk.a'
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += 'C:/Program Files/Autodesk/FBX/FBX SDK/2020.0.1/lib/vs2017/x64/release/libfbxsdk.lib'
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += 'C:/Program Files/Autodesk/FBX/FBX SDK/2020.0.1/lib/vs2017/x64/debug/libfbxsdk.lib'

