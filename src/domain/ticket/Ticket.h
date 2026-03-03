#ifndef TICKET_H
#define TICKET_H
#include "domain/ticket/TicketStatus.h"
struct Ticket   // table ticket
{
    int id;
    QString customer;
    QString brand;
    QString model;
    QString resgisId;
    QStringList empNames;
    QString date;
    QList<int> timeSlots;     // size = 5
    QString description;
    TicketStatus status;     // 0  created/1 in progress/2 done/3 closed
    double totalToPay;
};
#endif // TICKET_H

