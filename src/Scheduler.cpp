#include "../h/Scheduler.hpp"

#include "../h/print.hpp"

List<TCB> Scheduler::readyThreadsQueue;
Scheduler::SuspendedQueue Scheduler::suspendedThreadsQueue;

TCB *Scheduler::get() {
    return readyThreadsQueue.removeFirst();
}

void Scheduler::put(TCB *tcb) {
    readyThreadsQueue.addLast(tcb);
}

void Scheduler::suspend(TCB *tcb) {
    suspendedThreadsQueue.addOrdered(tcb);
}

void Scheduler::updateSuspended() {
    if (!suspendedThreadsQueue.isEmpty()) {
        suspendedThreadsQueue.peekFirst()->decreaseSleepInterval(1);
        while (!suspendedThreadsQueue.isEmpty() && suspendedThreadsQueue.peekFirst()->getSleepInterval() == 0) {
            put(suspendedThreadsQueue.removeFirst());
        }
    }
}

void Scheduler::SuspendedQueue::addOrdered(TCB *tcb) {
    Elem *prev, *curr;
    for (prev = nullptr, curr = head; curr != nullptr; prev = curr, curr = curr->next) {
        if (tcb->getSleepInterval() > curr->data->getSleepInterval()) {
            tcb->decreaseSleepInterval(curr->data->getSleepInterval());
        } else {
            curr->data->decreaseSleepInterval(tcb->getSleepInterval());
            break;
        }
    }
    Elem *elem = new Elem(tcb, curr);
    if (prev) prev->next = elem;
    else head = elem;
    if (!curr) tail = elem;
}
