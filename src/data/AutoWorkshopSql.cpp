#include "AutoWorkshopSql.h"
#include <QStandardPaths>
#include <QDir>
#include <QUuid>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QDateTime>
#include "domain/ticket/TicketStatus.h"
#include "logger/Log.h"

AutoWorkshopSql::AutoWorkshopSql()
{
    QString connectionName = "autoworkshop_vann";
    LOG_DEBUG(logDb) << "Initializing database connection connectionName=" << connectionName;
    if (QSqlDatabase::contains(connectionName))
    {
        db = QSqlDatabase::database(connectionName);
        LOG_DEBUG(logDb) << "Use existing connection";
    }
    else
    {
        db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        LOG_DEBUG(logDb) << "Create new connection";
    }
}

bool AutoWorkshopSql::openDb()
{
    if (db.isOpen())
    {
        return true;
        LOG_DEBUG(logDb) << "Database already open";
    }

    QString dirPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    QDir().mkpath(dirPath);

    QString path = dirPath + "/autoworkshop_vann.db";
    db.setDatabaseName(path);
    LOG_DEBUG(logDb) << "Opening database path=" << path;
    if (!de.open())
    {
        LOG_ERROR(logDb) << "Failed to open database " << db.lastError().text();
        return false;
    }
    LOG_INFO(logDb) << "Database open success";
    return true;
}

bool AutoWorkshopSql::isOpen() const
{
    bool status = db.isValid() && db.isOpen();
    LOG_DEBUG(logDb) << "Database status isValid=" <<db.isValid() << "isOpen=" << db.isOpen();
    return status;
}

void AutoWorkshopSql::close()
{
    if(db.isValid() && db.isOpen())
    {
        db.close();
    }
}

QSqlQuery AutoWorkshopSql::createQuery()
{
    return QSqlQuery(db);
}

// create tables if not exist
bool AutoWorkshopSql::initSchema()
{
    lastDbError.clear();
    if(!isOpen())
    {
        lastDbError = "DB is not open.";
        return false;
    }

    LOG_DEBUG(logDb) << "Initializing database schema";

    // crate a query object
    //QSqlQuery query(db);
    auto query = createQuery();

    auto execOrFail = [&](const QString& queryStr, const QString& table) -> bool
    {
        if (!query.exec(queryStr))
        {
            lastDbError = query.lastError().text();
            LOG_ERROR(logDb) << "Failed to create table: " << table << lastDbError;
            return false;
        }
        return true;
    };

    // users table
    if(!execOrFail("CREATE TABLE IF NOT EXISTS users ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "username TEXT UNIQUE NOT NULL, "
                    "password TEXT NOT NULL);", "users"))
    {
         return false;
    }

    // employees

    if(!execOrFail("CREATE TABLE IF NOT EXISTS employees ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "name TEXT NOT NULL, "
                    "tel TEXT, "
                    "create_at TEXT);", "employees"))
    {
        return false;
    }

    // emp_schedule
    if(!execOrFail("CREATE TABLE IF NOT EXISTS emp_schedule ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "emp_id INTEGER NOT NULL, "
                    "ticket_id INTEGER NOT NULL, "
                    "schedule_date TEXT,"
                    "slot0 INTEGER, "
                    "slot1 INTEGER, "
                    "slot2 INTEGER, "
                    "slot3 INTEGER, "
                    "slot4 INTEGER);", "emp_schedule"))
    {
        return false;
    }

    // tickets table
    if(!execOrFail("CREATE TABLE IF NOT EXISTS tickets ("
                    "id integer PRIMARY KEY AUTOINCREMENT, "
                    "customer TEXT, "
                    "brand text, "
                    "model text, "
                    "regis_id text, "
                    "schedule_date text, "
                    "slot0 int, "
                    "slot1 int, "
                    "slot2 int, "
                    "slot3 int, "
                    "slot4 int, "
                    "description text, "
                    "status INTEGER, "
                    "total_to_pay REAL);", "tickets"))
    {
        return false;
    }

    // ticket_parts
    if(!execOrFail("CREATE TABLE IF NOT EXISTS ticket_parts ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "ticket_id INTEGER NOT NULL, "
                    "part_name TEXT, "
                    "amount REAL, "
                    "unit_price REAL);", "ticket_parts"))
    {
        return false;
    }

    // ticket_estimate
    if(!execOrFail("CREATE TABLE IF NOT EXISTS ticket_estimate ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "ticket_id INTEGER NOT NULL, "
                    "description TEXT, "
                    "expected_cost TEXT, "
                    "accepted INTEGER,"
                    "created_at TEXT);", "ticket_estimate"))
    {
        return false;
    }

    LOG_DEBUG(logDb) << "Database schema initialize success";

    return true;

}

