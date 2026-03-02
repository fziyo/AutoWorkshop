#include "AddEmpDialog.h"
#include "ui_AddEmpDialog.h"
#include "logger/Log.h"
#include <QMessageBox>
#include "dtos/EmployeeDto.h"

AddEmpDialog::AddEmpDialog(EmployeeService* employeeService, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddEmpDialog)
    , employeeService(employeeService)
{
    ui->setupUi(this);
    connect(ui->okButton, &QPushButton::clicked, this, &AddEmpDialog::onClickOKButton);
    connect(ui->cancelButton, &QPushButton::clicked, this, &AddEmpDialog::onClickCancelButton);
}

void AddEmpDialog::onClickOKButton()
{
    // check validation
    QString name = ui->nameLineEdit->text().trimmed();
    QString tel = ui->telLineEdit->text().trimmed();
    if (name == "" || tel == "")
    {
        ui->errorLabel->setText("Please fill all inputs.");
        return;
    }

    // store info to db
    EmployeeDto newEmp;
    newEmp.name = name; newEmp.tel = tel;
    if (!employeeService->addEmployee(newEmp))
    {
        QMessageBox::critical(this, "Fail to add employee.", employeeService->getError());
        return;
    }
    QMessageBox::information(this, "Success", "Employee added successfully.");
    accept();
}

void AddEmpDialog::onClickCancelButton()
{
    reject();
}

AddEmpDialog::~AddEmpDialog()
{
    delete ui;
}
