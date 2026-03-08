#include <QtTest/QtTest>
#include "service/authService/AuthService.h"
#include "data/AutoWorkshopSql.h"

class TestAuthService : public QObject
{
    Q_OBJECT

private:
    std::unique_ptr<AutoWorkshopSql> db;
    std::unique_ptr<AuthService> auth;

private slots:

    void initTestCase()
    {
        db = std::make_unique<AutoWorkshopSql>("test_auth.db");

        QVERIFY(db->openDb());
        QVERIFY(db->initSchema());

        auth = std::make_unique<AuthService>(db.get());
    }

    void cleanupTestCase()
    {
        db->close();
        QFile::remove("test_auth.db");
    }

    void testEmptyUsername()
    {
        auto result = auth->login("", "123");

        QVERIFY(!result.ok);
        QCOMPARE(result.error,
                 QString("Username or password cannot be empty."));
    }

    void testCreateAccountSuccess()
    {
        auto result = auth->createAccount("user1", "123");

        QVERIFY(result.ok);
    }

    void testCreateAccountDuplicate()
    {
        auth->createAccount("user2", "123");

        auto result = auth->createAccount("user2", "123");

        QVERIFY(!result.ok);
        QCOMPARE(result.error,
                 QString("Account already exists, please log in."));
    }

    void testLoginSuccess()
    {
        auth->createAccount("loginuser", "123");

        auto result = auth->login("loginuser", "123");

        QVERIFY(result.ok);
        QCOMPARE(result.session->username,
                 QString("loginuser"));
    }

    void testLoginWrongPassword()
    {
        auth->createAccount("user3", "123");

        auto result = auth->login("user3", "wrong");

        QVERIFY(!result.ok);
    }
};

QTEST_MAIN(TestAuthService)
#include "test_authService.moc"
