#ifndef MEL_MYO_HPP
#define MEL_MYO_HPP

#include <MEL/Daq/Input.hpp>
#include <MEL/Core/NonCopyable.hpp>

namespace mel {

class MEL_API MyoBand : public mel::Input<Voltage>, mel::NonCopyable {

public:

    MyoBand(const std::string& name);
    ~MyoBand();

    bool update() override;
    bool update_channel(uint32 channel_number) override;

private:

    bool on_enable() override;
    bool on_disable() override;

private:

    class Impl;
    Impl* impl_;

};

}

#endif // MEL_MYO_HPP
