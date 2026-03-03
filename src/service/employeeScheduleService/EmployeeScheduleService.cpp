#include "EmployeeScheduleService.h"
#include "src/logger/Log.h"
#include <QDateTime>
#include <QDate>
#include "src/domain/timeSlot/TimeSlotsProvider.h"

EmployeeScheduleService::EmployeeScheduleService(AutoWorkshopSql* db):m_db(db)
{
    Q_ASSERT(m_db != nullptr);
}

EmpAvailability EmployeeScheduleService::checkEmployeeAvailability(const QString& empId, const QDate& appointedDate, const QList<int>& timeSlots)
{
    LOG_DEBUG(logService)
        << "Check employee availability"
        << "empId=" << empId
        << "date=" << appointedDate;
    // no slot picked, return
    bool slotsPicked = false;
    for (int timeSlot : timeSlots)
    {
        if (timeSlot == 1)
        {
            slotsPicked = true;
            break;
        }
    }
    if (!slotsPicked)
    {
        LOG_WARN(logService)
            << "No time slot selected"
            << "empId=" << empId
            << "date=" << appointedDate;
        return EmpAvailability::NoSlotSelected;
    }

    // check past time
    QDateTime now = QDateTime::currentDateTime();
    QDateTime earliestSelected;

    const auto& tSlots = TimeSlotProvider::timeSlots();

    for (int i = 0; i < timeSlots.size(); ++i)
    {
        if (timeSlots[i] == 1)
        {
            QDateTime slotStart(appointedDate, tSlots[i].startTime);
            LOG_DEBUG(logService)
                << "slot index=" << i
                << "start=" << tSlots[i].startTime;

            if (!earliestSelected.isValid() || slotStart < earliestSelected)
            {
                earliestSelected = slotStart;
            }
        }
    }

    if (earliestSelected.isValid() && earliestSelected < now)
    {
        LOG_WARN(logService)
        << "Selected time is in the past"
        << "empId=" << empId
        << "date=" << appointedDate;

        return EmpAvailability::PastTime;
    }

    // TODO: emp available
    int conflicts = m_db->countScheduleConflicts(empId, appointedDate, timeSlots);
    if (conflicts > 0)
    {
        LOG_DEBUG(logService)
            << "Employee not available"
            << "empId=" << empId
            << "date=" << appointedDate.toString("yyyy-MM-dd")
            << "conflicts=" << conflicts;
        return EmpAvailability::NotAvailable;
    }

    // TODO: emp unavailable
    LOG_DEBUG(logService)
        << "Employee available"
        << "empId=" << empId
        << "date=" << appointedDate.toString("yyyy-MM-dd");
    return EmpAvailability::Available;

}
