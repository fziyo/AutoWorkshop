#ifndef TICKETESTIMATE_H
#define TICKETESTIMATE_H

struct TicketEstimate   // table ticket_estimate
{
    int ticketId;
    QString description;
    QString expecCost;
    int accepted;    // 0 - No  1-YES
    // QString pricePaid;   //price paid by the client: repair + parts

};
#endif // TICKETESTIMATE_H
