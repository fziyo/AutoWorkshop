#include "AutoWorkshopSql.h"
#include <QStandardPaths>
#include <QDir>
#include <QUuid>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QDateTime>
#include "utils/TicketStatus.h"
#include "log/Log.h"

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
    if (!db.open())
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
                    "create_at DEFAULT CURRENT_TIMESTAMP);", "employees"))
    {
        return false;
    }

    // emp_schedules
    if(!execOrFail("CREATE TABLE IF NOT EXISTS emp_schedules ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "emp_id INTEGER NOT NULL, "
                    "ticket_id INTEGER NOT NULL, "
                    "schedule_date TEXT NOT NULL,"
                    "slot_index INTEGER NOT NULL,"
                    "FOREIGN KEY(emp_id) REFERENCES employees(id) ON DELETE CASCADE,"
                    "FOREIGN KEY(ticket_id) REFERENCES tickets(id) ON DELETE CASCADE);", "emp_schedules"))
    {
        return false;
    }

    // tickets
    if(!execOrFail("CREATE TABLE IF NOT EXISTS tickets ("
                    "id integer PRIMARY KEY AUTOINCREMENT, "
                    "customer TEXT NOT NULL, "
                    "brand TEXT NOT NULL, "
                    "model TEXT NOT NULL, "
                    "regis_id TEXT, "
                    "description TEXT, "
                    "status INTEGER NOT NULL, "
                    "total_to_pay REAL DEFAULT 0,"
                    "created_at TEXT DEFAULT CURRENT_TIMESTAMP);", "tickets"))
    {
        return false;
    }

    // ticket_parts
    if(!execOrFail("CREATE TABLE IF NOT EXISTS ticket_parts ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "ticket_id INTEGER NOT NULL, "
                    "part_name TEXT, "
                    "amount REAL, "
                    "unit_price REAL,"
                    "FOREIGN KEY(ticket_id) REFERENCES tickets(id) ON DELETE CASCADE);", "ticket_parts"))
    {
        return false;
    }

    // ticket_estimates
    if(!execOrFail("CREATE TABLE IF NOT EXISTS ticket_estimates ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "ticket_id INTEGER NOT NULL, "
                    "description TEXT, "
                    "expected_cost REAL, "
                    "accepted INTEGER DEFAULT 0,"
                    "created_at TEXT DEFAULT CURRENT_TIMESTAMP,"
                    "FOREIGN KEY(ticket_id) REFERENCES tickets(id) ON DELETE CASCADE);", "ticket_estimates"))
    {
        return false;
    }

    LOG_DEBUG(logDb) << "Database schema initialize success";

    return true;

}

// db level: provide api for user login verification
bool AutoWorkshopSql::verifyUser(const QString& username, const QString& password, int* userId)
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
        LOG_ERROR(logDb) << "createAccount failed" << lastDbError;
        return false;
    }

    auto query = createQuery();
    query.prepare("INSERT INTO users (username, password) VALUES (?, ?)");
    query.addBindValue(username); query.addBindValue(password);
    if (!query.exec())
    {
        lastDbError = query.lastError().text();
        LOG_ERROR(logDb) << "createAccount failed" << lastDbError;
        return false;
    }

    return true;
}

TicketDetailsDto AutoWorkshopSql::getTicket(int ticketId)
{
    TicketDetailsDto dto;

    if (!db.isOpen()) {
        LOG_ERROR(logDb) << "getTicket failed: DB not open";
        return dto;
    }

    QSqlQuery query = createQuery();

    query.prepare(R"(
        SELECT t.id,
               t.customer,
               t.brand,
               t.model,
               t.regis_id,
               t.description,
               t.status,
               t.total_to_pay,
               t.created_at,
               s.schedule_date,
               s.slot_index,
               e.name AS employee_name
        FROM tickets t
        LEFT JOIN emp_schedule s ON t.id = s.ticket_id
        LEFT JOIN employees e ON s.emp_id = e.id
        WHERE t.id = ?
    )");

    query.addBindValue(ticketId);

    if (!query.exec()) {
        LOG_ERROR(logDb) << "getTicket query failed:"
                         << query.lastError().text();
        return dto;
    }

    while (query.next())
    {
        if (dto.ticket.id == 0)
        {
            dto.ticket.id = query.value("id").toInt();
            dto.ticket.customer = query.value("customer").toString();
            dto.ticket.brand = query.value("brand").toString();
            dto.ticket.model = query.value("model").toString();
            dto.ticket.resgisId = query.value("regis_id").toString();
            dto.ticket.description = query.value("description").toString();
            dto.ticket.status =
                intToTicketStatus(query.value("status").toInt());
            dto.ticket.totalToPay =
                query.value("total_to_pay").toDouble();
            dto.ticket.createdAt =
                query.value("created_at").toString();
        }

        if (!query.value("schedule_date").isNull())
        {
            QDate date = QDate::fromString(query.value("schedule_date").toString(), "yyyy-MM-dd");

            dto.scheduleDate = date;

            dto.slotIndexes.append(query.value("slot_index").toInt());
        }

        if (!query.value("employee_name").isNull())
        {
            dto.employeeNames.append(
                query.value("employee_name").toString());
        }
    }

    return dto;
}

