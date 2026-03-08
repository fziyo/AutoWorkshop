#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ui/auth/login/LoginWidget.h"
#include "ui/auth/signup/CreateAccountWidget.h"
#include "context/AppContext.h"
#include <QStackedLayout>
#include <QDebug>
#include <QMouseEvent>
#include "log/Log.h"
#include "utils/TabsPages.h"
#include "ui/common/navigation/ILeaveGuard.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // initalize db and services
    AppContext::instance().init();
    tabNavigationController = new TabNavigationController(ui->tabWidget, this);
    ui->tabWidget->tabBar()->installEventFilter(this);
    ui->stackedWidget->setCurrentWidget(ui->pageLoginSignup);

    // create a stackedwidget with parent logInUpHolder ***注意designer里logInUpHolder不能有任何布局,否则stack布局无效***
    auto loginupStack = new QStackedLayout(ui->logInUpHolder);
    // crate a login widget
    auto loginWidget = new LoginWidget(ui->logInUpHolder);
    // create a create account widget
    auto createAccountWidget = new CreateAccountWidget(ui->logInUpHolder);

    loginupStack->addWidget(loginWidget); // index0
    loginupStack->addWidget(createAccountWidget); // index1
    loginupStack->setCurrentWidget(loginWidget);
    // switch to create accoutn widget
    connect(loginWidget, &LoginWidget::switchToCreateAccountWidget, this, [loginupStack, createAccountWidget](){
        loginupStack->setCurrentWidget(createAccountWidget);
        qCInfo(logUi) << "Switched to createAccountWidget.";
    });

    // switch to login widget
    connect(createAccountWidget, &CreateAccountWidget::switchToLoginWidget, this, [loginupStack, loginWidget](){
        loginupStack->setCurrentWidget(loginWidget);
        qCInfo(logUi) << "Switched to loginWidget.";
    });

    // on success login, switch to main page: load all tabs; show main page -> schedule
    connect(loginWidget, &LoginWidget::loginSuccess, this, [this](const std::optional<UserSession>& session){
        AppContext::instance().setSession(session);
        loadAllTabs();
        setFirstTabPage();
        ui->stackedWidget->setCurrentWidget(ui->pageMain);

    });

    // tickets->reload();

}

void MainWindow::loadAllTabs()
{
    ui->tabWidget->clear();
    scheduleTab = new ScheduleTabWidget(AppContext::instance().getTicketService(), ui->tabWidget);
    ticketsTab = new TicketsTabWidget(AppContext::instance().getTicketService(), AppContext::instance().getEmployeeService(), AppContext::instance().getEmployeeScheduleService(), ui->tabWidget);
    employeeTab = new EmployeeTabWidget(AppContext::instance().getEmployeeService(), ui->tabWidget);
    ui->tabWidget->addTab(scheduleTab, "Schedule");
    ui->tabWidget->addTab(ticketsTab, "Tickets");
    ui->tabWidget->addTab(employeeTab, "Employee");
    tabNavigationController->registerTabs(Tabs::TAB_SCHEDULE, scheduleTab->getStack());
    tabNavigationController->registerTabs(Tabs::TAB_TICKETS, ticketsTab->getStack());
    tabNavigationController->registerTabs(Tabs::TAB_EMPLOYEE, employeeTab->getStack());
    LOG_INFO(logUi) << "All 3 tabs loaded successfully.";

}

void MainWindow::setFirstTabPage()
{
    ui->tabWidget->setCurrentWidget(scheduleTab);
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == ui->tabWidget->tabBar() && event->type() == QEvent::MouseButtonPress)
    {
        auto* mouseEvent = static_cast<QMouseEvent*>(event);
        int clickedTabIndex = ui->tabWidget->tabBar()->tabAt(mouseEvent->pos());
        qCInfo(logUi) << "tabWidget index clicked: " << clickedTabIndex;
        int currentPageIndex = ui->tabWidget->currentIndex();
        qCInfo(logUi) << "tabWidget current page index: " << currentPageIndex;
        // whether cureent tab widget is tickets tab
        auto* leaveGuard = dynamic_cast<ILeaveGuard*>(ui->tabWidget->widget(currentPageIndex));
        if (leaveGuard && currentPageIndex == TicketsTabPages::TICKETS_TAB_CREATE && clickedTabIndex != Tabs::TAB_TICKETS)
        {
            if (!leaveGuard->canLeave())
            {
                return true;  // will not switch tab
            }

            leaveGuard->leaveAndClear();
        }
    }
    return QMainWindow::eventFilter(watched, event);  // switch tab
}

MainWindow::~MainWindow()
{
    delete ui;
}



