#include <QtTest/QtTest>
#include <QSqlDatabase>
#include <QTemporaryDir>
#include <QSqlQuery>
#include <QSqlError>

#include "data/AutoWorkshopSql.h"
#include "utils/TicketStatus.h"

class TestAutoWorkshopSql : public QObject
{
    Q_OBJECT

private:
    QTemporaryDir tempDir;
    std::unique_ptr<AutoWorkshopSql> db;

private slots:

    void initTestCase();
    void cleanupTestCase();

    void testOpenDb();
    void testInitSchema();

    void testCreateAccount();
    void testVerifyUser();

    void testAddEmployee();
    void testGetAllEmployees();

    void testGetTicket();
    void testGetWeeklyTickets();

    void testUpdateTicketStatus();
    void testUpdateTicketStatusById();

    void testGetAllTicketDetails();

    void testCountScheduleConflicts();

private:

    int createTestTicket();
    int createTestEmployee(const QString& name);
    void createSchedule(int empId,int ticketId,
                        const QString& date,int slot);
};



// ---------- init ----------

void TestAutoWorkshopSql::initTestCase()
{
    QString dbPath = tempDir.path() + "/test_data.db";

    db = std::make_unique<AutoWorkshopSql>(dbPath);

    QVERIFY(db->openDb());
    QVERIFY(db->initSchema());
}

void TestAutoWorkshopSql::cleanupTestCase()
{
    db->close();
}



// ---------- basic db ----------

void TestAutoWorkshopSql::testOpenDb()
{
    QVERIFY(db->isOpen());
}

void TestAutoWorkshopSql::testInitSchema()
{
    QVERIFY(db->initSchema());
}



// ---------- user ----------

void TestAutoWorkshopSql::testCreateAccount()
{
    QString username = "test_user";
    QString password = "123456";

    bool result = db->createAccount(username,password);

    QVERIFY(result);
}

void TestAutoWorkshopSql::testVerifyUser()
{
    QString username = "login_user";
    QString password = "password";

    db->createAccount(username,password);

    int userId = -1;

    bool loginResult =
        db->verifyUser(username,password,&userId);

    QVERIFY(loginResult);
    QVERIFY(userId > 0);
}



// ---------- employee ----------

void TestAutoWorkshopSql::testAddEmployee()
{
    EmployeeDto dto;

    dto.name = "John";
    dto.tel  = "123456";

    bool result = db->addEmployee(dto);

    QVERIFY(result);
}

void TestAutoWorkshopSql::testGetAllEmployees()
{
    EmployeeDto dto;

    dto.name = "Alice";
    dto.tel  = "999999";

    db->addEmployee(dto);

    auto employees = db->getAllEmployees();

    QVERIFY(employees.size() > 0);
}



// ---------- ticket helpers ----------

int TestAutoWorkshopSql::createTestTicket()
{
    QSqlQuery q = db->createQuery();

    q.prepare(R"(

        INSERT INTO tickets
        (customer,brand,model,regis_id,
         description,status,total_to_pay,created_at)

        VALUES
        ('Tom','BMW','X5','ABC123',
         'Test repair',0,0,
         datetime('now'))

    )");

    if(!q.exec())
        qDebug() << q.lastError();

    return q.lastInsertId().toInt();
}

int TestAutoWorkshopSql::createTestEmployee(const QString& name)
{
    QSqlQuery q(db->createQuery());

    q.prepare(R"(

        INSERT INTO employees
        (name,tel,create_at)

        VALUES
        (?, '111111',
         datetime('now'))

    )");

    q.addBindValue(name);

    if(!q.exec())
        qDebug() << q.lastError();

    return q.lastInsertId().toInt();
}

void TestAutoWorkshopSql::createSchedule(
    int empId,
    int ticketId,
    const QString& date,
    int slot)
{
    QSqlQuery q(db->createQuery());

    q.prepare(R"(

        INSERT INTO emp_schedules
        (emp_id,ticket_id,
         schedule_date,slot_index)

        VALUES (?, ?, ?, ?)

    )");

    q.addBindValue(empId);
    q.addBindValue(ticketId);
    q.addBindValue(date);
    q.addBindValue(slot);

    if(!q.exec())
        qDebug() << q.lastError();
}



// ---------- ticket ----------

void TestAutoWorkshopSql::testGetTicket()
{
    int ticketId = createTestTicket();
qDebug() << "ticketId=" << ticketId;
    int empId = createTestEmployee("Bob");

    createSchedule(empId,ticketId, "2026-03-10",1);

    createSchedule(empId,ticketId, "2026-03-10",2);

    auto dto = db->getTicket(ticketId);

    QVERIFY(dto.ticket.id == ticketId);
    QVERIFY(dto.slotIndexes.size() == 2);
}



void TestAutoWorkshopSql::testGetWeeklyTickets()
{
    int ticketId = createTestTicket();

    int empId = createTestEmployee("Mike");

    createSchedule(empId,ticketId,
                   "2026-03-10",1);

    auto list =
        db->getWeeklyTickets(
            QDate(2026,3,9),
            QDate(2026,3,15));

    QVERIFY(list.size() > 0);
}



// ---------- status ----------

void TestAutoWorkshopSql::testUpdateTicketStatus()
{
    int ticketId = createTestTicket();

    auto dto = db->getTicket(ticketId);

    bool ok =
        db->updateTicketStatus(
            dto,
            TicketStatus::Closed);

    QVERIFY(ok);
}

void TestAutoWorkshopSql::testUpdateTicketStatusById()
{
    int ticketId = createTestTicket();

    bool ok =
        db->updateTicketStatusById(
            ticketId,
            2);

    QVERIFY(ok);
}



// ---------- queries ----------

void TestAutoWorkshopSql::testGetAllTicketDetails()
{
    int ticketId = createTestTicket();

    int empId = createTestEmployee("Anna");

    createSchedule(empId,ticketId,
                   "2026-03-11",2);

    auto list = db->getAllTicketDetails();

    QVERIFY(list.size() > 0);
}



// ---------- schedule ----------

void TestAutoWorkshopSql::testCountScheduleConflicts()
{
    int ticketId = createTestTicket();

    int empId = createTestEmployee("Jack");

    createSchedule(empId,ticketId,
                   "2026-03-12",1);

    QList<int> ss = {0,1,0,0,0};

    int count = db->countScheduleConflicts(
            QString::number(empId),
            QDate(2026,3,12),
            ss);

    QVERIFY(count >= 1);
}



QTEST_MAIN(TestAutoWorkshopSql)

#include "test_AutoWorkshopSql.moc"
