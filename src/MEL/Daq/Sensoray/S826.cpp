#include <MEL/Daq/Sensoray/S826.hpp>
#include <MEL/Logging/Log.hpp>
#include <windows.h>
#include <826api.h> 

namespace mel
{
    
S826::S826(int board) :
    DaqBase("s826_" + std::to_string(board)),
    board_(board),
    AO(*this)
{

}

S826::~S826() {

}

bool S826::on_open() {
    // open comms with all boards
    int detected_boards = S826_SystemOpen();
    print(detected_boards);
    if (detected_boards == S826_ERR_DUPADDR) {
        LOG(Error) << "More than one S826 board with same board number detected.";
        return false;
    }
    if (detected_boards == 0) {
        LOG(Error) << "No S826 boards detected.";
        return false;
    }
    return true;
}

bool S826::on_close() {
    // close comms with all boards
    S826_SystemClose();
    return true;
}

bool S826::on_enable() {
    return true;
}

bool S826::on_disable() {
    return true;
}

std::string S826::get_error_message(int error) {
    switch(error) {
        case S826_ERR_OK           : return "No error";
        case S826_ERR_BOARD        : return "Illegal board number";
        case S826_ERR_VALUE        : return "Illegal argument value";
        case S826_ERR_NOTREADY     : return "Device not ready or timeout waiting for device";
        case S826_ERR_CANCELLED    : return "Wait cancelled";
        case S826_ERR_DRIVER       : return "Driver call failed";
        case S826_ERR_MISSEDTRIG   : return "Missed adc trigger";
        case S826_ERR_DUPADDR      : return "Two boards set to same board number";
        case S826_ERR_BOARDCLOSED  : return "Board is not open";
        case S826_ERR_CREATEMUTEX  : return "Can't create mutex";
        case S826_ERR_MEMORYMAP    : return "Can't map board to memory address";
        case S826_ERR_MALLOC       : return "Can't allocate memory";
        case S826_ERR_FIFOOVERFLOW : return "Counter's snapshot fifo overflowed";
        case S826_ERR_LOCALBUS     : return "Can't read local bus (register contains illegal value)";
        case S826_ERR_OSSPECIFIC   : return "Port-specific error (base error number)";
        default                    : return "Unknown error code";
    }
}

} // namespace mel
