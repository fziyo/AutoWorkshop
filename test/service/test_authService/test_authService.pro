QT += core testlib sql
CONFIG += console testcase

TEMPLATE = app
TARGET = test_authService

INCLUDEPATH += $$PWD/../../../src

SOURCES += \
    $$PWD/test_authService.cpp \
    $$PWD/../../../src/service/authService/AuthService.cpp \
    $$PWD/../../../src/log/log.cpp \
     $$PWD/../../../src/data/AutoWorkshopSql.cpp
