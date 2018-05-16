#include <mutex>
#include <MEL/Utility/NamedMutex.hpp>

using namespace mel;

int main(int argc, char* argv[]) {
    NamedMutex mutex("evan");
    std::lock_guard<NamedMutex> lock(mutex);
    return 0;
}
