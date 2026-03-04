#include "TicketsTabWidget.h"
#include "ui_TicketsTabWidget.h"
#include "utils/TimeSlotsProvider.h"
#include <QMessageBox>
#include "utils/TabsPages.h"
TicketsTabWidget::TicketsTabWidget(TicketService* ticketService, EmployeeService* employeeService, EmployeeScheduleService* employeeScheduleService, QWidget *parent)
    : BaseTab(parent)
    , ui(new Ui::TicketsTabWidget)
    , ticketService(ticketService)
    , employeeService(employeeService)
    , employeeScheduleService(employeeScheduleService)
{
    ui->setupUi(this);

    ticketsHomePage = new TicketsHomePage(ticketService, employeeService, employeeScheduleService);
    createTicketWidget = new CreateTicketWidget(ticketService, employeeService, employeeScheduleService);

    getStack()->addWidget(ticketsHomePage);
    getStack()->addWidget(createTicketWidget);
    getStack()->setCurrentIndex(0);

    // receive signal to go to create ticket page
    connect(ticketsHomePage, &TicketsHomePage::onCreateClicked, this, [this]() {
        getStack()->setCurrentWidget(createTicketWidget);
    });
    // receive signal and switch to root
    connect(createTicketWidget, &CreateTicketWidget::goToRootTab, this, [this](){getStack()->setCurrentIndex(TicketsTabPages::TICKETS_TAB_MAIN);});
}

bool TicketsTabWidget::canLeave()
{
    return !(createTicketWidget && createTicketWidget->hasUnsavedChanges()) ||
           QMessageBox::question(this, "Unsaved changes", "Discard current information?") == QMessageBox::Yes;
}

void TicketsTabWidget::leaveAndClear()
{
    createTicketWidget->clearState();
}

TicketsTabWidget::~TicketsTabWidget()
{
    delete ui;
}
