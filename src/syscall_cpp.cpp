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

void Thread::runWrapper(void *arg) {
    if (arg == nullptr) return;
    Thread *thread = (Thread *)arg;
    thread->run();
}
