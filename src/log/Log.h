#ifndef LOG_H
#define LOG_H
#include <QLoggingCategory>

// app
Q_DECLARE_LOGGING_CATEGORY(logApp)
// ui
Q_DECLARE_LOGGING_CATEGORY(logUi)
// service
Q_DECLARE_LOGGING_CATEGORY(logService)
// data
Q_DECLARE_LOGGING_CATEGORY(logDb)

#define LOG_DEBUG(cat)    qCDebug(cat)
#define LOG_INFO(cat)     qCInfo(cat)
#define LOG_WARN(cat)     qCWarning(cat)
#define LOG_ERROR(cat)    qCCritical(cat)

class Log
{
public:
    static void init(bool enableDebug = true);
};
#endif // LOG_H
