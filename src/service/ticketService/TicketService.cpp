#include "TicketService.h"
#include "service/ticketService/TimeSlotsProvider.h"
#include "logger/Log.h"
#include <QDebug>
TicketService::TicketService(AutoWorkshopSql* db):m_db(db)
{

}

/**
 * @brief TicketService::getWeeklyTickets
 * @param startDate
 * @param endDate
 * @return QList<Ticket> a list of all tickets in the weedays, empty list if none
 */
QList<Ticket> TicketService::getWeeklyTickets(const QDate& startDate, const QDate& endDate)
{
    qDebug() << "m_db pointer:" << m_db;
    return m_db->getWeeklyTickets(startDate, endDate);
}

/**
 * @brief TicketService::calculateStatus  calculate correct status based on current time
 * @param ticket
 * @param currentDate
 * @param currentTime
 * @return TicketStatus  the correct status
 */
TicketStatus TicketService::calculateStatus(const Ticket& ticket, const QDate& currentDate, const QTime& currentTime)
{
    if(QDate::fromString(ticket.date, "yyyy-MM-dd") < currentDate.addDays(-7))
    {   // ticket one week ago - closed
        return TicketStatus::Closed;
    }

    if (QDate::fromString(ticket.date, "yyyy-MM-dd") > currentDate)
    {
        // future work - created
        return TicketStatus::Created;

    }

    if (QDate::fromString(ticket.date, "yyyy-MM-dd") == currentDate)
    {   // today's work

        auto [start, end] = calculateTimeRange(ticket);
        if (end <= currentTime)
        {
            return TicketStatus::Created;
        }
        if (start <= currentTime)
        {
            return TicketStatus::InProgress;
        }

        return TicketStatus::Created;

    }
    return TicketStatus::Done;
}

/**
 * @brief TicketService::refreshStatus change status based on current time
 * @param ticket
 * @return Ticket with updated status
 */
Ticket TicketService::refreshStatus(const Ticket& ticket)
{
    TicketStatus newStatus = calculateStatus(ticket, QDate::currentDate(), QTime::currentTime());

    if (newStatus != ticket.status) {
        m_db->updateTicketStatus(ticket, newStatus);
    }

    return m_db->getTicket(ticket.id);
}

/**
 * @brief TicketService::calculateTimeRange
 * @param ticket
 * @return QPair<QTime, QTime>  scheduled starting time and ending time, a tiket with multiple slots considered
 */
QPair<QTime, QTime> TicketService::calculateTimeRange(const Ticket& ticket)
{
    const auto& allTimeSlots = TimeSlotProvider::timeSlots();
    QTime endTime, startTime;
    for (auto& timeSlot : allTimeSlots)
    {
        if (ticket.timeSlots[timeSlot.index] == 1)
        {
            if (!startTime.isValid())
            {
                startTime = timeSlot.start;
            }
            endTime = timeSlot.end;
        }
    }
    return {startTime, endTime};
}

/**
 * @brief TicketService::updateTicketStatus
 * @param ticket
 * @param newStatus
 * @return bool
 */
bool TicketService::updateTicketStatus(const Ticket& ticket, TicketStatus newStatus)
{
    return m_db->updateTicketStatus(ticket, newStatus);
}

bool TicketService::updateTicketStatusById(const int ticketId, const int newStatus)
{
    return m_db->updateTicketStatusById(ticketId, newStatus);
}



QList<Ticket> TicketService::getAllTickets()
{
    return m_db->getAllTickets();
}

void TicketService::setError(QString err)
{
    error = err;
}

QString TicketService::getError()
{
    return error;
}