// db level: provide api for user login verification
bool AutoWorkshopSql::verifyUser(const QString& username, const QString& password, int* userId, QString* role)
{
    lastDbError.clear();
    if(!db.isOpen())
    {
        lastDbError = "DB is not open.";
        LOG_ERROR(logDb) << "verifyUser failed" << lastDbError;
        return false;
    }

    auto query = createQuery();
    query.prepare("SELECT id FROM users WHERE username=? AND password=? ");
    query.addBindValue(username); query.addBindValue(password);
    LOG_DEBUG(logDb) << "Verifying user"
                     << "username=" << username;

    if(!query.exec())
    {
        lastDbError = query.lastError().text();
        LOG_ERROR(logDb) << "verifyUser sql failed" << lastDbError;
        return false;
    }

    if(!query.next())
    {
        lastDbError = "Invalid username or password.";
        LOG_INFO(logDb)
            << "Login failed"
            << "username=" << username;
        return false;
    }

    if (userId) *userId = query.value(0).toInt();
    // if(role) *role = query.value(1).toString();

    LOG_INFO(logDb)
        << "Login success"
        << "username=" << username
        << "userId=" << *userId;
    return true;
}

bool AutoWorkshopSql::checkUserExist(const QString& username)
{
    lastDbError.clear();
    if (!db.isOpen())
    {
        lastDbError = "Db is not open";
        LOG_ERROR(logDb) << "checkUserExist failed" << lastDbError;
        return false;
    }

    auto query = createQuery();
    query.prepare("SELECT id FROM users WHERE username = ?");
    query.addBindValue(username);
    if (!query.exec())
    {
        lastDbError = query.lastError().text();
        LOG_ERROR(logDb) << "checkUserExist sql failed" << lastDbError;
        return false;
    }

    if (!query.next())
    {
        LOG_DEBUG(logDb) << "User does not exist" << "username=" << username;
        return false;
    }
    return true;
}

bool AutoWorkshopSql::createAccount(const QString& username, const QString& password)
{
    lastDbError.clear();
    if (!isOpen())
    {
        lastDbError = "Db is not open";
        LOG_ERROR(logDb) << "checkUserExist failed" << lastDbError;
        return false;
    }

    auto query = createQuery();
    query.prepare("INSERT INTO users (username, password) VALUES (?, ?)");
    query.addBindValue(username); query.addBindValue(password);
    if (!query.exec())
    {
        lastDbError = query.lastError().text();
        return false;
    }

    return true;
}

Ticket AutoWorkshopSql::getTicket(int ticketId)
{
    Ticket ticket;

    auto query = createQuery();
    query.prepare("select * from tickets where id =:ticketId");
    query.bindValue(":ticketId", ticketId);

    if (!query.exec()) {
        qDebug() << "Query failed:" << query.lastError().text();
        return ticket;
    }

    while (query.next()) {
        ticket.id= query.value("id").toInt();
        ticket.customer = query.value("customer").toString();
        ticket.brand = query.value("brand").toString();
        ticket.model = query.value("model").toString();
        ticket.resgisId = query.value("regis_id").toString();

        ticket.empNames = query.value("emp_name").toString().split(", ");
        ticket.date = query.value("date").toString();
        ticket.timeSlots.append(query.value("slot0").toInt());
        ticket.timeSlots.append(query.value("slot1").toInt());
        ticket.timeSlots.append(query.value("slot2").toInt());
        ticket.timeSlots.append(query.value("slot3").toInt());
        ticket.timeSlots.append(query.value("slot4").toInt());
        ticket.description = query.value("description").toString();
        ticket.status = intToTicketStatus(query.value("status").toInt());
    }
    return ticket;
}

