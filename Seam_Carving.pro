#-------------------------------------------------
#
# Project created by QtCreator 2015-11-01T19:05:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DIA_Ass4
TEMPLATE = app

CONFIG += console c++11

INCLUDEPATH += /usr/local/include/opencv /usr/local/include/opencv2

LIBS += -L/usr/local/lib/ -lopencv_stitching -lopencv_superres -lopencv_videostab -lopencv_objdetect -lopencv_shape -lopencv_video -lopencv_calib3d -lopencv_features2d -lopencv_ml -lopencv_flann -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_photo -lopencv_imgproc -lopencv_core


SOURCES += main.cpp\
        mainwindow.cpp \
    seam.cpp

HEADERS  += mainwindow.h \
    seam.h

FORMS    += mainwindow.ui
