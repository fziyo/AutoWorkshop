#ifndef SLOTWIDGET_H
#define SLOTWIDGET_H

#include <QWidget>
#include <QLabel>
#include "entity/Ticket.h"
#include "dtos/TicketDetailsDto.h"
namespace Ui {
class SlotWidget;
}

class SlotWidget : public QWidget
{
    Q_OBJECT


public:
    explicit SlotWidget(QWidget *parent = nullptr);
    ~SlotWidget();
    //static QPoint dragStartPosition;

public:
    void setTicketInfo(const TicketDetailsDto &);

    QString extractStatus(const QString &info);

protected:
    // void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event)override;
private:
    Ui::SlotWidget *ui;
    QLabel *infoLabel;
    QPoint dragStartPosition;
    Ticket ticketData;


};

#endif // SLOTWIDGET_H
