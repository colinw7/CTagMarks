TEMPLATE = app

TARGET = CQTagMarks

DEPENDPATH += .

QT += widgets

QMAKE_CXXFLAGS += -std=c++14

MOC_DIR = .moc

CONFIG += debug

# Input
SOURCES += \
CQTagMarks.cpp \
CMessage.cpp \
main.cpp \

HEADERS += \
CQTagMarks.h \
CMessage.h \

DESTDIR     = ../bin
OBJECTS_DIR = ../obj
LIB_DIR     = ../lib

INCLUDEPATH += \
../include \
../../CQUtil/include \
../../CHtml/include \
../../CXML/include \
../../CFile/include \
../../CStrUtil/include \
../../COS/include \
../../CUtil/include \

unix:LIBS += \
-L$$LIB_DIR \
-L../../CQUtil/lib \
-L../../CTagMarks/lib \
-L../../CBookmarks/lib \
-L../../CHtml/lib \
-L../../CXML/lib \
-L../../CFile/lib \
-L../../CStrUtil/lib \
-L../../CRegExp/lib \
-L../../COS/lib \
-L../../CUtil/lib \
-lCQUtil -lCTagMarks -lCBookmarks -lCHtml -lCXML -lCFile -lCStrUtil -lCOS -lCUtil \
-lCRegExp -ltre \
