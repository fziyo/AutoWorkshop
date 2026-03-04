#include "AuthService.h"
#include "context/AppContext.h"

// AuthService::AuthService() {}
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
    // access db
    auto& db = AppContext::instance().getDb();

    if(!db.isOpen())
    {
        loginResult.ok = false;
        loginResult.error = "Database err: " + db.getLastDbError() + ". Db is not open.";
        return loginResult;
    }

    // check if user exists in db and get id
    int userId = -1;
    if (!db.verifyUser(username, password, &userId))
    {
        loginResult.ok = false;
        loginResult.error = db.getLastDbError() + " verify user failed.";;
        return loginResult;
    }

    loginResult.ok = true;
    loginResult.session = UserSession {userId, username};
    return loginResult;
}

CreateAccountResult AuthService::createAccount(const QString& username, const QString& password)
{
    CreateAccountResult createAccountResult;
    auto& db = AppContext::instance().getDb();
    if (!db.checkUserExist(username))
    {
        if (db.createAccount(username, password))
        {
            createAccountResult.ok = true;
        } else {
            createAccountResult.ok = false;
            createAccountResult.error = db.getLastDbError();
        }

    } else {
        createAccountResult.ok = false;
        createAccountResult.error = "Account already exists, please log in.";
    }

    return createAccountResult;
}

















