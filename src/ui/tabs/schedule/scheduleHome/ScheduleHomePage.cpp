#include "ScheduleHomePage.h"
#include "ui_ScheduleHomePage.h"
#include "log/Log.h"
#include "entity/Ticket.h"
#include "ui/common/slotWidget/SlotWidget.h"
#include "utils/TicketStatus.h"

ScheduleHomePage::ScheduleHomePage(TicketService* ticketService, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ScheduleHomePage)
    , ticketService(ticketService)
{
    ui->setupUi(this);
    // headers stretch
    ui->scheduleTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->scheduleTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // cannot edit cell
    ui->scheduleTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // enable  to accept drop and event filte
    ui->scheduleTable->viewport()->installEventFilter(this);
    ui->scheduleTable->viewport()->setAcceptDrops(true);
    ui->scheduleTable->setDragEnabled(true);
    ui->scheduleTable->setAcceptDrops(true);
    ui->scheduleTable->setDropIndicatorShown(true);
    ui->scheduleTable->setDragDropMode(QAbstractItemView::InternalMove);
    ui->scheduleTable->setDragDropOverwriteMode(false);

    currentWeekStart = getCurrentWeekStart(QDate::currentDate());
    loadWeek(currentWeekStart);
    // week change
    connect(ui->PreviousWeekButton, &QPushButton::clicked, this, &ScheduleHomePage::onPressPreviousWeekButton);
    connect(ui->nextWeekButton, &QPushButton::clicked, this, &ScheduleHomePage::onPressNextWeekButton);
}


/**
 * @brief ScheduleTabWidget::loadWeek
 * @param currentWeekStart
 */
void ScheduleHomePage::loadWeek(const QDate& currentWeekStart)
{
    QDate currentWeekEnd = currentWeekStart.addDays(4);
    ui->weekLabel->setText(QString("%1 - %2")
                               .arg(currentWeekStart.toString("MMM dd"))
                               .arg(currentWeekEnd.toString("MMM dd, yyyy")));
    ui->scheduleTable->clearContents();
    QList<TicketDetailsDto> tickets = ticketService->getWeeklyTickets(currentWeekStart, currentWeekEnd);
    LOG_INFO(logUi) << "tickets count =" << tickets.size();
    //put ticket widget on table cell
    foreach (const TicketDetailsDto& ticket, tickets) {

        // Convert date and time to row and column indices
        LOG_INFO(logUi) <<"date: " << ticket.scheduleDate;
        int col = ticket.scheduleDate.dayOfWeek()-1; // monday 1
        LOG_INFO(logUi) <<"day of week" <<col;
        LOG_INFO(logUi) << "ticket status before update: " << ticketStatusToString(ticket.ticket.status);
        TicketDetailsDto updatedTicket = ticketService->refreshStatus(ticket);
        LOG_INFO(logUi) << "updatedTicket status: " << ticketStatusToString(updatedTicket.ticket.status);
        foreach (int slotIndex, updatedTicket.slotIndexes)
        {
            int row = slotIndex;
            LOG_INFO(logUi) <<"slotwidget: row" <<row;
            SlotWidget* slotInfo = new SlotWidget();
            slotInfo->setTicketInfo(updatedTicket);
            // check if there already has a ticket widget
            QWidget* existingWidget = ui->scheduleTable->cellWidget(row*2, col);
            LOG_INFO(logUi)<<"set ticket slot widget";
            if (existingWidget)
                // put to next
                ui->scheduleTable->setCellWidget(row*2+1, col, slotInfo);
            else
            {   // put
                ui->scheduleTable->setCellWidget(row*2, col, slotInfo);

            }
        }
    }

    // distribute space evenly
    ui->scheduleTable->resizeColumnsToContents();
    ui->scheduleTable->resizeRowsToContents();
    ui->scheduleTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->scheduleTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

}

/**
 * @brief ScheduleTabWidget::getCurrentWeekStart
 * @param date
 * @return QDate date of monday
 */
QDate ScheduleHomePage::getCurrentWeekStart(const QDate &date)
{
    int dayOfWeek = date.dayOfWeek();
    QDate startOfWeek = date.addDays(-((dayOfWeek + 6) % 7));
    return startOfWeek;
}

void ScheduleHomePage::onPressNextWeekButton()
{
    currentWeekStart = currentWeekStart.addDays(7);
    loadWeek(currentWeekStart);
}

void ScheduleHomePage::onPressPreviousWeekButton()
{
    currentWeekStart = currentWeekStart.addDays(-7);
    loadWeek(currentWeekStart);
}

ScheduleHomePage::~ScheduleHomePage()
{
    delete ui;
}
