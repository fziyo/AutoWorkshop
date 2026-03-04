#ifndef APPCONTEXT_H
#define APPCONTEXT_H

#include <QObject>
#include <QString>
#include "data/AutoWorkshopSql.h"
#include "utils/UserSession.h"
#include "service/ticketService/TicketService.h"
#include "service/employeeService/EmployeeService.h"
#include "service/employeeScheduleService/EmployeeScheduleService.h"

class AutoWorkshopSql;

class AppContext final: public QObject
{
    Q_OBJECT
public:
    static AppContext& instance();
    AutoWorkshopSql& getDb();

    // user session
    void setSession(const std::optional<UserSession>& session);
    const std::optional<UserSession>& getSession() const;
    void clearSession();
     ~AppContext();

    TicketService* getTicketService();
    EmployeeService* getEmployeeService();
    EmployeeScheduleService* getEmployeeScheduleService();

private:
    explicit AppContext(QObject* parent = nullptr);


signals:
    void sessionChanged();

private:
    std::unique_ptr<AutoWorkshopSql> db;   // single db
    std::unique_ptr<TicketService> ticketService;
    std::unique_ptr<EmployeeService> employeeService;
    std::unique_ptr<EmployeeScheduleService> employeeScheduleService;
    std::optional<UserSession> m_session;

};

#endif // APPCONTEXT_H
