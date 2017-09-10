#pragma once
#include <MelShare.h>
#include <string>
#include <thread>

namespace mel {

    namespace util {

        class GuiFlag {

        private:

            mel::comm::MelShare flag_;
            std::array<int, 1> flag_value_;

        public:

            bool check_flag(int flag_value) {
                flag_.read(flag_value_);
                return flag_value == flag_value_[0];
            }

            void wait_for_flag(int flag_value) {
                while (!check_flag(flag_value)) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(0));
                }
            }

            void reset_flag(int flag_value) {
                flag_value_[0] = flag_value;
                flag_.write(flag_value_);
            }

            GuiFlag(std::string melshare_name, int initial_flag_value) : flag_(comm::MelShare(melshare_name)) {
                reset_flag(initial_flag_value);
            }

        };

    }

}