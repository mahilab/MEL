#include <MEL/Logging/Csv.hpp>
#include <MEL/Logging/Log.hpp>
#include <thread>

namespace mel
{

    

Csv::Csv() : File() {}

Csv::Csv(const std::string &filepath, WriteMode w_mode, OpenMode o_mode) :
    File(filepath, w_mode, o_mode)
{

}

} // mel
