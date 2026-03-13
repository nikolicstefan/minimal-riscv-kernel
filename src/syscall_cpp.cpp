#include "../h/syscall_cpp.hpp"

#include "../lib/hw.h"

void *operator new(size_t size) {
    return mem_alloc(size);
}

void *operator new[](size_t size) {
    return mem_alloc(size);
}

void operator delete(void *addr) noexcept {
    mem_free(addr);
}

void operator delete[](void *addr) noexcept {
    mem_free(addr);
}

Thread::Thread() : myHandle(nullptr), body(&runWrapper), arg(this) {}

Thread::Thread(void (*body)(void *), void *arg) : myHandle(nullptr), body(body), arg(arg) {
    thread_create(&myHandle, body, arg);
}

Thread::~Thread() {
    if (myHandle) {
        mem_free(myHandle);
    }
    myHandle = nullptr;
    body = nullptr;
    arg = nullptr;
}

int Thread::start() {
    if (!myHandle) {
        return thread_create(&myHandle, body, arg);
    } else {
        return -1;
    }
}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t period) {
    return time_sleep(period);
}

void Thread::runWrapper(void *arg) {
    if (arg == nullptr) return;
    Thread *thread = (Thread *)arg;
    thread->run();
}

Semaphore::Semaphore(unsigned init) : myHandle(nullptr) {
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore() {
    sem_close(myHandle);
    myHandle = nullptr;
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}

void PeriodicThread::terminate() {
    shouldTerminate = true;
}

PeriodicThread::PeriodicThread(time_t period) : Thread(), period(period), shouldTerminate(false) {}

void PeriodicThread::run() {
    while (!shouldTerminate) {
        periodicActivation();
        sleep(period);
    }
}

char Console::getc() {
    return ::getc();
}

void Console::putc(char c) {
    ::putc(c);
}
