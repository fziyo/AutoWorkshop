QT += core testlib sql

CONFIG += console testcase

TEMPLATE = app
TARGET = test_AutoWorkshopSql

INCLUDEPATH += ../../src

SOURCES += \
    test_AutoWorkshopSql.cpp \
    ../../src/data/AutoWorkshopSql.cpp \
    ../../src/log/log.cpp
