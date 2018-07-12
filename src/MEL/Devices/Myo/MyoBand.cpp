#include <MEL/Devices/Myo/MyoBand.hpp>
#include <array>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <MEL/Logging/Log.hpp>
#include <MEL/Utility/Mutex.hpp>
#include <thread>
#include <myo/myo.hpp>

namespace mel {

    class MyoBand::Impl : public myo::DeviceListener {

    public:

        Impl() :
            hub("com.mel.myo"),
            emgSamples(),
            stop_(false)
        {}

        ~Impl() {
            stop_ = true;
            thread_.join();
        }

        bool start() {
            try {
                LOG(mel::Info) << "Attempting to find a Myo ...";
                myo_ = hub.waitForMyo(10000);
                if (!myo_) {
                    LOG(mel::Warning) << "Unable to find a Myo";
                    return false;
                }
                LOG(mel::Info) << "Connected to a Myo";
                myo_->setStreamEmg(myo::Myo::streamEmgEnabled);
                hub.addListener(this);
                thread_ = std::thread(&Impl::io_thread_func, this);
                return true;
            }
            catch (const std::exception& e) {
                LOG(mel::Error) << "Myo Error: " << e.what();
                return false;
            }
        }

        void onUnpair(myo::Myo* myo, uint64_t timestamp) {
            emgSamples.fill(0);
        }

        void onEmgData(myo::Myo* myo, uint64_t timestamp, const int8_t* emg)
        {
            mel::Lock lock(mutex_);
            for (int i = 0; i < 8; i++) {
                emgSamples[i] = emg[i];
            }
        }

        // ... implement more virutal functions as needed

        int io_thread_func() {
            while (!stop_) {
                hub.run(1000 / 20);
            }
            return 0;
        }

        std::array<int8_t, 8> emgSamples;
        myo::Hub hub;
        myo::Myo* myo_;
        std::thread thread_;
        mel::Mutex mutex_;
        ctrl_bool stop_;

    };


    MyoBand::MyoBand(const std::string& name) :
        Input<Voltage>(name, { 0,1,2,3,4,5,6,7 }),
        impl_(new MyoBand::Impl)
    {

    }

    MyoBand::~MyoBand() {
        delete impl_;
    }

    bool MyoBand::enable() {
        if (impl_->start())
            return Device::enable();
        else
            return false;
    }

    bool MyoBand::disable() {
        return Device::disable();
    }

    bool MyoBand::update() {
        mel::Lock lock(impl_->mutex_);
        values_[0] = static_cast<double>(impl_->emgSamples[0]) * 0.001;
        values_[1] = static_cast<double>(impl_->emgSamples[1]) * 0.001;
        values_[2] = static_cast<double>(impl_->emgSamples[2]) * 0.001;
        values_[3] = static_cast<double>(impl_->emgSamples[3]) * 0.001;
        values_[4] = static_cast<double>(impl_->emgSamples[4]) * 0.001;
        values_[5] = static_cast<double>(impl_->emgSamples[5]) * 0.001;
        values_[6] = static_cast<double>(impl_->emgSamples[6]) * 0.001;
        values_[7] = static_cast<double>(impl_->emgSamples[7]) * 0.001;
        return true;
    }

    bool MyoBand::update_channel(uint32 channel_number) {
        mel::Lock lock(impl_->mutex_);
        values_[channel_number] = static_cast<double>(impl_->emgSamples[channel_number]) * 0.001;
        return true;
    }

}
