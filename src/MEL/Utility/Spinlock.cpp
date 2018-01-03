#include <MEL/Utility/Spinlock.hpp>

namespace mel {

Spinlock::Spinlock() : state_(Unlocked) {

}

void Spinlock::lock()
{
    while (state_.exchange(Locked, std::memory_order_acquire) == Locked) {
        // busy wait
    }
}

void Spinlock::unlock()
{
    state_.store(Unlocked, std::memory_order_release);
}

} // namespace mel


