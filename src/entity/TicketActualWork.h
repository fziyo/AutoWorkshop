#ifndef TICKETACTUALWORK_H
#define TICKETACTUALWORK_H
struct TickActualWork
{
    int ticketId;
    int empId;
    QStringList empNames;
    QString date;
    QList<int> timeSlots;
};
#endif // TICKETACTUALWORK_H
