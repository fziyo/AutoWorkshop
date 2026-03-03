#ifndef TIMESLOTSPROVIDER_H
#define TIMESLOTSPROVIDER_H

#include <QTime>
#include <QVector>

struct TimeSlot
{
    int index;
    QTime startTime;
    QTime endTime;
};

class TimeSlotProvider
{
public:
    static const QVector<TimeSlot>& timeSlots()
    {
        static QVector<TimeSlot> timeSlots = {
            {0, QTime(9,30), QTime(11,0)},
            {1, QTime(11,0), QTime(12,30)},
            {2, QTime(12,30),QTime(14,0)},
            {3, QTime(14,0), QTime(15,30)},
            {4, QTime(15,30),QTime(17,0)}
        };
        return timeSlots;
    }
};

#endif // TIMESLOTSPROVIDER_H
