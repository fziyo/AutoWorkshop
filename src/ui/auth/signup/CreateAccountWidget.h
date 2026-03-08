#ifndef CREATEACCOUNTWIDGET_H
#define CREATEACCOUNTWIDGET_H

#include <QWidget>

namespace Ui {
class CreateAccountWidget;
}

class CreateAccountWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CreateAccountWidget(QWidget *parent = nullptr);
    ~CreateAccountWidget();

private slots:
    void onLoginButtonClicked();
    void onCreateAccountClicked();

signals:
    void switchToLoginWidget();

private:
    void setCreateAccountErrorLabel(const QString& error);
    Ui::CreateAccountWidget *ui;
};

#endif // CREATEACCOUNTWIDGET_H
