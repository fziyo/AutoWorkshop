#include "AppContext.h"
#include "data/AutoWorkshopSql.h"
#include <QDebug>
#include <QStandardPaths>
#include <QDir>

AppContext& AppContext::instance()
{
    static AppContext context;
    return context;
}
// create context
AppContext::AppContext(QObject* parent)
    : QObject(parent)
{
}
// initialize db and service
void AppContext::init()
{
    db = std::make_unique<AutoWorkshopSql>();

    db->openDb();
    db->initSchema();

    authService = std::make_unique<AuthService>(db.get());
    ticketService = std::make_unique<TicketService>(db.get());
    employeeService = std::make_unique<EmployeeService>(db.get());
    employeeScheduleService = std::make_unique<EmployeeScheduleService>(db.get());
}
// for test
void AppContext::setDb(AutoWorkshopSql* db_)
{
    db.reset(db_);
}

AutoWorkshopSql& AppContext::getDb()
{
    return *db;
}

AuthService* AppContext::getAuthService()
{
    return authService.get();
}

TicketService* AppContext::getTicketService()
{
    return ticketService.get();
}

EmployeeService* AppContext::getEmployeeService()
{
    return employeeService.get();
}

EmployeeScheduleService* AppContext::getEmployeeScheduleService()
{
    return employeeScheduleService.get();
}

// user session
void AppContext::setSession(const std::optional<UserSession>& session)
{
    m_session = session;
    emit sessionChanged();
}

const std::optional<UserSession>& AppContext::getSession() const
{
    return m_session;
}

void AppContext::clearSession()
{
    m_session.reset();
    emit sessionChanged();
}

AppContext::~AppContext() = default;





