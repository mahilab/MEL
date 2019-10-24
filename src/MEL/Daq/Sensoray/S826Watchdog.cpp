#include <MEL/Daq/Sensoray/S826.hpp>
#include <MEL/Daq/Sensoray/S826Watchdog.hpp>
#include <MEL/Logging/Log.hpp>
#include <windows.h>
#include <826api.h> 
#include <bitset>

namespace mel {

S826Watchdog::S826Watchdog(S826& s826, Time timeout) : Watchdog(timeout), s826_(s826) {}

bool S826Watchdog::start() {
    uint32 microseconds = (uint32)timeout_.as_microseconds();
    uint32 wdtiming[5] = {microseconds * 50, 1, 1, 0, 0};
    int result;
    result = S826_SafeWrenWrite(s826_.board_, S826_SAFEN_SWE);
    if (result != S826_ERR_OK) {
        LOG(Error) << "Failed to enable safe write on " << s826_.get_name() << "(" << S826::get_error_message(result) << ")";
        return false;
    }
    result = S826_WatchdogConfigWrite(0, 0x10, wdtiming); // activate safemode upon Timer0 event.
    if (result != S826_ERR_OK) {
        LOG(Error) << "Failed to configure watchdog on " << s826_.get_name() << "(" << S826::get_error_message(result) << ")";
        return false;
    }
    result = S826_WatchdogEnableWrite(s826_.board_, 1);
    if (result != S826_ERR_OK) {
        LOG(Error) << "Failed to start watchdog on " << s826_.get_name() << "(" << S826::get_error_message(result) << ")";
        return false;
    }
    else {
        LOG(Verbose) << "Started watchdog on " << s826_.get_name();
        return true;
    }
}

bool S826Watchdog::kick() {
    int result = S826_WatchdogKick(s826_.board_, 0x5A55AA5A);
    if (result != S826_ERR_OK) {
        LOG(Error) << "Failed to kick watchdog on " << s826_.get_name() << "(" << S826::get_error_message(result) << ")";
        return false;
    }
    return true;
}

bool S826Watchdog::stop() {
    int result = S826_WatchdogEnableWrite(s826_.board_, 0);
    if (result != S826_ERR_OK) {
        LOG(Error) << "Failed to stop watchdog on " << s826_.get_name() << "(" << S826::get_error_message(result) << ")";
        return false;
    }
    else {
        LOG(Verbose) << "Stopped watchdog on " << s826_.get_name();
        return true;
    }
}

bool S826Watchdog::is_expired() {
    if (S826_WatchdogEventWait(s826_.board_, 0) == S826_ERR_OK)
        return true;
    return false;
}

bool S826Watchdog::clear() {
    int result;    
    result = S826_SafeWrenWrite(s826_.board_, S826_SAFEN_SWE);
    if (result != S826_ERR_OK) {
        LOG(Error) << "Failed to enable safe write on " << s826_.get_name() << "(" << S826::get_error_message(result) << ")";
        return false;
    }
    result = S826_SafeControlWrite(s826_.board_, 0, 0);
    if (result != S826_ERR_OK) {
        LOG(Error) << "Failed to stop clear watchdog on " << s826_.get_name() << "(" << S826::get_error_message(result) << ")";
        return false;
    }
    return true;
}

} // namespace mel