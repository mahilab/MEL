#ifndef MEL_MYO_HPP
#define MEL_MYO_HPP

#include <MEL/Daq/Input.hpp>  
#include "/Users/craigmcdonald/Documents/dev/myo-sdk-mac-0.9.0/myo/myo.hpp"
#include <MEL/Utility/NonCopyable.hpp>

namespace mel {

class MyoBand : public mel::Input<Voltage>, mel::NonCopyable {

public:

    MyoBand(const std::string& name);
    ~MyoBand();
    bool enable() override;
    bool disable() override;
    bool update() override;
    bool update_channel(uint32 channel_number) override;

private:

    class Impl;
    Impl* impl_;

};

}

#endif // MEL_MYO_HPP
