#include "Log.h"
#include <QDateTime>
#include <QFile>
#include <QTextStream>

Q_LOGGING_CATEGORY(logApp, "app")
Q_LOGGING_CATEGORY(logUi, "ui")
Q_LOGGING_CATEGORY(logAuth, "service")
Q_LOGGING_CATEGORY(logDb, "app.db")

void Log::init(bool enableDebug)
{
    QString rules;

    if (enableDebug)
    {
        rules = "*.debug=true\n"
                "*.info=true\n";
    }
    else
    {
        rules = "*.debug=false\n"
                "*.info=true\n";
    }

    QLoggingCategory::setFilterRules(rules);

    qSetMessagePattern(
        "%{time yyyy-MM-dd hh:mm:ss.zzz} "
        "[%{type}] "
        "[%{category}] "
        "%{file}:%{function}：%{line} - "
        "%{message}"
        );
}
