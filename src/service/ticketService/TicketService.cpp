#include "TicketService.h"
#include "utils/TimeSlotsProvider.h"
#include "log/Log.h"
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
QList<TicketDetailsDto> TicketService::getWeeklyTickets(const QDate& startDate, const QDate& endDate)
{
    return m_db->getWeeklyTickets(startDate, endDate);
}

/**
 * @brief TicketService::calculateStatus  calculate correct status based on current time
 * @param ticket
 * @param currentDate
 * @param currentTime
 * @return TicketStatus  the correct status
 */
TicketStatus TicketService::calculateStatus(const TicketDetailsDto& ticket, const QDate& currentDate, const QTime& currentTime)
{
    if(ticket.scheduleDate < currentDate.addDays(-7))
    {   // ticket one week ago - closed
        return TicketStatus::Closed;
    }

    if (ticket.scheduleDate > currentDate)
    {
        // future work - created
        return TicketStatus::Created;

    }

    if (ticket.scheduleDate == currentDate)
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
TicketDetailsDto TicketService::refreshStatus(const TicketDetailsDto& ticket)
{
    TicketStatus newStatus = calculateStatus(ticket, QDate::currentDate(), QTime::currentTime());
    LOG_DEBUG(logService) << "refreshStatus：：newStatus: " << ticketStatusToString(newStatus);
    if (newStatus != ticket.ticket.status) {
        LOG_DEBUG(logService) << "database update ticket status: " << ticketStatusToString(newStatus);
        bool updateRes = m_db->updateTicketStatus(ticket, newStatus);
        LOG_DEBUG(logService) << "database update ticket status result: " << updateRes;
        return m_db->getTicket(ticket.ticket.id);
    }
    LOG_DEBUG(logService) << "No need to update status ";
    return ticket;
}

/**
 * @brief TicketService::calculateTimeRange
 * @param ticket
 * @return QPair<QTime, QTime>  scheduled starting time and ending time, a tiket with multiple slots considered
 */
QPair<QTime, QTime>
TicketService::calculateTimeRange(const TicketDetailsDto& ticket)
{
    if (ticket.slotIndexes.isEmpty())
        return {};

    QList<int> ticketSlots = ticket.slotIndexes;
    std::sort(ticketSlots.begin(), ticketSlots.end());

    const auto& allTimeSlots = TimeSlotProvider::timeSlots();

    QTime startTime = allTimeSlots[ticketSlots.first()].startTime;
    QTime endTime   = allTimeSlots[ticketSlots.last()].endTime;

    return {startTime, endTime};
}

/**
 * @brief TicketService::updateTicketStatus
 * @param ticket
 * @param newStatus
 * @return bool
 */
bool TicketService::updateTicketStatus(const TicketDetailsDto& ticket, TicketStatus newStatus)
{
    return m_db->updateTicketStatus(ticket, newStatus);
}

bool TicketService::updateTicketStatusById(const int ticketId, const int newStatus)
{
    return m_db->updateTicketStatusById(ticketId, newStatus);
}



QList<TicketDetailsDto> TicketService::getAllTickets()
{
    return m_db->getAllTicketDetails();
}

void TicketService::setError(QString err)
{
    error = err;
}

QString TicketService::getError()
{
    return error;
}












