QT += core testlib sql
CONFIG += console testcase

TEMPLATE = app
TARGET = test_employeeScheduleService

INCLUDEPATH += $$PWD/../../../src

SOURCES += \
    $$PWD/test_employeeScheduleService.cpp \
    $$PWD/../../../src/service/employeeScheduleService/EmployeeScheduleService.cpp \
    $$PWD/../../../src/log/log.cpp \
    $$PWD/../../../src/data/AutoWorkshopSql.cpp
