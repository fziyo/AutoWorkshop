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

AppContext::AppContext(QObject* parent):QObject(parent)
{
    db = std::make_unique<AutoWorkshopSql>();
    // open database
    if (!db->openDb()) {
        qFatal("Cannot open database");
    }
    // create tables if not exist
    if (!db->initSchema()) {
        qFatal("Cannot init database schema");
    }
    ticketService = std::make_unique<TicketService>(db.get());
    employeeService = std::make_unique<EmployeeService>(db.get());
    employeeScheduleService = std::make_unique<EmployeeScheduleService>(db.get());
}

AutoWorkshopSql& AppContext::getDb()
{
    return *db;
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
    m_session = UserSession{};
    emit sessionChanged();
}

AppContext::~AppContext() = default;





