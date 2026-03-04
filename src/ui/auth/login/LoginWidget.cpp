#include "LoginWidget.h"
#include "ui_LoginWidget.h"
#include "log/Log.h"
#include <QDebug>

LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginWidget)
{
    ui->setupUi(this);
    ui->passwordInput->setEchoMode(QLineEdit::Password);  // password mode

    connect(ui->loginButton, &QPushButton::clicked, this, &LoginWidget::onLoginClicked);
    connect(ui->createAccountButton, &QPushButton::clicked, this, &LoginWidget::onCreateAccountClicked);
}

LoginWidget::~LoginWidget()
{
    delete ui;
}

void LoginWidget::onLoginClicked()
{
    qCInfo(logUi) << "Login button clicked!";
    setLoginErrorLabel("");
    const QString username = ui->usernameInput->text();
    const QString password = ui->passwordInput->text();
    const LoginResult loginResult = auth.login(username, password);

    if(!loginResult.ok || !loginResult.session.has_value())
    {
        setLoginErrorLabel(loginResult.error);
        qCCritical(logUi) << "Logging err: " << loginResult.error;
        return;
    }

    qCInfo(logUi) << "Login success!";
    emit loginSuccess(loginResult.session);

}

void LoginWidget::onCreateAccountClicked()
{
    emit switchToCreateAccountWidget();
}

void LoginWidget::setLoginErrorLabel(const QString& errorMsg)
{
    if(ui->errorLabel)
        ui->errorLabel->setText(errorMsg);
}
