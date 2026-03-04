#ifndef TICKETSERVICE_H
#define TICKETSERVICE_H
#include "data/AutoWorkshopSql.h"

class TicketService
{
public:
    TicketService(AutoWorkshopSql* db);
    QList<TicketDetailsDto> getWeeklyTickets(const QDate& startDate, const QDate& endDate);
    TicketStatus calculateStatus(const TicketDetailsDto& ticket, const QDate& today, const QTime& now);
    TicketDetailsDto refreshStatus(const TicketDetailsDto& ticket);
    QPair<QTime, QTime> calculateTimeRange(const TicketDetailsDto& ticket);
    bool updateTicketStatus(const TicketDetailsDto& ticket, TicketStatus newStatus);
    bool updateTicketStatusById(const int ticketId, const int newStatus);
    QList<TicketDetailsDto> getAllTickets();
    void setError(QString err);
    QString getError();
private:
    AutoWorkshopSql* m_db;
    QString error;
};

#endif // TICKETSERVICE_H
