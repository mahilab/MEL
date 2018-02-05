#include <hil.h>
#include <MEL/Daq/Quanser/QDaq.hpp>
#include <MEL/Daq/Quanser/QWatchdog.hpp>
#include <MEL/Logging/Log.hpp>

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
    if (!daq_.open_) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
            << daq_.get_name() << " is not open";
        return false;
    }
    t_error result;
    result = hil_watchdog_start(daq_.handle_, timeout_.as_seconds());
    if (result == 0) {
        LOG(Info) << "Started watchdog on " << daq_.get_name();
        return true;
    }
    else {
        LOG(Error) << "Failed to start watchdog on " << daq_.get_name() << " "
            << QDaq::get_quanser_error_message(result);

        return false;
    }
}

bool QWatchdog::kick() {
    if (!daq_.open_) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
            << daq_.get_name() << " is not open";
        return false;
    }
    t_error result;
    result = hil_watchdog_reload(daq_.handle_);
    if (result == 1) {
        return true;
    }
    else if (result == 0) {
        LOG(Warning) << "Watchdog on " << daq_.get_name() << " expired";
        watching_ = false;
        return false;
    }
    else {
        LOG(Error) << "Failed to kick watchdog on " << daq_.get_name() << " "
            << QDaq::get_quanser_error_message(result);
        return false;
    }
}

bool QWatchdog::stop() {
    if (!daq_.open_) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
            << daq_.get_name() << " is not open";
        return false;
    }
    t_error result;
    result = hil_watchdog_stop(daq_.handle_);
    if (result == 0) {
        LOG(Info) << "Stopped watchdog on " << daq_.get_name();
        watching_ = false;
        return true;
    }
    else {
        LOG(Error) << "Failed to stop watchdog on " << daq_.get_name() << " "
            << QDaq::get_quanser_error_message(result);
        return false;
    }
}

bool QWatchdog::is_expired() {
    if (!daq_.open_) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
            << daq_.get_name() << " is not open";
        return false;
    }
    t_error result;
    result = hil_watchdog_is_expired(daq_.handle_);
    if (result == 1) {
        watching_ = false;
        return true;
    }
    else if (result == 0) {
        return false;
    }
    else {
        LOG(Error) << "Failed to check expiration of watchdog on "
                   << daq_.get_name() << " "
                   << QDaq::get_quanser_error_message(result);
        return false;
    }
}

bool QWatchdog::clear() {
    if (!daq_.open_) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
            << daq_.get_name() << " is not open";
        return false;
    }
    t_error result;
    result = hil_watchdog_clear(daq_.handle_);
    if (result == 0) {
        LOG(Info) << "Cleared watchdog on " << daq_.get_name();
        return true;
    }
    else {
        LOG(Error) << "Failed to clear watchdog on " << daq_.get_name() << " "
            << QDaq::get_quanser_error_message(result);
        return false;
    }
}

} // namespace mel
