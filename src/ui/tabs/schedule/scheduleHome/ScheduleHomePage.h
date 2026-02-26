#ifndef SCHEDULEHOMEPAGE_H
#define SCHEDULEHOMEPAGE_H

#include <QWidget>
#include <QDate>
#include <QStackedWidget>
#include "service/ticketService/TicketService.h"

namespace Ui {
class ScheduleHomePage;
}

class ScheduleHomePage : public QWidget
{
    Q_OBJECT

public:
    explicit ScheduleHomePage(TicketService* ticketService, QWidget *parent = nullptr);
    ~ScheduleHomePage();
    void loadWeek(const QDate &startDate);
    QDate getCurrentWeekStart(const QDate &date);
    void onPressNextWeekButton();
    void onPressPreviousWeekButton();
    updateTicketStatus();

private:
    Ui::ScheduleHomePage *ui;
    TicketService* ticketService;
    QDate currentWeekStart;
};

#endif // SCHEDULEHOMEPAGE_H
