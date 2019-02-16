#include <MEL/Logging/Log.hpp>

namespace mel {

#ifndef MEL_DISABLE_LOG
    static ColorConsoleWriter<TxtFormatter> default_console_writer(Info);
    Logger<DEFAULT_LOGGER>* MEL_LOGGER = &init_logger<DEFAULT_LOGGER>(Verbose, "MEL.log", 256000, 10).add_writer(&default_console_writer);
#else
    Logger<DEFAULT_LOGGER>* MEL_LOGGER = nullptr;
#endif

} // namespace mel
