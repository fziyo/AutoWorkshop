#ifndef CREATETICKETWIDGET_H
#define CREATETICKETWIDGET_H

#include <QWidget>
#include "service/ticketService/TicketService.h"
#include "service/employeeService/EmployeeService.h"
#include "service/employeeScheduleService/EmployeeScheduleService.h"
#include <QListWidgetItem>
#include "ui/common/state/IUnsavedState.h"
namespace Ui {
class CreateTicketWidget;
}

class CreateTicketWidget : public QWidget, public IUnsavedState
{
    Q_OBJECT

public:
    explicit CreateTicketWidget(TicketService* ticketService, EmployeeService* employeeService, EmployeeScheduleService* employeeScheduleService, QWidget *parent = nullptr);
    void setUpEmployeeList();
    void refreshAvailability(QListWidgetItem* item);
    ~CreateTicketWidget();
    bool hasUnsavedChanges() const override;
    void clearState() override;
    bool validateField(const QString& value, const QString& message, QWidget* focusWidget);
    //void saveTicket();
    void cancelCreateTicket();
signals:
    void goToRootTab();

private:
    Ui::CreateTicketWidget *ui;
    TicketService* ticketService;
    EmployeeService* employeeService;
    EmployeeScheduleService* employeeScheduleService;
};

#endif // CREATETICKETWIDGET_H
