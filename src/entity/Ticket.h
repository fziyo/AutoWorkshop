#ifndef TICKET_H
#define TICKET_H
#include "utils/TicketStatus.h"
struct Ticket
{
    int id;
    QString customer;
    QString brand;
    QString model;
    QString resgisId;
    QString description;
    TicketStatus status;     // 0  created/1 in progress/2 done/3 closed
    double totalToPay;
    QString createdAt;
};
#endif // TICKET_H

