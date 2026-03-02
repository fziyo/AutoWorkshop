#include "EmployeeScheduleService.h"
#include "src/logger/Log.h"

EmployeeScheduleService::EmployeeScheduleService(AutoWorkshopSql* db):m_db(db)
{
    Q_ASSERT(m_db != nullptr);
}

EmpAvailability EmployeeScheduleService::checkEmployeeAvailability(const QString& empId, const QString& appointedDate, const QList<int>& timeSlots)
{
    LOG_DEBUG(logService)
        << "Check employee availability"
        << "empId=" << empId
        << "date=" << appointedDate;
    // TODO: no slot picked, return
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

    // TODO: emp available
    int conflicts = m_db->countScheduleConflicts(empId, appointedDate, timeSlots);
    if (conflicts > 0)
    {
        LOG_DEBUG(logService)
            << "Employee not available"
            << "empId=" << empId
            << "date=" << appointedDate
            << "conflicts=" << conflicts;
        return EmpAvailability::NotAvailable;
    }

    // TODO: emp unavailable
    LOG_DEBUG(logService)
        << "Employee available"
        << "empId=" << empId
        << "date=" << appointedDate;
    return EmpAvailability::Available;

}
