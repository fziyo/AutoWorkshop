#include "TicketsHomePage.h"
#include "ui_TicketsHomePage.h"
#include "utils/TimeSlotsProvider.h"
#include <QMessageBox>
#include "utils/TabsPages.h"

TicketsHomePage::TicketsHomePage(TicketService* ticketService, EmployeeService* employeeService, EmployeeScheduleService* employeeScheduleService, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::TicketsHomePage)
    , ticketService(ticketService)
    , employeeService(employeeService)
    , employeeScheduleService(employeeScheduleService)
{
    ui->setupUi(this);

    // can only select row
    ui->ticketsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    // select only one row
    ui->ticketsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    // cannot edit cell
    ui->ticketsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->ticketsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->ticketsTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // get all tickets
    QList<TicketDetailsDto> tickets = ticketService->getAllTickets();
    displayAllTickets(tickets);
    // show create ticket page
    connect(ui->createTicketButton, &QPushButton::clicked, this, &TicketsHomePage::onCreateClicked);
}

void TicketsHomePage::displayAllTickets(const QList<TicketDetailsDto>& tickets )
{
    // tableWidget_emp in ui for displaying the data:
    ui->ticketsTable->clearContents();
    ui->ticketsTable->setRowCount(tickets.size());

    // set status combobox
    const QStringList texts = {"Created", "In progress", "Done", "Closed"};

    int row = 0;
    foreach (const TicketDetailsDto &ticket, tickets) {
        QComboBox* combobox = new QComboBox;
        combobox->addItems(texts);
        combobox->setCurrentIndex(ticketStatusToInt(ticket.ticket.status));

        ui->ticketsTable->setItem(row, 0, new QTableWidgetItem(QString::number(ticket.ticket.id)));
        ui->ticketsTable->setCellWidget(row, 1, combobox);
        ui->ticketsTable->setItem(row, 2, new QTableWidgetItem(ticket.ticket.resgisId));
        ui->ticketsTable->setItem(row, 3, new QTableWidgetItem(ticket.ticket.customer));
        ui->ticketsTable->setItem(row, 4, new QTableWidgetItem(ticket.scheduleDate.toString("yyyy-MM-dd")));

        // manually change ticket status, update database
        connect(combobox, &QComboBox::currentIndexChanged, this, &TicketsHomePage::updateTicketStatusById);

        QStringList timeStr;
        foreach (int slotIndex, ticket.slotIndexes)
        {
            timeStr.append(TimeSlotProvider::timeSlots().at(slotIndex).startTime.toString("hh:mm") + " - " + TimeSlotProvider::timeSlots().at(slotIndex).endTime.toString("hh:mm"));
        }
        QString timeString = timeStr.join(" ");

        ui->ticketsTable->setItem(row, 5, new QTableWidgetItem(timeString));

        ui->ticketsTable->setItem(row, 6, new QTableWidgetItem(ticket.employeeNames.join(", ")));
        row++;
    }
}

// update ticket status when user changes it
void TicketsHomePage::updateTicketStatusById(int newStatus)
{
    int row = ui->ticketsTable->currentRow();
    int ticketId = ui->ticketsTable->item(row, 0)->text().toInt();
    qDebug()<<"TicketsHomePage::updateTicketStatus id: " << ticketId << " to status: " << newStatus;

    bool res = ticketService->updateTicketStatusById(ticketId, newStatus);
    if (res)
        QMessageBox::information(nullptr, "Info", "Status changed!");
    else
        QMessageBox::information(nullptr, "Info", "Fail to change status!");

}


TicketsHomePage::~TicketsHomePage()
{
    delete ui;
}
