#ifndef AUTOWORKSHOPSQL_H
#define AUTOWORKSHOPSQL_H

#include <QString>
#include <QSqlDatabase>
#include <QDate>
#include "entity/Ticket.h"
#include "entity/Employee.h"
#include "utils/TicketStatus.h"
#include "dtos/EmployeeDto.h"
#include "dtos/TicketDetailsDto.h"

class AutoWorkshopSql
{
public:
    AutoWorkshopSql();
    // for test
    AutoWorkshopSql(const QString& dbPath);
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
    TicketDetailsDto getTicket(int ticketId);
    //QList<Ticket> filterTicketById(const QString &input);
    QList<TicketDetailsDto> getWeeklyTickets(const QDate& startDate, const QDate& endDate);
    bool updateTicketStatus(const TicketDetailsDto& ticket, TicketStatus newStatus);
    bool updateTicketStatusById(int ticketId, int newStatus);
    QList<TicketDetailsDto> getAllTicketDetails();

    // empployee
    bool addEmployee(const EmployeeDto& info);
    QList<Employee> getAllEmployees();
    QList<Employee> filterByName(const QString& name);
    virtual int countScheduleConflicts(const QString& empId, const QDate& appointedDate, const QList<int>& timeSlots);

private:
    QSqlDatabase db;
    QString lastDbError;

};

#endif // AUTOWORKSHOPSQL_H
