#include <MEL/Daq/Quanser/QWatchdog.hpp>
#include <MEL/Daq/Quanser/QDaq.hpp>
#include <MEL/Utility/Console.hpp>
#include <hil.h>

namespace mel {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

QWatchdog::QWatchdog(QDaq& daq, Time timeout) :
    Watchdog(timeout),
    daq_(daq)
{ }

QWatchdog::~QWatchdog() {

}

bool QWatchdog::start() {
    if (daq_.open_) {
        print("Starting QDaq " + namify(daq_.name_) + " watchdog.");
        t_error result;
        result = hil_watchdog_start(daq_.handle_, timeout_.as_seconds());
        if (result == 0) {
            watching_ = true;
            return true;
        }
        else {
            print(QDaq::get_quanser_error_message(result));
            return false;
        }
    }
    else {
        print("QDaq " + namify(daq_.name_) + " has not been opened; unable to call start_watchdog().");
        return false;
    }
}

bool QWatchdog::kick() {
    if (daq_.open_) {
        t_error result;
        result = hil_watchdog_reload(daq_.handle_);
        if (result == 1) {
            return true;
        }
        else if (result == 0) {
            watching_ = false;
            return false;
        }
        else {
            print(QDaq::get_quanser_error_message(result));
            return false;
        }
    }
    else {
        print("QDaq " + namify(daq_.get_name()) + " has not been opened; unable to call reload_watchdog().");
        return false;
    }
}

bool QWatchdog::stop() {
    t_error result;
    result = hil_watchdog_stop(daq_.handle_);
    if (result == 0) {
        watching_ = false;
        return true;
    }
    else {
        print(QDaq::get_quanser_error_message(result));
        return false;
    }
}

bool QWatchdog::is_expired() {
    t_error result;
    result = hil_watchdog_is_expired(daq_.handle_);
    if (result == 1) {
        watching_ = false;
        return true;
    }
    else if (result == 0)
        return false;
    else {
        print(QDaq::get_quanser_error_message(result));
        return false;
    }
}

bool QWatchdog::clear() {
    t_error result;
    result = hil_watchdog_clear(daq_.handle_);
    if (result == 0)
        return true;
    else {
        print(QDaq::get_quanser_error_message(result));
        return false;
    }
}

} // namespace mel
