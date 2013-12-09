include(../common.pri)

QT       += core
#QT       += script
QT       += testlib

TARGET = qi_tests

CONFIG   += console
CONFIG   -= app_bundle

INCLUDEPATH += $$ROOT_DIR/src/

TEMPLATE = app

HEADERS +=  test_signal.h \
            test_cell_id.h

SOURCES +=  main.cpp \
            test_signal.cpp \
            test_cell_id.cpp