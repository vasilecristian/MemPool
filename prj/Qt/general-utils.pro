#-------------------------------------------------
#
# Project created by QtCreator 2014-04-11T00:38:03
#
#-------------------------------------------------

QT       -= core gui

CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS -= -fexceptions
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3
QMAKE_CXXFLAGS_RELEASE += -m64


TARGET = ../../lib/general-utils
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += /usr/local/include
INCLUDEPATH += ../../include
INCLUDEPATH += ../../include/fast-rtti

HEADERS += \
    ../../include/fast-rtti/RTTI.h \
    ../../include/gu/StateMachine/StatesStack.h \
    ../../include/gu/StateMachine/State.h \
    ../../include/gu/Utils.h \
    ../../include/gu/Updatable.hpp \
    ../../include/gu/Threads.h \
    ../../include/gu/SmartPtr.hpp \
    ../../include/gu/Singleton.hpp \
    ../../include/gu/MemPool.h \
    ../../include/gu/Log.h \
    ../../include/gu/InstanceFactory.h \
    ../../include/gu/EventsQueue.h \
    ../../include/gu/EventReceiver.hpp \
    ../../include/gu/EventBase.hpp \
    ../../include/gu/Drawable.hpp \
    ../../include/gu/MemLeakDetector.h \
    ../../include/gu/SmartPtrQueue.hpp

SOURCES += \
    ../../source/gu/StateMachine/StatesStack.cpp \
    ../../source/gu/StateMachine/State.cpp \
    ../../source/gu/Utils.cpp \
    ../../source/gu/Threads.cpp \
    ../../source/gu/MemPool.cpp \
    ../../source/gu/Log.cpp \
    ../../source/gu/InstanceFactory.cpp \
    ../../source/gu/EventsQueue.cpp \
    ../../source/gu/MemLeakDetector.cpp
