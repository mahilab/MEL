#pragma once
#include <atomic>

namespace mel {

class Spinlock {

public:

    Spinlock();

    void lock();
    void unlock();

private:

    typedef enum { Locked, Unlocked } LockState;
    std::atomic<LockState> state_;

};

} // namespace mel
