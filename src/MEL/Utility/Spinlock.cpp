#include <MEL/Utility/Spinlock.hpp>

namespace mel {

void Spinlock::lock() {
    while(lock_.test_and_set(std::memory_order_acquire)) {
        ; // busy wait
    }
}

void Spinlock::unlock() {
    lock_.clear(std::memory_order_release);
}

} // namespace mel


