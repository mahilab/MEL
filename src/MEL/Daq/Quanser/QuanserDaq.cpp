#include <MEL/Daq/Quanser/QuanserDaq.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Logging/Log.hpp>
#include <quanser_messages.h>
#include <hil.h>
#include <tchar.h>
#include <cstring>
#include <stdexcept>


namespace mel {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

QuanserDaq::QuanserDaq(const std::string& card_type, uint32 id, QuanserOptions options) :
    DaqBase(card_type + "_" + std::to_string(id)),
    card_type_(card_type),
    id_(id),
    options_(options)
{
}

bool QuanserDaq::on_open() {
    t_error result;
    // Try to open in 5 attempts
    for (int attempt = 0; attempt < 5; attempt++) {
        result = hil_open(card_type_.c_str(), std::to_string(id_).c_str(), &handle_);
        sleep(milliseconds(10));
        if (result == 0) {
            // successful open
            if (!set_options(options_)) {
                // options didn't take so close
                on_close();
                return false;
            }
            return true;
        }
        else {
            // unsuccesful open, continue
            LOG(Error) << "Failed to open " << get_name() << " (Attempt " << attempt + 1 << "/" << 5 << ") "
                       << get_quanser_error_message(result);
        }
    }
    // all attempts to open were unsuccessful
    LOG(Error) << "Exhausted all attempts to open " << get_name();
    return false;
}

bool QuanserDaq::on_close() {
    t_error result;
    result = hil_close(handle_);
    sleep(milliseconds(10));
    if (result == 0) {
        return true;
    }
    else {
        LOG(Error) << get_quanser_error_message(result);
        return false;
    }
}

bool QuanserDaq::set_options(const QuanserOptions& options) {
    options_ = options;
    char options_str[4096];
    std::strcpy(options_str, options_.get_string().c_str());
    t_error result;
    result = hil_set_card_specific_options(handle_, options_str, std::strlen(options_str));
    sleep(milliseconds(10));
    if (result == 0) {
        LOG(Verbose) << "Set " << get_name() << " options to: \"" << options_.get_string() << "\"";
        return true;
    }
    else {
        LOG(Error) << "Failed to set " << get_name() << " options to: \"" << options_.get_string() << "\" "
                   << get_quanser_error_message(result);
        return false;
    }
}

QuanserOptions QuanserDaq::get_options() {
    return options_;
}

std::size_t QuanserDaq::get_qdaq_count(const std::string& card_type) {
    std::size_t id = 0;
    std::vector<t_card> qdaqs;
    t_error result;
    while (true) {
        t_card qdaq;
        result = hil_open(card_type.c_str(), std::to_string(id).c_str(), &qdaq);
        if (result < 0) {
            break;
        }
        else {
            qdaqs.push_back(qdaq);
            ++id;
        }
    }
    for (size_t i = 0; i < qdaqs.size(); ++i) {
        hil_close(qdaqs[i]);
    }
    return id;
}

std::string QuanserDaq::get_quanser_error_message(int error, bool format) {
    TCHAR message[512];
    msg_get_error_message(NULL, error, message, sizeof(message));
    if (format)
        return "(Quanser Error #" + std::to_string(-error) + ": " + std::string(message) + ")";
    else
        return std::string(message);
}

} // namespace mel
