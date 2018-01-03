#pragma once
#include <string>
#include <deque>
#include <map>

namespace mel {

//---------------------------------------------------------------------
// ERROR HANDLING
//---------------------------------------------------------------------

/// List of possible erros in
enum mel_error {
    ERROR_NO_ERROR = 0,
    ERROR_QUARC_ERROR,
    ERROR_VIRTUAL_FUNCTION_NOT_IMPLEMENTED,
    ERROR_JOINT_TORQUE_LIMIT_EXCEEDED,
    ERROR_ACTUATOR_TORQUE_LIMIT_EXCEEDED,
    ERROR_MOTOR_HARD_CURRENT_LIMIIT_EXCEEDED,
    ERROR_MOTOR_I2T_CURRENT_LIMIT_EXCEEDED
};

/// Enable MEL's console level error messaging mode
void enable_debug_mode();

/// Disables MEL's console level warning messaging mode
void disable_debug_mode();

/// This is called when an error occurs. It logs error to MEL_ERROR_LOG
/// and prints the error to the console if MEL_DEBUG_LEVEL > 0.
mel_error throw_error(mel_error error, ...);

/// Returns the string message corresponding to a mel_error.
std::string get_error_message(mel_error error);

/// Returns the last error to be thrown.
mel_error get_last_error();

static std::map<mel_error, std::string> ERROR_MESSAGES =
{
    { ERROR_NO_ERROR,                           ""},
    { ERROR_QUARC_ERROR,                        "%s (QUARC error code %i)" },
    { ERROR_VIRTUAL_FUNCTION_NOT_IMPLEMENTED,   "Virtual function %s has not been implemented by child %s." },
    { ERROR_JOINT_TORQUE_LIMIT_EXCEEDED ,       "Joint <%s> command torque exceeded torque limit of %f with a value of %f." },
    { ERROR_ACTUATOR_TORQUE_LIMIT_EXCEEDED,     "Actuator <%s> command torque exceeded torque limit of %f with a value of %f." },
    { ERROR_MOTOR_HARD_CURRENT_LIMIIT_EXCEEDED, "Motor <%s> command current exceeded hard current limit of %f with a value of %f." },
    { ERROR_MOTOR_I2T_CURRENT_LIMIT_EXCEEDED,   "Motor <%s> command current triggered I^2*t current limit. Torque saturated to continous current limt of %f."}
};

} // namespace mel
