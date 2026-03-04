#ifndef AUTHTYPES_H
#define AUTHTYPES_H
#include <optional>
#include <QString>
#include "utils/UserSession.h"

struct LoginResult {
    bool ok = false;
    std::optional<UserSession> session; // when has value, user is loggedin
    QString error;
};

struct CreateAccountResult {
    bool ok = false;
    QString error;
};

#endif // AUTHTYPES_H
