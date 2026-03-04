#include "SlotWidget.h"
#include "ui_SlotWidget.h"
#include "log/Log.h"
#include <QLayout>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>

SlotWidget::SlotWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SlotWidget)
{
    ui->setupUi(this);
    setAcceptDrops(true);
    infoLabel = new QLabel("Empty Slot", this);
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setWordWrap(true);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(infoLabel);
    setLayout(layout);
}

void SlotWidget::setTicketInfo(const TicketDetailsDto& ticket)
{   // p: 3d348b orange: f18701  red: f35b04 blue: 7678ed
    QString displayText = QString("Ticket ID: %1\nStatus: %2\nTechnician(s): %3")
                              .arg(ticket.ticket.id)
                              .arg(ticketStatusToString(ticket.ticket.status))
                              .arg(ticket.employeeNames.join(", "));
    infoLabel->setText(displayText);
    // add different background color according to status
    if (ticket.ticket.status == TicketStatus::Created)
        infoLabel->setStyleSheet("QLabel { background-color : #F9CBCB; border-radius: 5;}");
    if (ticket.ticket.status == TicketStatus::InProgress)
        infoLabel->setStyleSheet("QLabel { background-color : #BAE2D4; border-radius: 5; }");
    if (ticket.ticket.status == TicketStatus::Done)
        infoLabel->setStyleSheet("QLabel { background-color : #7678ed; border-radius: 5; }");
    if (ticket.ticket.status == TicketStatus::Closed)
        infoLabel->setStyleSheet("QLabel { background-color : #E5D3F1;  border-radius: 5;}");
}

QString SlotWidget::extractStatus(const QString &info)
{
    QStringList strList = info.split(", ");
    qDebug()<<strList[1];

    QString statusString = strList[1];
    QStringList parts = statusString.split(": ");
    return parts[1];
}

void SlotWidget::mouseMoveEvent(QMouseEvent *event) {
    if (!(event->buttons() & Qt::LeftButton))
        return;
    if ((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
        return;

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    QByteArray encodeData;

    QDataStream dataStream(&encodeData, QIODevice::WriteOnly);
    QPoint globalStartPos = this->mapToGlobal(dragStartPosition);

    dataStream <<globalStartPos<< ticketData.id;
    // << ticketData.brand << ticketData.timeSlots;
    mimeData->setData("application/x-ticket", encodeData);
    qDebug()<<"SlotWidget::mouseMoveEvent" << dragStartPosition;

    //qDebug()<<"SlotWidget::mouseMoveEvent"<< ticketData.id << ticketData.date << ticketData.timeSlots;
    drag->setMimeData(mimeData);
    QPixmap pixmap = this->grab(); // Optionally take a screenshot of the widget for the drag visual
    drag->setPixmap(pixmap);

    drag->exec(Qt::MoveAction|Qt::CopyAction );
}

void SlotWidget::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasFormat("application/x-ticket")) {
        dragStartPosition = event->position().toPoint();
        qCInfo(logUi) <<"SlotWidget::dragEnterEvent: "<< dragStartPosition;
        event->acceptProposedAction();
    }
}


SlotWidget::~SlotWidget()
{
    delete ui;
    delete infoLabel;
}
