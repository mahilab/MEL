#include <MEL/Communications/MelNet.hpp>
#include <MEL/Utility/Console.hpp>

namespace mel {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

MelNet::MelNet(unsigned int local_port, unsigned int remote_port,
               IpAddress remote_address, bool blocking) :
    local_port_(local_port),
    remote_port_(remote_port),
    remote_address_(remote_address)
{
    socket_.bind(local_port_);
    set_blocking(blocking);
}

void MelNet::send_data(const std::vector<double>& data) {
    packet_send_.clear();
    for (auto i = 0; i < data.size(); ++i)
        packet_send_ << data[i];
    socket_.send(packet_send_, remote_address_, remote_port_);
}

std::vector<double> MelNet::receive_data() {
    IpAddress sender;
    unsigned short port;
    if(socket_.receive(packet_receive_, sender, port) != Socket::NotReady) {
        std::size_t bytes = packet_receive_.get_data_size();
        std::size_t size = bytes / 8;
        std::vector<double> data(size);
        for (auto i = 0; i < size; ++i)
            packet_receive_ >> data[i];
        packet_receive_.clear();
        return data;
    } else {
        return std::vector<double>();
    }
}

void MelNet::send_message(const std::string& message) {
    packet_send_.clear();
    packet_send_ << message;
    socket_.send(packet_send_, remote_address_, remote_port_);
}

std::string MelNet::receive_message() {
    IpAddress sender;
    unsigned short port;
    if(socket_.receive(packet_receive_, sender, port) != Socket::NotReady) {
        std::string message;
        packet_receive_ >> message;
        packet_receive_.clear();
        return message;
    } else {
        return std::string();
    }
}

void MelNet::set_blocking(bool blocking) {
    socket_.set_blocking(blocking);
}

bool MelNet::is_blocking() const {
    return socket_.is_blocking();
}

} // namespace mel
