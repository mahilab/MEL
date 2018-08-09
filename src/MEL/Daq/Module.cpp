#include <MEL/Daq/Module.hpp>
#include <MEL/Logging/Log.hpp>
#include <algorithm>

namespace mel {

//==============================================================================
// HELPER FUNCTIONS
//==============================================================================

std::vector<uint32> sort_and_reduce_channels(const std::vector<uint32>& channels) {
    std::vector<uint32> sorted_channels = channels;
    std::sort(sorted_channels.begin(), sorted_channels.end());
    sorted_channels.erase(std::unique(sorted_channels.begin(), sorted_channels.end()), sorted_channels.end());
    if (sorted_channels != channels)
        LOG(Warning) << "Specifed channels " << channels << " sorted and reduced to " << sorted_channels;
    return sorted_channels;
}

std::map<uint32, std::size_t> make_channel_map(const std::vector<uint32>& channel_numbers) {
    std::map<uint32, std::size_t> channel_map;
    for (std::size_t i = 0; i < channel_numbers.size(); ++i)
        channel_map[channel_numbers[i]] = i;
    return channel_map;
}

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

ModuleBase::ModuleBase() { }

bool ModuleBase::update() {
    bool success = true;
    for (std::size_t i = 0; i < channel_numbers_.size(); ++i) {
        if (!update_channel(channel_numbers_[i]))
            success = false;
    }
    return success;
}

void ModuleBase::set_channel_numbers(const std::vector<uint32>& channel_numbers) {
    auto old_map = channel_map_;
    channel_numbers_ = sort_and_reduce_channels(channel_numbers);
    channel_map_ = make_channel_map(channel_numbers_);
    for (std::size_t i = 0; i < containers_.size(); i++)
        containers_[i]->change_channel_numbers(old_map, channel_map_);
}

const std::vector<uint32>& ModuleBase::get_channel_numbers() const {
    return channel_numbers_;
}

std::size_t ModuleBase::get_channel_count() const {
    return channel_numbers_.size();
}

std::size_t ModuleBase::index(uint32 channel_number) const {
    return channel_map_.at(channel_number);
}

bool ModuleBase::validate_channel_number(uint32 channel_number) const {
    if (channel_map_.count(channel_number) > 0)
        return true;
    LOG(Error) << "Invalid channel number " << channel_number
               << " not declared in channel numbers";
    return false;
}

bool ModuleBase::validate_channel_count(std::size_t size) const {
    if (channel_numbers_.size() == size)
        return true;
    LOG(Error) << "Invalid number of elements (" << size
               << ") not equal to channel count of " << channel_numbers_.size();
    return false;
}

void ModuleBase::add_buffer(BufferBase* container) {
    containers_.push_back(container);
}

} // namespace mel
