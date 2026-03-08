#ifndef APPCONTEXT_H
#define APPCONTEXT_H

#include <QObject>
#include <QString>
#include "data/AutoWorkshopSql.h"
#include "utils/UserSession.h"
#include "service/authService/AuthService.h"
#include "service/ticketService/TicketService.h"
#include "service/employeeService/EmployeeService.h"
#include "service/employeeScheduleService/EmployeeScheduleService.h"

class AutoWorkshopSql;

class AppContext final: public QObject
{
    Q_OBJECT
public:
    static AppContext& instance();
    // init whole app context
    void init();
    // for testing service
    void setDb(AutoWorkshopSql* db);
    AutoWorkshopSql& getDb();


    // user session
    void setSession(const std::optional<UserSession>& session);
    const std::optional<UserSession>& getSession() const;
    void clearSession();

    AuthService* getAuthService();
    TicketService* getTicketService();
    EmployeeService* getEmployeeService();
    EmployeeScheduleService* getEmployeeScheduleService();
    ~AppContext();

private:
    explicit AppContext(QObject* parent = nullptr);


signals:
    void sessionChanged();

private:
    std::unique_ptr<AutoWorkshopSql> db;   // single db
    std::unique_ptr<AuthService> authService;
    std::unique_ptr<TicketService> ticketService;
    std::unique_ptr<EmployeeService> employeeService;
    std::unique_ptr<EmployeeScheduleService> employeeScheduleService;
    std::optional<UserSession> m_session;

};

#endif // APPCONTEXT_H
