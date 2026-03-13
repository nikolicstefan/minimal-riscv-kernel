#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include "List.hpp"
#include "TCB.hpp"

class TCB;

class Scheduler {
public:
    static TCB *get();

    static void put(TCB *tcb);

    static void suspend(TCB *tcb);

    static void updateSuspended();

private:
    class SuspendedQueue : public List<TCB> {
    public:
        void addOrdered(TCB *tcb);
    };

    static List<TCB> readyThreadsQueue;
    static SuspendedQueue suspendedThreadsQueue;
};

#endif // SCHEDULER_HPP
