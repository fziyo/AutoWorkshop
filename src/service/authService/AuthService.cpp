#include "AuthService.h"

AuthService::AuthService(AutoWorkshopSql* db)
    : m_db(db)
{
}

LoginResult AuthService::login(const QString& username, const QString& password) const
{
    LoginResult loginResult;

    // validate form
    if(username.trimmed().isEmpty() || password.isEmpty())
    {
        loginResult.ok = false;
        loginResult.error = "Username or password cannot be empty.";
        return loginResult;
    }

    if(!m_db->isOpen())
    {
        loginResult.ok = false;
        loginResult.error = "Database err: " + m_db->getLastDbError() + ". Db is not open.";
        return loginResult;
    }

    // check if user exists in db and get id
    int userId = -1;
    if (!m_db->verifyUser(username, password, &userId))
    {
        loginResult.ok = false;
        loginResult.error = m_db->getLastDbError() + " verify user failed.";;
        return loginResult;
    }

    loginResult.ok = true;
    loginResult.session = UserSession {userId, username};
    return loginResult;
}

CreateAccountResult AuthService::createAccount(const QString& username, const QString& password)
{
    CreateAccountResult result;
    if (m_db->checkUserExist(username))
    {
        result.ok = false;
        result.error =
            "Account already exists, please log in.";

        return result;
    }

    if (m_db->createAccount(username, password))
    {
        result.ok = true;
    }
    else
    {
        result.ok = false;
        result.error = m_db->getLastDbError();
    }

    return result;
}

















