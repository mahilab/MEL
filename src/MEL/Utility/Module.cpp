#include <MEL/Daq/Module.hpp>
#include <MEL/Logging/Log.hpp>

namespace mel {

ModuleBase::ModuleBase(const std::string& name, IoType type, const std::vector<uint32>& channel_numbers) :
    Device("Module::" + name),
    type_(type),
    channel_numbers_(sort_and_reduce_channels(channel_numbers)),
    channel_count_(channel_numbers_.size()),
    channel_map_(make_channel_map(channel_numbers_))
{ }

const std::vector<uint32>& ModuleBase::get_channel_numbers() const {
    return channel_numbers_;
}

std::size_t ModuleBase::get_channel_count() const {
    return channel_count_;
}

IoType ModuleBase::get_type() const {
    return type_;
}

bool ModuleBase::validate_channel_number(uint32 channel_number) const {
    if (channel_map_.count(channel_number) > 0)
        return true;
    LOG(Error) << "Invalid channel number " << channel_number
        << " not declared in channel numbers";
    return false;
}

bool ModuleBase::validate_channel_count(std::size_t size) const {
    if (channel_count_ == size)
        return true;
    LOG(Error) << "Invalid number of elements (" << size
        << ") not equal to channel count of " << channel_count_;
    return false;
}

std::vector<uint32> ModuleBase::sort_and_reduce_channels(const std::vector<uint32>& channels) {
    std::vector<uint32> sorted_channels = channels;
    std::sort(sorted_channels.begin(), sorted_channels.end());
    sorted_channels.erase(std::unique(sorted_channels.begin(), sorted_channels.end()), sorted_channels.end());
    return sorted_channels;
}

std::map<uint32, std::size_t> ModuleBase::make_channel_map(const std::vector<uint32>& channel_numbers) {
    std::map<uint32, std::size_t> channel_map;
    for (std::size_t i = 0; i < channel_numbers.size(); ++i)
        channel_map[channel_numbers[i]] = i;
    return channel_map;
}

} // namespace mel
