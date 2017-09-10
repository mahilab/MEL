#include "Encoder.h"

namespace mel {

    namespace core {

        Encoder::Encoder() :
            PositionSensor(),
            encoder_channel_(Daq::Encoder()),
            encrate_channel_(Daq::EncRate()),
            counts_per_unit_(0),
            count_(0.0),
            rate_(0.0)
        { }

        Encoder::Encoder(std::string name, double counts_per_unit, Daq::Encoder encoder_channel) :
            PositionSensor(name, false),
            counts_per_unit_(counts_per_unit),
            encoder_channel_(encoder_channel),
            count_(0.0),
            rate_(0.0)
        { }

        Encoder::Encoder(std::string name, double counts_per_unit, Daq::Encoder encoder_channel, Daq::EncRate encrate_channel) :
            PositionSensor(name, true),
            counts_per_unit_(counts_per_unit),
            encoder_channel_(encoder_channel),
            encrate_channel_(encrate_channel),
            count_(0.0),
            rate_(0.0)
        { }

        double Encoder::get_position() {
            count_ = encoder_channel_.get_count();
            position_ = count_ / (counts_per_unit_ *  encoder_channel_.get_quadrature_factor());
            return position_;
        }

        double Encoder::get_velocity() {
            if (velocity_enabled_) {
                rate_ = encrate_channel_.get_rate();
                velocity_ = rate_ / (counts_per_unit_ * encoder_channel_.get_quadrature_factor());
                return velocity_;
            }
            return PositionSensor::get_velocity(); // else call the default get_velocity() function
        }

    }

}