QList<Ticket> AutoWorkshopSql::filterTicketById(const QString &input)
{
    QList<Ticket> tickets;

    auto query = createQuery();
    query.prepare("select * from tickets where cast(id as text) like :input");
    query.bindValue(":input", "%" + input + "%");

    if (!query.exec()) {
        qDebug() << "Query failed:" << query.lastError().text();
        return tickets; // Return an empty list if the query fails
    }

    while (query.next()) {
        Ticket ticket;

        ticket.id= query.value("id").toInt();
        ticket.customer = query.value("customer").toString();
        ticket.brand = query.value("brand").toString();
        ticket.model = query.value("model").toString();
        ticket.resgisId = query.value("regis_id").toString();

        ticket.empNames = query.value("emp_name").toString().split(", ");
        ticket.date = query.value("date").toString();
        ticket.timeSlots.append(query.value("slot0").toInt());
        ticket.timeSlots.append(query.value("slot1").toInt());
        ticket.timeSlots.append(query.value("slot2").toInt());
        ticket.timeSlots.append(query.value("slot3").toInt());
        ticket.timeSlots.append(query.value("slot4").toInt());
        ticket.description = query.value("description").toString();
        ticket.status = intToTicketStatus(query.value("status").toInt());

        tickets.append(ticket);
    }

    return tickets;
}

QList<Ticket> AutoWorkshopSql::getWeeklyTickets(const QDate& startDate, const QDate& endDate)
{
    auto query = createQuery();
    QString queryString = QString("select * from tickets where date >= '%1' and date <= '%2' order by date asc")
                              .arg(startDate.toString("yyyy-MM-dd"))
                              .arg(endDate.toString("yyyy-MM-dd"));
    QList<Ticket> tickets;
    // Execute the query
    if (!query.exec(queryString)) {
        qDebug() << "Database query error:" << query.lastError();
        return tickets;
    }

    // Process the results
    while (query.next()) {
        Ticket ticket;
        // Assuming Ticket is a struct or class you've defined, and you map the query result to it
        ticket.id = query.value("id").toInt();
        ticket.status = intToTicketStatus(query.value("status").toInt());
        ticket.empNames.append(query.value("emp_name").toString());

        ticket.date = query.value("date").toString();
        ticket.timeSlots.append(query.value("slot0").toInt());
        ticket.timeSlots.append(query.value("slot1").toInt());
        ticket.timeSlots.append(query.value("slot2").toInt());
        ticket.timeSlots.append(query.value("slot3").toInt());
        ticket.timeSlots.append(query.value("slot4").toInt());

        tickets.append(ticket);
    }

    return tickets;
}

bool AutoWorkshopSql::updateTicketStatus(const Ticket& ticket, TicketStatus newStatus)
{
    auto query = createQuery();
    QString queryString = QString("update tickets set status = %1 where id = %2")
                              .arg(ticketStatusToInt(newStatus))
                              .arg(ticket.id);
    qDebug() << queryString;

    if (!query.exec(queryString))
    {
        qDebug() << "Car_workshop_sql::updateTickeStatus Query failed:" << query.lastError();
        return false;
    }
    return true;
}

bool AutoWorkshopSql::updateTicketStatusById(int ticketId, int newStatus)
{
    auto query = createQuery();
    QString queryString = QString("update tickets set status = %1 where id = %2")
                              .arg(newStatus)
                              .arg(ticketId);
    qDebug() << queryString;

    if (!query.exec(queryString))
    {
        qDebug() << "Car_workshop_sql::updateTickeStatus Query failed:" << query.lastError();
        return false;
    }
    return true;
}


QString AutoWorkshopSql::getLastDbError() const
{
    return lastDbError;
}

QList<Ticket> AutoWorkshopSql::getAllTickets()
{
    QList<Ticket> tickets;

    auto query = createQuery();
    query.exec("select * from tickets");
    while (query.next()) {
        qDebug() << "DB file:" << query.value(2).toString();
        Ticket ticket;
        ticket.id= query.value("id").toInt();
        ticket.customer = query.value("customer").toString();
        ticket.brand = query.value("brand").toString();
        ticket.model = query.value("model").toString();
        ticket.resgisId = query.value("regis_id").toString();
        //ticket.empNames
        ticket.empNames = query.value("emp_name").toString().split(", ");
        ticket.date = query.value("date").toString();
        ticket.timeSlots.append(query.value("slot0").toInt());
        ticket.timeSlots.append(query.value("slot1").toInt());
        ticket.timeSlots.append(query.value("slot2").toInt());
        ticket.timeSlots.append(query.value("slot3").toInt());
        ticket.timeSlots.append(query.value("slot4").toInt());
        ticket.description = query.value("description").toString();
        qDebug() << "Ticket status: " + query.value("status").toString();
        ticket.status = intToTicketStatus(query.value("status").toInt());

        tickets.append(ticket);
    }
    qDebug() << "==== Tickets fetched:" << tickets.size() << "====";

    for (int i = 0; i < tickets.size(); ++i) {
        const Ticket& t = tickets[i];
        qDebug()
            << "Ticket[" << i << "]"
            << "id=" << t.id
            << "customer=" << t.customer
            << "brand=" << t.brand
            << "model=" << t.model
            << "regisId=" << t.resgisId
            << "empNames=" << t.empNames
            << "date=" << t.date
            << "slots=" << t.timeSlots
            << "status=" << ticketStatusToString(t.status);
    }


    return tickets;
}

