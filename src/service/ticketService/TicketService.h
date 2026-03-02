#ifndef TICKETSERVICE_H
#define TICKETSERVICE_H
#include "data/AutoWorkshopSql.h"

class TicketService
{
public:
    TicketService(AutoWorkshopSql* db);
    QList<Ticket> getWeeklyTickets(const QDate& startDate, const QDate& endDate);
    TicketStatus calculateStatus(const Ticket& ticket, const QDate& today, const QTime& now);
    Ticket refreshStatus(const Ticket& ticket);
    QPair<QTime, QTime> calculateTimeRange(const Ticket& ticket);
    bool updateTicketStatus(const Ticket& ticket, TicketStatus newStatus);
    bool updateTicketStatusById(const int ticketId, const int newStatus);
    QList<Ticket> getAllTickets();
    void setError(QString err);
    QString getError();
private:
    AutoWorkshopSql* m_db;
    QString error;
};

#endif // TICKETSERVICE_H
