#include <MEL/Communications/MelShare.hpp>
#include <MEL/Utility/Types.hpp>
#include <MEL/Utility/Console.hpp>

namespace mel {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

MelShare::MelShare(const std::string& name, std::size_t max_bytes) :
    shm_(name, max_bytes),
    mutex_(name + "_mutex")
{
}

void MelShare::write_data(const std::vector<double>& data) {
    Lock lock(mutex_);
    uint32 size = static_cast<uint32>(data.size() * sizeof(double));
    shm_.write(&size, sizeof(uint32));
    shm_.write(&data[0], size, sizeof(uint32));
}

std::vector<double> MelShare::read_data() {
    Lock lock(mutex_);
    uint32 size = get_size();
    if (size > 0) {
        std::vector<double> data(size / sizeof(double));
        shm_.read(&data[0], size, sizeof(uint32));
        return data;
    }
    else
        return std::vector<double>();
}

void MelShare::write_message(const std::string &message) {
    Lock lock(mutex_);
    uint32 size = static_cast<uint32>(message.length() + 1);
    shm_.write(&size, sizeof(uint32));
    shm_.write(message.c_str(), size, sizeof(uint32));
}

std::string MelShare::read_message() {
    Lock lock(mutex_);
    uint32 size = get_size();
    if (size > 0) {
        std::vector<char> message(size);
        shm_.read(&message[0], size, sizeof(uint32));
        return std::string(&message[0]);
    }
    else
        return std::string();
}

uint32 MelShare::get_size() {
    uint32 size;
    shm_.read(&size, sizeof(uint32));
    return size;
}


} // namespace mel
