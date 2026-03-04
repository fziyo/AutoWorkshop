#ifndef EMPLOYEESCHEDULE_H
#define EMPLOYEESCHEDULE_H
#include <QDate>
struct EmployeeSchedule
{
    int id;
    int empId;
    int ticketId;
    QDate date;
    int slotIndex;
};
#endif // EMPLOYEESCHEDULE_H
