#include "CreateAccountWidget.h"
#include "ui_CreateAccountWidget.h"
#include "service/authService/AuthService.h"
#include "log/Log.h"
#include <QMessageBox>

CreateAccountWidget::CreateAccountWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CreateAccountWidget)
{
    ui->setupUi(this);
    connect(ui->createButton, &QPushButton::clicked, this, &CreateAccountWidget::onCreateAccountClicked);
    connect(ui->loginButton, &QPushButton::clicked, this, &CreateAccountWidget::onLoginButtonClicked);
}

CreateAccountWidget::~CreateAccountWidget()
{
    delete ui;
}

void CreateAccountWidget::onLoginButtonClicked()
{
    emit switchToLoginWidget();
}

void CreateAccountWidget::onCreateAccountClicked()
{
    qCInfo(logUi) << "Create account button clicked!";
    setCreateAccountErrorLabel("");
    const QString username = ui->usernameInput->text();
    const QString password = ui->passwordInput->text();
    const CreateAccountResult createAccountResult = auth.createAccount(username, password);

    if(!createAccountResult.ok)
    {
        setCreateAccountErrorLabel(createAccountResult.error);
        qDebug() << createAccountResult.error;
        return;
    }

    qCInfo(logUi) << "Create account success!";
    QMessageBox::information(this, "Success", "Account created, please log in.");
    emit switchToLoginWidget();
}

void CreateAccountWidget::setCreateAccountErrorLabel(const QString& error)
{
    if (ui->errorLabel)
        ui->errorLabel->setText(error);
}
