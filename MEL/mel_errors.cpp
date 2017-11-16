#include "mel_errors.h"
#include "mel_util.h"
#include <cstdarg>

namespace mel {

    namespace util {

        bool MEL_DEBUG_MODE = false;
        std::deque<mel_error> MEL_ERROR_LOG;

        void enable_debug_mode() {
            MEL_DEBUG_MODE = true;
        }

        void disable_debug_mode() {
            MEL_DEBUG_MODE = false;
        }

        mel_error throw_error(mel_error error, ...) {
            va_list argptr;
            va_start(argptr, error);
            MEL_ERROR_LOG.push_back(error);
            if (MEL_DEBUG_MODE) {
                std::string message = "[" + get_ymdhms() + "]   ERROR   " + ERROR_MESSAGES[error] + 
                    " (error code " + std::to_string(error) + ")\n";
                vfprintf(stderr, message.c_str(), argptr);
                va_end(argptr);
            }
            return error;
        }

        std::string get_error_message(mel_error error) {
            return ERROR_MESSAGES[error];
        }

        mel_error get_last_error() {
            if (!MEL_ERROR_LOG.empty()) {
                return MEL_ERROR_LOG.back();
            }
        }

    }
}

