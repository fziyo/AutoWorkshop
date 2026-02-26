#ifndef SCHEDULETABWIDGET_H
#define SCHEDULETABWIDGET_H

#include <QWidget>
#include <QStackedWidget>
#include "data/AutoWorkshopSql.h"
#include "service/ticketService/TicketService.h"
#include "ui/tabs/baseTab/BaseTab.h"
#include "ui/tabs/schedule/scheduleHome/ScheduleHomePage.h"

namespace Ui {
class ScheduleTabWidget;
}

class ScheduleTabWidget : public BaseTab
{
    Q_OBJECT

public:
    explicit ScheduleTabWidget(TicketService* ticketService, QWidget *parent = nullptr);
    ~ScheduleTabWidget();

private:
    Ui::ScheduleTabWidget *ui;
    ScheduleHomePage* scheduleHomePage;
};

#endif // SCHEDULETABWIDGET_H