// QList<Ticket> AutoWorkshopSql::filterTicketById(const QString &input)
// {
//     QList<Ticket> tickets;

//     auto query = createQuery();
//     query.prepare("select * from tickets where cast(id as text) like :input");
//     query.bindValue(":input", "%" + input + "%");

//     if (!query.exec()) {
//         LOG_ERROR(logDb) << "filterTicketById query failed:" << query.lastError().text();
//         return tickets; // Return an empty list if the query fails
//     }

//     while (query.next()) {
//         Ticket ticket;

//         ticket.id= query.value("id").toInt();
//         ticket.customer = query.value("customer").toString();
//         ticket.brand = query.value("brand").toString();
//         ticket.model = query.value("model").toString();
//         ticket.resgisId = query.value("regis_id").toString();

//         ticket.empNames = query.value("emp_name").toString().split(", ");
//         ticket.date = query.value("date").toString();
//         ticket.timeSlots.append(query.value("slot0").toInt());
//         ticket.timeSlots.append(query.value("slot1").toInt());
//         ticket.timeSlots.append(query.value("slot2").toInt());
//         ticket.timeSlots.append(query.value("slot3").toInt());
//         ticket.timeSlots.append(query.value("slot4").toInt());
//         ticket.description = query.value("description").toString();
//         ticket.status = intToTicketStatus(query.value("status").toInt());

//         tickets.append(ticket);
//     }

//     return tickets;
// }

QList<TicketDetailsDto> AutoWorkshopSql::getWeeklyTickets(const QDate& startDate, const QDate& endDate)
{
    auto query = createQuery();
    query.prepare(R"(
        SELECT t.id,
               t.customer,
               t.brand,
               t.model,
               t.regis_id,
               t.description,
               t.status,
               t.total_to_pay,
               t.created_at,
               s.schedule_date,
               s.slot_index
        FROM tickets t
        JOIN emp_schedules s ON t.id = s.ticket_id
        WHERE s.schedule_date >= ?
          AND s.schedule_date <= ?
        ORDER BY s.schedule_date ASC
    )");
    query.addBindValue(startDate.toString("yyyy-MM-dd"));
    query.addBindValue(endDate.toString("yyyy-MM-dd"));
    QList<TicketDetailsDto> ticketDetailsDtos;

    if (!query.exec()) {
        LOG_ERROR(logDb) << "getWeeklyTickets query error:" << query.lastError();
        return ticketDetailsDtos;
    }
    QMap<int, TicketDetailsDto> ticketMap;
    while (query.next()) {
        int ticketId = query.value("id").toInt();

        if (!ticketMap.contains(ticketId))
        {
            TicketDetailsDto dto;
            dto.ticket.id = ticketId;
            dto.ticket.customer = query.value("customer").toString();
            dto.ticket.brand = query.value("brand").toString();
            dto.ticket.model = query.value("model").toString();
            dto.ticket.resgisId = query.value("regis_id").toString();
            dto.ticket.description = query.value("description").toString();
            dto.ticket.status = intToTicketStatus(query.value("status").toInt());
            dto.ticket.totalToPay = query.value("total_to_pay").toDouble();
            dto.ticket.createdAt = query.value("created_at").toString();

            dto.scheduleDate = QDate::fromString(query.value("schedule_date").toString(), "yyyy-MM-dd");

            ticketMap.insert(ticketId, dto);
        }
        ticketMap[ticketId].slotIndexes.append(query.value("slot_index").toInt());
    }
    ticketDetailsDtos = ticketMap.values();
    return ticketDetailsDtos;
}

