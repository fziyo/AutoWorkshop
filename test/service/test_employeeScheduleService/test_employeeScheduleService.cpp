#include <QtTest>
#include "service/employeeScheduleService/EmployeeScheduleService.h"
#include "data/AutoWorkshopSql.h"

class FakeDb : public AutoWorkshopSql
{
public:
    int conflictCount = 0;

    FakeDb():AutoWorkshopSql("test.db") {}

    int countScheduleConflicts(const QString&, const QDate&, const QList<int>&)
    {
        return conflictCount;
    }
};

class TestEmployeeScheduleService : public QObject
{
    Q_OBJECT

private:
    FakeDb db;
    EmployeeScheduleService service{&db};

private slots:

    void testNoSlotSelected()
    {
        QList<int> ss = {0,0,0,0};

        auto result = service.checkEmployeeAvailability(
            "emp1",
            QDate::currentDate().addDays(1),
            ss
            );

        QCOMPARE(result, EmpAvailability::NoSlotSelected);
    }

    void testPastTime()
    {
        QList<int> ss = {1,0,0,0};

        auto result = service.checkEmployeeAvailability(
            "emp1",
            QDate::currentDate().addDays(-1),
            ss
            );

        QCOMPARE(result, EmpAvailability::PastTime);
    }

    void testConflict()
    {
        db.conflictCount = 1;

        QList<int> ss = {1,0,0,0};

        auto result = service.checkEmployeeAvailability(
            "emp1",
            QDate::currentDate().addDays(1),
            ss
            );

        QCOMPARE(result, EmpAvailability::NotAvailable);
    }

    void testAvailable()
    {
        db.conflictCount = 0;

        QList<int> ss = {1,0,0,0};

        auto result = service.checkEmployeeAvailability(
            "emp1",
            QDate::currentDate().addDays(1),
            ss
            );

        QCOMPARE(result, EmpAvailability::Available);
    }
};

QTEST_MAIN(TestEmployeeScheduleService)
#include "test_employeeScheduleService.moc"
