#ifndef TICKETSTATUS_H
#define TICKETSTATUS_H
enum class TicketStatus {
    Created = 0,
    InProgress = 1,
    Done = 2,
    Closed = 3
};

// enum -> DB
inline int ticketStatusToInt(TicketStatus status)
{
    return static_cast<int>(status);
}

// DB -> enum
inline TicketStatus intToTicketStatus(int val)
{
    switch (val) {
    case 0: return TicketStatus::Created;
    case 1: return TicketStatus::InProgress;
    case 2: return TicketStatus::Done;
    case 3: return TicketStatus::Closed;
    default:
        qWarning() << "Invalid TicketStatus from DB:" << val;
        return TicketStatus::Created;
    }
}

inline QString ticketStatusToString(TicketStatus status)
{
    switch (status) {
    case TicketStatus::Created:
        return "Created";
    case TicketStatus::InProgress:
        return "In Progress";
    case TicketStatus::Done:
        return "Done";
    case TicketStatus::Closed:
        return "Closed";
    default:
        return "Unknown";
    }
}


#endif // TICKETSTATUS_H
