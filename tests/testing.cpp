#include <atomic>
#include <MEL/Utility/Console.hpp>
#include <vector>

using namespace mel;

int main() {
    std::atomic<std::size_t> head;
    print(head.is_lock_free());
    return 0;
}