bool AutoWorkshopSql::updateTicketStatus(const TicketDetailsDto& ticketDto, TicketStatus newStatus)
{
    auto query = createQuery();
    QString queryString = QString("update tickets set status = %1 where id = %2")
                              .arg(ticketStatusToInt(newStatus))
                              .arg(ticketDto.ticket.id);

    if (!query.exec(queryString))
    {
        LOG_ERROR(logDb) << "updateTickeStatus qSuery failed:" << query.lastError();
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

    if (!query.exec(queryString))
    {
        LOG_ERROR(logDb) << "updateTickeStatus Query failed:" << query.lastError();
        return false;
    }
    return true;
}


QString AutoWorkshopSql::getLastDbError() const
{
    return lastDbError;
}

QList<TicketDetailsDto> AutoWorkshopSql::getAllTicketDetails()
{
    QList<TicketDetailsDto> results;

    if (!db.isOpen()) {
        LOG_ERROR(logDb) << "getAllTicketDetails failed: DB not open";
        return results;
    }

    QSqlQuery query = createQuery();

    query.prepare(R"(
        SELECT t.id,
               t.customer,
               t.brand,
               t.model,
               t.regis_id,
               t.description,
               t.status,
               t.total_to_pay,
               t.created_at,
               s.schedule_date,
               s.slot_index,
               e.name AS employee_name
        FROM tickets t
        LEFT JOIN emp_schedules s ON t.id = s.ticket_id
        LEFT JOIN employees e ON s.emp_id = e.id
        ORDER BY t.id
    )");
    LOG_DEBUG(logDb) << "Executing query:" << query.lastQuery();

    if (!query.exec()) {
        LOG_ERROR(logDb) << "getAllTicketDetails SQL failed"
                         << query.lastError().text();
        return results;
    }

    QMap<int, TicketDetailsDto> ticketMap;
    while (query.next())
    {
        int ticketId = query.value("id").toInt();

        if (!ticketMap.contains(ticketId))
        {
            TicketDetailsDto dto;

            dto.ticket.id = ticketId;
            dto.ticket.customer = query.value("customer").toString();
            dto.ticket.brand = query.value("brand").toString();
            dto.ticket.model = query.value("model").toString();
            dto.ticket.resgisId = query.value("regis_id").toString();
            dto.ticket.description = query.value("description").toString();
            dto.ticket.status =
                intToTicketStatus(query.value("status").toInt());
            dto.ticket.totalToPay =
                query.value("total_to_pay").toDouble();
            dto.ticket.createdAt =
                query.value("created_at").toString();

            ticketMap.insert(ticketId, dto);
        }

        if (!query.value("schedule_date").isNull())
        {
            ticketMap[ticketId].scheduleDate =
                QDate::fromString(
                    query.value("schedule_date").toString(),
                    "yyyy-MM-dd");

            ticketMap[ticketId].slotIndexes.append(
                query.value("slot_index").toInt());
        }

        if (!query.value("employee_name").isNull())
        {
            ticketMap[ticketId].employeeNames.append(
                query.value("employee_name").toString());
        }
    }

    results = ticketMap.values();

    LOG_DEBUG(logDb)
        << "getAllTicketDetails success count="
        << results.size();

    return results;
}

// emp
bool AutoWorkshopSql::addEmployee(const EmployeeDto& info)
{

    auto query = createQuery();
    query.prepare("select * from employees where name = :name");
    query.bindValue(":name", info.name);

    if (!query.exec())
    {
        LOG_ERROR(logDb) << query.lastError();
        lastDbError = query.lastError().text();
        return false;
    }

    if (query.next())
    {
        lastDbError = "Employee exists.";
        LOG_ERROR(logDb) << lastDbError;
        return false;
    }

    query.prepare("insert into employees (name, tel, create_at) values (:name, :tel, :create_at)");
    query.bindValue(":name", info.name);
    query.bindValue(":tel", info.tel);
    query.bindValue(":create_at", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));

    if (!query.exec()) {
        lastDbError = query.lastError().text();
        LOG_ERROR(logDb) << lastDbError;
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
        lastDbError = query.lastError().text();
        LOG_ERROR(logDb) << lastDbError;
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

    LOG_DEBUG(logDb) << "Executing query getting all employees success!" << " Size: " << employees.size();

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
        lastDbError = query.lastError().text();
        LOG_ERROR(logDb) << lastDbError;
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

    return employees;
}

int AutoWorkshopSql::countScheduleConflicts(const QString& empId, const QDate& appointedDate, const QList<int>& timeSlots)
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

    if (slotConditions.isEmpty())
    {
        LOG_DEBUG(logDb)
        << "No time slots selected, skip conflict check"
        << "empId=" << empId
        << "date=" << appointedDate.toString("yyyy-MM-dd");
        return 0;
    }

    auto query = createQuery();
    QString sqlString = QString("SELECT COUNT(*) FROM emp_schedule WHERE emp_id = :empId AND schedule_date = :appointedDate AND %1")
        .arg(slotConditions.join(" AND "));
    query.prepare(sqlString);
    query.bindValue(":empId", empId);
    query.bindValue(":appointedDate", appointedDate);
    if (!query.exec())
    {
        lastDbError = query.lastError().text();
        LOG_ERROR(logDb)
            << "countScheduleConflicts SQL failed"
            << lastDbError;

        return 0;
    }

    return query.next() ? query.value(0).toInt() : 0;
}

AutoWorkshopSql::~AutoWorkshopSql() = default;
