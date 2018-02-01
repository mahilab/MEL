#include <MEL/Daq/Quanser/QDaq.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/System.hpp>
#include <quanser_messages.h>
#include <hil.h>
#include <tchar.h>
#include <cstring>

namespace mel {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

QDaq::QDaq(const std::string& card_type, uint32 id, QOptions options) :
    Daq(card_type + "_" + std::to_string(id)),
    card_type_(card_type),
    id_(id),
    options_(options)
{
}

QDaq::~QDaq() {

}

bool QDaq::open() {
    if (open_)
        return false; // already open
    t_error result;
    // Try to open in 5 attempts
    for (int attempt = 0; attempt < 5; attempt++) {
        print("Opening " + namify(name_) + " (Attempt " + stringify(attempt + 1) + ") ... ");
        result = hil_open(card_type_.c_str(), std::to_string(id_).c_str(), &handle_);
        sleep(milliseconds(10));
        if (result == 0) {
            // successful open
            Daq::open();
            if (!set_options(options_)) {
                close();
                return false;
            }
            print("Opening " + namify(name_) + " (Attempt " + stringify(attempt + 1) + ") Succeeded");
            return true;
        } 
        else {
            // unsuccesful open, continue
            print("Opening " + namify(name_) + " (Attempt " + stringify(attempt + 1) + ") Failed");
            print(get_quanser_error_message(result));
        }
    }
    // all attempts to open were unsuccessful
    return false;
}

bool QDaq::close() {
    if (!open_)
        return false;
    print("Closing " + namify(name_) + " ... ");
    t_error result;
    result = hil_close(handle_);
    sleep(milliseconds(10));
    if (result == 0) {
        print("Done");
        return Daq::close();
    }
    else {
        print("Closing " + namify(name_) + "Failed");
        print(get_quanser_error_message(result));
        return false;
    }
}

bool QDaq::set_options(const QOptions& options) {
    if (!open_) {
        print(namify(get_name()) + " has not been opened; unable to call " + __FUNCTION__);
        return false;
    }
    options_ = options;
    print("Setting " + namify(name_) + " options ... ", false);
    char options_str[4096];
    std::strcpy(options_str, options_.get_string().c_str());
    t_error result;
    result = hil_set_card_specific_options(handle_, options_str, std::strlen(options_str));
    sleep(milliseconds(10));
    if (result == 0) {
        print("Done");
        return true;
    }
    else {
        print("Failed");
        print(get_quanser_error_message(result));
        return false;
    }
}

QOptions QDaq::get_options() {
    return options_;
}

std::size_t QDaq::get_qdaq_count(const std::string& card_type) {
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

std::string QDaq::get_quanser_error_message(int error) {
    TCHAR message[512];
    msg_get_error_message(NULL, error, message, sizeof(message));
    return std::string(message);
}

} // namespace mel
