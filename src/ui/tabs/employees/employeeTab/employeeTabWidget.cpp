#include "EmployeeTabWidget.h"
#include "ui_EmployeeTabWidget.h"
#include "ui/common/addEmpDialog/AddEmpDialog.h"
#include "logger/Log.h"
#include <QDialog>

EmployeeTabWidget::EmployeeTabWidget(EmployeeService* employeeService, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EmployeeTabWidget)
    , employeeService(employeeService)
{

    ui->setupUi(this);
    // only single row can be selected
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    refreshEmployeeTable();

    // click add button show add emp dialog
    connect(ui->addEmpButton, &QPushButton::clicked, this, &EmployeeTabWidget::onClickAddButton);
    // filter
    connect(ui->findEmpButton, &QPushButton::clicked, this, &EmployeeTabWidget::filterEmployee);
    connect(ui->empNameLineEdit, &QLineEdit::returnPressed,this, &EmployeeTabWidget::filterEmployee);


}

QStackedWidget* EmployeeTabWidget::getStack() const
{
    return ui->employeesStack;
}

void EmployeeTabWidget::onClickAddButton()
{
    AddEmpDialog addEmpDialog(employeeService, this);
    if (addEmpDialog.exec() == QDialog::Accepted)
    {
        addEmpDialog.close();
        refreshEmployeeTable();
    } else
    {
        addEmpDialog.close();
    }
}

void EmployeeTabWidget::refreshEmployeeTable()
{
    QList<Employee> employees = employeeService->getAllEmployees();
    displayOnEmployeeTable(employees);
}

void EmployeeTabWidget::displayOnEmployeeTable(const QList<Employee>& employees)
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(employees.size());
    // do not show number on left table side
    ui->tableWidget->verticalHeader()->setVisible(false);
    int row = 0;
    foreach (const Employee& emp, employees) {
        auto index = new QTableWidgetItem(QString::number(row+1));
        auto name = new QTableWidgetItem(emp.name);
        auto tel = new QTableWidgetItem(emp.tel);
        auto createTime = new QTableWidgetItem(emp.createTime);
        for (auto item : {name, tel, createTime})
        {
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        }
        ui->tableWidget->setItem(row, 0, index);
        ui->tableWidget->setItem(row, 1, name);
        ui->tableWidget->setItem(row, 2, tel);
        ui->tableWidget->setItem(row, 3, createTime);
        row++;
    }
}

void EmployeeTabWidget::filterEmployee()
{
    QString name = ui->empNameLineEdit->text();
    QList<Employee> filterRes = employeeService->filterByName(name);
    displayOnEmployeeTable(filterRes);
}

EmployeeTabWidget::~EmployeeTabWidget()
{
    delete ui;
}
