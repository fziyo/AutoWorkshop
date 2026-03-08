#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include <QString>
#include "service/authService/AuthTypes.h"

// struct AuthResult
// {
//     bool ok = false;
//     std::optional<UserSession> session;  // when logged in
//     QString authError;
// };

class AuthService
{
public:
    AuthService(AutoWorkshopSql* db);
    LoginResult login(const QString& username, const QString& password) const;
    CreateAccountResult createAccount(const QString& username, const QString& password);

private:
    AutoWorkshopSql* m_db;
};

#endif // AUTHSERVICE_H
