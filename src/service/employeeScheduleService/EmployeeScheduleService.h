#ifndef EMPLOYEESCHEDULESERVICE_H
#define EMPLOYEESCHEDULESERVICE_H
#include "data/AutoWorkshopSql.h"
#include "domain/schedule/EmpAvailability.h"

class EmployeeScheduleService
{
public:
    EmployeeScheduleService(AutoWorkshopSql* db);
    EmpAvailability checkEmployeeAvailability(const QString& empId, const QDate& appointedDate, const QList<int>& timeSlots);

private:
    AutoWorkshopSql* m_db;
    QString error;
};

#endif // EMPLOYEESCHEDULESERVICE_H
