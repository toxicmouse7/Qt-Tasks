QT       += core gui multimedia multimediawidgets widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

TARGET = MultimediaWorker

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    audio.cpp \
    camera.cpp \
    main.cpp \
    mainwindow.cpp \
    screen.cpp

HEADERS += \
    audio.h \
    camera.h \
    mainwindow.h \
    screen.h

FORMS += \
    mainwindow.ui

INCLUDEPATH += C:\OpenCV\opencv\build\include

LIBS += C:\OpenCV\opencv-build\bin\libopencv_core343.dll
LIBS += C:\OpenCV\opencv-build\bin\libopencv_highgui343.dll
LIBS += C:\OpenCV\opencv-build\bin\libopencv_imgcodecs343.dll
LIBS += C:\OpenCV\opencv-build\bin\libopencv_imgproc343.dll
LIBS += C:\OpenCV\opencv-build\bin\libopencv_features2d343.dll
LIBS += C:\OpenCV\opencv-build\bin\libopencv_calib3d343.dll
LIBS += C:\OpenCV\opencv-build\bin\libopencv_video343.dll
LIBS += C:\OpenCV\opencv-build\bin\libopencv_videoio343.dll

RC_ICONS = multimediaworker.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