// emp
bool AutoWorkshopSql::addEmployee(const EmployeeDto& info)
{

    auto query = createQuery();
    query.prepare("select * from employees where name = :name");
    query.bindValue(":name", info.name);

    if (!query.exec())
    {
        qCCritical(logDb) << "Error executing SQL query:"<<query.lastError();
        lastDbError = query.lastError().text();
        return false;
    }

    if (query.next())
    {
        lastDbError = "Employee exists.";
        return false;
    }

    query.prepare("insert into employees (name, tel, create_at) values (:name, :tel, :create_at)");
    query.bindValue(":name", info.name);
    query.bindValue(":tel", info.tel);
    query.bindValue(":create_at", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));

    if (!query.exec()) {
        qCCritical(logDb) << "Error executing SQL query:"<<query.lastError();
        lastDbError = query.lastError().text();
        return false;
    }

    return true;

}

QList<Employee> AutoWorkshopSql::getAllEmployees()
{
    QList<Employee> employees;
    auto query = createQuery();
    query.prepare("select * from employees");
    if (!query.exec())
    {
        qCCritical(logDb) << "Error executing SQL query:"<<query.lastError();
        lastDbError = query.lastError().text();
        return employees;
    }

    while (query.next())
    {
        Employee employee;
        employee.id = query.value("id").toInt();
        employee.name = query.value("name").toString();
        employee.tel = query.value("tel").toString();
        employee.createTime = query.value("create_at").toString();
        employees.append(employee);
    }

    qCInfo(logDb) << "Executing query getting all employees success!" << " Size: " << employees.size();

    return employees;
}

QList<Employee> AutoWorkshopSql::filterByName(const QString& name)
{
    QList<Employee> employees;
    auto query = createQuery();
    query.prepare("select * from employees where name like :name");
    query.bindValue(":name", "%" + name + "%");

    if (!query.exec())
    {
        qCCritical(logDb) << "Error executing SQL query:"<<query.lastError();
        lastDbError = query.lastError().text();
        return employees;
    }

    while (query.next())
    {
        Employee employee;
        employee.id = query.value("id").toInt();
        employee.name = query.value("name").toString();
        employee.tel = query.value("tel").toString();
        employee.createTime = query.value("create_at").toString();
        employees.append(employee);
    }

    qCInfo(logDb) << "Executing query getting all employees success!" << " Size: " << employees.size();

    return employees;
}

int AutoWorkshopSql::countScheduleConflicts(const QString& empId, const QString& appointedDate, const QList<int>& timeSlots)
{
    qCInfo(logDb) << "Check employee schedule conflicts. ";
    QStringList slotConditions;
    for (int i = 0; i < timeSlots.size(); ++i)
    {
        if (timeSlots[i] == 1)
        {
            slotConditions.append(QString("slot%1 = 1").arg(i));
        }

    }
    qCInfo(logDb) << "Slot conditions: " << slotConditions;
    if (slotConditions.isEmpty())
        return 0;
    auto query = createQuery();
    QString sqlString = QString("SELECT COUNT(*) FROM emp_schedule WHERE emp_id = :empId AND schedule_date = :appointedDate AND %1")
        .arg(slotConditions.join(" AND "));
    query.prepare(sqlString);
    query.bindValue(":empId", empId);
    query.bindValue(":appointedDate", appointedDate);
    if (!query.exec())
    {
        qCCritical(logDb) << "Error executing SQL query:"<<query.lastError();
        lastDbError = query.lastError().text();
        return 0;
    }

    return query.next() ? query.value(0).toInt() : 0;
}

AutoWorkshopSql::~AutoWorkshopSql() = default;
