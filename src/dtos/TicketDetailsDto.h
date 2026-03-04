#ifndef TICKETDETAILSDTO_H
#define TICKETDETAILSDTO_H
#include "entity/Ticket.h"
#include <QString>
#include <QDate>
struct TicketDetailsDto
{
    Ticket ticket;
    QList<QString> employeeNames;
    QDate scheduleDate;
    QList<int> slotIndexes;
};

#endif // TICKETDETAILSDTO_H
