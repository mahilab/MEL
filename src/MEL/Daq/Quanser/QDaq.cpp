#include <MEL/Daq/Quanser/QDaq.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/System.hpp>
#include <quanser_messages.h>
#include <hil.h>
#include <tchar.h>

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
        return false;
    t_error result;
    // Try to open in 5 attempts
    for (int attempt = 0; attempt < 5; attempt++) {
        print("Opening " + namify(name_) + " (Attempt " + stringify(attempt + 1) + ") ... ", false);
        result = hil_open(card_type_.c_str(), std::to_string(id_).c_str(), &handle_);
        sleep(milliseconds(10));
        if (result == 0) {
            // TODO: velocity sanity check
            print("Done");
            return Daq::open();
        }
        else {
            print("Failed");
            print(get_quanser_error_message(result));
        }
    }
    return false;
}

bool QDaq::close() {
    if (!open_)
        return false;
    print("Closing " + namify(name_) + " ... ", false);
    t_error result;
    result = hil_close(handle_);
    sleep(milliseconds(10));
    if (result == 0) {
        print("Done");
        return Daq::close();
    }
    else {
        print("Failed");
        print(get_quanser_error_message(result));
        return false;
    }
}

bool QDaq::set_options() {
    if (open_) {
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
    else {
        print(namify(name_) + " has not been opened; unable to call start_watchdog().");
        return false;
    }
}

bool QDaq::set_options(const QOptions& options) {
    options_ = options;
    return set_options();
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
