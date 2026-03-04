#ifndef TICKETSHOMEPAGE_H
#define TICKETSHOMEPAGE_H

#include <QWidget>
#include "service/ticketService/TicketService.h"
#include "service/employeeService/EmployeeService.h"
#include "service/employeeScheduleService/EmployeeScheduleService.h"

namespace Ui {
class TicketsHomePage;
}

class TicketsHomePage : public QWidget
{
    Q_OBJECT

public:
    explicit TicketsHomePage(TicketService* ticketService, EmployeeService* employeeService, EmployeeScheduleService* employeeScheduleService, QWidget *parent = nullptr);
    ~TicketsHomePage();
    void displayAllTickets(const QList<TicketDetailsDto>& tickets );
    void updateTicketStatusById(int newStatus);
signals:
    void onCreateClicked();
private:
    Ui::TicketsHomePage *ui;
    TicketService* ticketService;
    EmployeeService* employeeService;
    EmployeeScheduleService* employeeScheduleService;
};

#endif // TICKETSHOMEPAGE_H
