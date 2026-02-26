#include "ScheduleTabWidget.h"
#include "ui_ScheduleTabWidget.h"
#include "app/AppContext.h"


ScheduleTabWidget::ScheduleTabWidget(TicketService* ticketService, QWidget *parent)
    : BaseTab(parent)
    , ui(new Ui::ScheduleTabWidget)
{
    ui->setupUi(this);
    scheduleHomePage = new ScheduleHomePage(ticketService);
    getStack()->addWidget(scheduleHomePage);
    getStack()->setCurrentIndex(0);
}


ScheduleTabWidget::~ScheduleTabWidget()
{
    delete ui;
}


