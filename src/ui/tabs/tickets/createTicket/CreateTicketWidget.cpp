#include "CreateTicketWidget.h"
#include "ui_CreateTicketWidget.h"
#include "entity/Employee.h"
#include "log/Log.h"
#include <QMessageBox>

CreateTicketWidget::CreateTicketWidget(TicketService* ticketService, EmployeeService* employeeService, EmployeeScheduleService* employeeScheduleService, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CreateTicketWidget)
    , ticketService(ticketService)
    , employeeService(employeeService)
    , employeeScheduleService(employeeScheduleService)
{
    ui->setupUi(this);
    ui->dateInput->setDate(QDate::currentDate());
    setUpEmployeeList();
    // when employee checked, obtain availability
    connect(ui->employeeListWidget, &QListWidget::itemChanged, this, &CreateTicketWidget::refreshAvailability);

    // when cancle is clicked
    connect(ui->cancelButton, &QPushButton::clicked, this, &CreateTicketWidget::cancelCreateTicket);

    // ok is clicked
    //connect(ui->okButton, &QPushButton::clicked, this, &CreateTicketWidget::saveTicket);
}


void CreateTicketWidget::setUpEmployeeList()
{
    QList<Employee> emps = employeeService->getAllEmployees();
    qCInfo(logUi) << "Getting all employees success!";
    foreach (const Employee& emp, emps) {
        QListWidgetItem* listItem = new QListWidgetItem(emp.name, ui->employeeListWidget);
        listItem->setFlags(listItem->flags() | Qt::ItemIsUserCheckable);
        listItem->setCheckState(Qt::Unchecked);
        listItem->setData(Qt::UserRole, emp.id);
    }
}

void CreateTicketWidget::refreshAvailability(QListWidgetItem* item)
{
    if(item->checkState() == Qt::Checked) {
        // Handle the checked state
        LOG_INFO(logUi) << item->text() << " is checked.";
        // query if emp is available
        QDate selectedDate = ui->dateInput->date();
        LOG_INFO(logUi) << "Date input: " << selectedDate.toString("yyyy-MM-dd");
        QList<int> timeSlots;
        timeSlots.append(ui->checkBox1->isChecked() ? 1: 0);
        timeSlots.append(ui->checkBox2->isChecked() ? 1: 0);
        timeSlots.append(ui->checkBox3->isChecked() ? 1: 0);
        timeSlots.append(ui->checkBox4->isChecked() ? 1: 0);
        timeSlots.append(ui->checkBox5->isChecked() ? 1: 0);

        LOG_INFO(logUi) << "Check boxes states: " << timeSlots;
        QString empId = item->data(Qt::UserRole).toString();
        LOG_INFO(logUi) << "Selected employee id: " << empId;
        EmpAvailability availability = employeeScheduleService->checkEmployeeAvailability(empId, selectedDate, timeSlots);
        switch (availability)
        {
        case EmpAvailability::NoSlotSelected:
            QMessageBox::information(this, "Tip", "Please check at least 1 slot!");
            ui->employeeListWidget->blockSignals(true);
            item->setCheckState(Qt::Unchecked);
            ui->employeeListWidget->blockSignals(false);
            break;
        case EmpAvailability::PastTime:
            QMessageBox::information(this, "Tip", "Cannot select past time!");
            ui->employeeListWidget->blockSignals(true);
            item->setCheckState(Qt::Unchecked);
            ui->employeeListWidget->blockSignals(false);
            break;
        case EmpAvailability::Available:
            break;
        case EmpAvailability::NotAvailable:
            QMessageBox::information(this, "Info", "Staff not available!");
            ui->employeeListWidget->blockSignals(true);
            item->setCheckState(Qt::Unchecked);
            ui->employeeListWidget->blockSignals(false);
            break;
        }
    }
}

bool CreateTicketWidget::hasUnsavedChanges() const
{
    if (!ui->brandInput->text().isEmpty() || ui->checkBox1->isChecked() || ui->checkBox2->isChecked() || ui->checkBox3->isChecked() ||
        ui->checkBox4->isChecked() || ui->checkBox5->isChecked() || !ui->customerInput->text().isEmpty() || !ui->descriptionInput->toPlainText().isEmpty()
        || !ui->modelInput->text().isEmpty() || !ui->regisIdInput->text().isEmpty())
    {
        return true;
    }
    return false;
}

void CreateTicketWidget::clearState()
{
    ui->customerInput->clear();
    ui->brandInput->clear();
    ui->modelInput->clear();
    ui->regisIdInput->clear();

    ui->dateInput->clear();

    ui->checkBox1->setChecked(false);
    ui->checkBox2->setChecked(false);
    ui->checkBox3->setChecked(false);
    ui->checkBox4->setChecked(false);
    ui->checkBox5->setChecked(false);

    ui->employeeListWidget->clearSelection();
    for (int i = 0; i < ui->employeeListWidget->count(); ++i) {
        auto* item = ui->employeeListWidget->item(i);
        item->setCheckState(Qt::Unchecked);
    }
}

void CreateTicketWidget::cancelCreateTicket()
{
    if (hasUnsavedChanges() && QMessageBox::question(this, "Unsaved changes", "Discard current information?") == QMessageBox::No)
        return;
    clearState();
    emit goToRootTab();
}

bool CreateTicketWidget::validateField(const QString& value, const QString& message, QWidget* focusWidget)
{
    if (value.trimmed().isEmpty())
    {
        QMessageBox::warning(this, "Validation Error", message);
        if (focusWidget)
            focusWidget->setFocus();
        return false;
    }
    return true;
}

// void CreateTicketWidget::saveTicket()
// {
//     Ticket newTicket;

//     // Customer
//     if (!validateField(ui->customerInput->text(),
//                           "Customer name is required.",
//                           ui->customerInput))
//         return;

//     newTicket.customer = ui->customerInput->text().trimmed();


//     //Model
//     if (!validateField(ui->modelInput->text(),
//                           "Model is required.",
//                           ui->modelInput))
//         return;

//     newTicket.model = ui->modelInput->text().trimmed();


//     //Brand
//     if (!validateField(ui->brandInput->text(),
//                           "Brand is required.",
//                           ui->brandInput))
//         return;

//     newTicket.brand = ui->brandInput->text().trimmed();


//     //Description (optional)
//     newTicket.description = ui->descriptionInput->toPlainText().trimmed();


//     // Employee
//     QVariant employeeData = ui->employeeListWidget-;
//     if (!employeeData.isValid() || employeeData.toInt() <= 0)
//     {
//         QMessageBox::warning(this, "Validation Error",
//                              "Please select an employee.");
//         ui->employeeComboBox->setFocus();
//         return;
//     }

//     newTicket.employeeId = employeeData.toInt();


//     // Date
//     QDate selectedDate = ui->dateEdit->date();
//     if (!selectedDate.isValid())
//     {
//         QMessageBox::warning(this, "Validation Error",
//                              "Please select a valid date.");
//         ui->dateEdit->setFocus();
//         return;
//     }

//     newTicket.date = selectedDate;


//     // -------- Time Slots --------
//     QList<int> selectedSlots = getSelectedTimeSlots(); // 你自己的函数
//     if (selectedSlots.isEmpty())
//     {
//         QMessageBox::warning(this, "Validation Error",
//                              "Please select at least one time slot.");
//         return;
//     }

//     newTicket.timeSlots = selectedSlots;

// }

CreateTicketWidget::~CreateTicketWidget()
{
    delete ui;
}
