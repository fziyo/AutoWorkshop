#ifndef AUTOWORKSHOPSQL_H
#define AUTOWORKSHOPSQL_H

#include <QString>
#include <QSqlDatabase>
#include <QDate>
#include "domain/ticket/Ticket.h"
#include "domain/employee/Employee.h"
#include "domain/ticket/TicketStatus.h"
#include "dtos/EmployeeDto.h"

class AutoWorkshopSql
{
public:
    AutoWorkshopSql();
    ~AutoWorkshopSql();

    bool openDb();
    bool isOpen() const;
    void close();
    bool initSchema();
    QString getLastDbError() const;
    QSqlQuery createQuery();

    bool verifyUser(const QString& username, const QString& password, int* userId = nullptr);
    bool checkUserExist(const QString& username);
    bool createAccount(const QString& username, const QString& password);
    Ticket getTicket(int ticketId);
    QList<Ticket> filterTicketById(const QString &input);
    QList<Ticket> getWeeklyTickets(const QDate& startDate, const QDate& endDate);
    bool updateTicketStatus(const Ticket& ticket, TicketStatus newStatus);
    bool updateTicketStatusById(int ticketId, int newStatus);
    QList<Ticket> getAllTickets();

    // empployee
    bool addEmployee(const EmployeeDto& info);
    QList<Employee> getAllEmployees();
    QList<Employee> filterByName(const QString& name);
    int countScheduleConflicts(const QString& empId, const QString& appointedDate, const QList<int>& timeSlots);

private:
    QSqlDatabase db;
    QString lastDbError;

};

#endif // AUTOWORKSHOPSQL_H
