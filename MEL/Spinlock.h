#pragma once
#include <atomic>

class Spinlock {
private:
    typedef enum { Locked, Unlocked } LockState;
    std::atomic<LockState> state_;

public:
    Spinlock() : state_(Unlocked) {}

    void lock()
    {
        while (state_.exchange(Locked, std::memory_order_acquire) == Locked) {
            // busy wait
        }
    }
    void unlock()
    {
        state_.store(Unlocked, std::memory_order_release);
    }
};