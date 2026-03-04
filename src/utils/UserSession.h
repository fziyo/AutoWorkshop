#ifndef USERSESSION_H
#define USERSESSION_H
#include <QObject>
#include <QString>
#include "data/AutoWorkshopSql.h"

struct UserSession
{
    int userId;
    QString username;
    // QString role;

};
#endif // USERSESSION_H
