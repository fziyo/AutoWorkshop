#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>
#include "context/AppContext.h"

namespace Ui {
class LoginWidget;
}

class LoginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();

signals:
    void loginSuccess(const std::optional<UserSession>& );
    void switchToCreateAccountWidget();

private slots:
    void onLoginClicked();
    void onCreateAccountClicked();

private:
    Ui::LoginWidget *ui;
    void setLoginErrorLabel(const QString& errorMsg);
};

#endif // LOGINWIDGET_H
