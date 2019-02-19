#include <MEL/Daq/Encoder.hpp>
#include <MEL/Logging/Log.hpp>

namespace mel {

//==============================================================================
// MODULE DEFINITIONS
//==============================================================================

Encoder::Encoder() :
    Module(),
    factors_(this, X4),
    units_per_count_(this, 1.0),
    positions_(this),
    conversions_(this)
{
    compute_conversions();
}

Encoder::Encoder(const ChanNums& channel_numbers) :
    Module<int>(channel_numbers),
    factors_(this, X4),
    units_per_count_(this, 1.0),
    positions_(this),
    conversions_(this)
{
    compute_conversions();
}

Encoder::~Encoder() {

}

bool Encoder::on_enable() {
    compute_conversions();
    return true;
}

bool Encoder::reset_counts(const std::vector<int>& counts) {
    if (validate_channel_count(counts.size())) {
        auto ch_nums = get_channel_numbers();
        for (std::size_t i = 0; i < ch_nums.size(); ++i) {
            reset_count(ch_nums[i], counts[i]);
        }
    }
    return true;
}

bool Encoder::reset_count(ChanNum channel_number, int count) {
    if (validate_channel_number(channel_number)) {
        values_[channel_number] = count;
        return true;
    }
    return false;
}


bool Encoder::set_quadrature_factors(const std::vector<QuadFactor>& factors) {
    factors_.set(factors);
    compute_conversions();
    return true;
}

bool Encoder::set_quadrature_factor(ChanNum channel_number, QuadFactor factor) {
    if (validate_channel_number(channel_number)) {
        factors_[channel_number] = factor;
        compute_conversions();
        return true;
    }
    return false;
}

bool Encoder::zero() {
    std::vector<int> zero(get_channel_count(), 0);
    return reset_counts(zero);
}

bool Encoder::zero_channel(ChanNum channel_number) {
    int zero = 0;
    return reset_count(channel_number, zero);
}

void Encoder::set_units_per_count(const std::vector<double>& units_per_count) {
    units_per_count_.set(units_per_count);
    compute_conversions();
}

void Encoder::set_units_per_count(ChanNum channel_number, double units_per_count) {
    if (validate_channel_number(channel_number)) {
        units_per_count_[channel_number] = units_per_count;
        compute_conversions();
    }
}

const std::vector<double>& Encoder::get_positions() {
    for (auto const& ch : get_channel_numbers())
        positions_[ch] = values_[ch] * conversions_[ch];
    return positions_.get();
}

double Encoder::get_position(ChanNum channel_number) {
    if (validate_channel_number(channel_number)) {
        return values_[channel_number] * conversions_[channel_number];
    }
    else
        return double();
}

Encoder::Channel Encoder::get_channel(ChanNum channel_number) {
    if (validate_channel_number(channel_number))
        return Channel(this, channel_number);
    else
        return Channel();
}

std::vector<Encoder::Channel> Encoder::get_channels(const ChanNums& channel_numbers) {
    std::vector<Channel> channels;
    for (std::size_t i = 0; i < channel_numbers.size(); ++i)
        channels.push_back(get_channel(channel_numbers[i]));
    return channels;
}

Encoder::Channel Encoder::operator[](ChanNum channel_number) {
    return get_channel(channel_number);
}

std::vector<Encoder::Channel> Encoder::operator[](const ChanNums& channel_numbers) {
    return get_channels(channel_numbers);
}

void Encoder::compute_conversions() {
    for (std::size_t i = 0; i < get_channel_count(); ++i) {
        ChanNum ch = get_channel_numbers()[i];
        conversions_[ch] = units_per_count_[ch] / static_cast<double>(factors_[ch]);
    }
}

//==============================================================================
// CHANNEL DEFINITIONS
//==============================================================================

Encoder::Channel::Channel() :
    ChannelBase()
{ }

Encoder::Channel::Channel(Encoder* module, ChanNum channel_number) :
    ChannelBase(module, channel_number)
{ }

double Encoder::Channel::get_position() {
    position_ = static_cast<Encoder*>(module_)->get_position(channel_number_);
    return position_;
}

bool Encoder::Channel::zero() {
    return static_cast<Encoder*>(module_)->zero_channel(channel_number_);
}

bool Encoder::Channel::reset_count(int count) {
    return static_cast<Encoder*>(module_)->reset_count(channel_number_, count);
}

bool Encoder::Channel::set_quadrature_factor(QuadFactor factor) {
    return static_cast<Encoder*>(module_)->set_quadrature_factor(channel_number_, factor);
}

void Encoder::Channel::set_units_per_count(double units_per_count) {
    static_cast<Encoder*>(module_)->set_units_per_count(channel_number_, units_per_count);
}

} // namespace mel
